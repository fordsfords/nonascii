/* nonascii.c */
/*
# This code and its documentation is Copyright 2021 Steven Ford, http://geeky-boy.com
# and licensed "public domain" style under Creative Commons "CC0": http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/nonascii
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

/* See https://github.com/fordsfords/safe_atoi */
#define SAFE_ATOI(a_,l_) do { \
  unsigned long long fs_[9] = { \
    0, 0xff, 0xffff, 0, 0xffffffff, 0, 0, 0, 0xffffffffffffffff }; \
  errno = 0; \
  (l_) = fs_[sizeof(l_)]; \
  if ((l_) < 0) { /* Is result a signed value? */ \
    char *in_a_ = a_;  char *temp_ = NULL;  long long llresult_; \
    if (strlen(in_a_) > 2 && *in_a_ == '0' && *(in_a_ + 1) == 'x') { \
      llresult_ = strtoll(in_a_ + 2, &temp_, 16); \
    } else { \
      llresult_ = strtoll(in_a_, &temp_, 10); \
    } \
    if (errno != 0 || temp_ == in_a_ || temp_ == NULL || *temp_ != '\0') { \
      if (errno == 0) { \
        errno = EINVAL; \
      } \
      fprintf(stderr, "%s:%d, Error, invalid number for %s: '%s'\n", \
         __FILE__, __LINE__, #l_, in_a_); \
      exit(1); \
    } else { /* strtol thinks success; check for overflow. */ \
      (l_) = llresult_; /* "return" value of macro */ \
      if ((l_) != llresult_) { \
        fprintf(stderr, "%s:%d, %s over/under flow: '%s'\n", \
           __FILE__, __LINE__, #l_, in_a_); \
        exit(1); \
      } \
    } \
  } else { \
    char *in_a_ = a_;  char *temp_ = NULL;  unsigned long long llresult_; \
    if (strlen(in_a_) > 2 && *in_a_ == '0' && *(in_a_ + 1) == 'x') { \
      llresult_ = strtoull(in_a_ + 2, &temp_, 16); \
    } else { \
      llresult_ = strtoull(in_a_, &temp_, 10); \
    } \
    if (errno != 0 || temp_ == in_a_ || temp_ == NULL || *temp_ != '\0') { \
      if (errno == 0) { \
        errno = EINVAL; \
      } \
      fprintf(stderr, "%s:%d, Error, invalid number for %s: '%s'\n", \
         __FILE__, __LINE__, #l_, in_a_); \
      exit(1); \
    } else { /* strtol thinks success; check for overflow. */ \
      (l_) = llresult_; /* "return" value of macro */ \
      if ((l_) != llresult_) { \
        fprintf(stderr, "%s:%d, %s over/under flow: '%s'\n", \
           __FILE__, __LINE__, #l_, in_a_); \
        exit(1); \
      } \
    } \
  } \
} while (0)

/* Options */
int o_num_reports = 0;  /* -n */


char usage_str[] = "Usage: nonascii [-h] [-n num_reports] [--] [in_file ...]";
void usage_err(char *msg) {
  if (msg) fprintf(stderr, "\n%s\n\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}  /* usage_err */

void help() {
  printf("%s\n"
    "where:\n"
    "  -h - print help\n"
    "  -- - indicates end of options.\n"
    "  -n num_reports - only print up to 'num_reports' lines of output\n"
    "    for each input file. 0 means no limit (infinite).\n"
    "  in_file - zero or more text files. If none, use STDIN. A file named\n"
    "    '-' indicates to read standard input.\n"
    "For details, see https://github.com/fordsfords/nonascii\n",
    usage_str);
  exit(0);
}  /* help */


void parse_cmdline(int argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "h-n:")) != EOF) {
    switch (opt) {
      case 'h':
        help();
        break;
      case '-':
printf("???dash\n");
        return;  /* Done processing options. */
        break;
      case 'n':
        SAFE_ATOI(optarg, o_num_reports);
        break;
      default:
        usage_err(NULL);
    }  /* switch */
  }  /* while */
}  /* parse_cmdline */


/* This function is not thread-safe. */
void process_open_file(FILE *in_fd, char *in_file)
{
  #define BUFFER_SZ 65536
  static char buffer[BUFFER_SZ];
  size_t numbytes_read;
  long line_num;
  long char_offset;
  int num_reports;
  int done;

  line_num = 1;
  char_offset = 1;
  num_reports = 0;
  done = 0;
  while (!done) {
    size_t i;
    numbytes_read = fread(buffer, 1, sizeof(buffer), in_fd);
    if (numbytes_read < sizeof(buffer) || feof(in_fd) || ferror(in_fd)) {
      done = 1;
    }

    for (i = 0; i < numbytes_read; i++) {
      char c = buffer[i];
      char_offset++;

      if ((c & 0x80) != 0) {  /* if non-ascii print report. */
        printf("nonascii: %s:%ld[%ld]: 0x%02hhx\n",
            in_file, line_num, char_offset, c);
        num_reports ++;
        if (o_num_reports > 0 && num_reports >= o_num_reports) {
          return;  /* Reached limit of reports, done with file. */
        }
      }
      else if (c == '\n') {
        line_num++;
        char_offset = 1;
      }
    }  /* for i */
  }  /* while !done */
  if (ferror(in_fd)) {
    fprintf(stderr, "Error reading file '%s'\n", in_file);
  }
}  /* process_open_file */


void process_named_file(char *in_file)
{
  if (strcmp(in_file, "-") == 0) {
    process_open_file(stdin, "(STDIN)");
  } else {
    FILE *in_fp = fopen(in_file, "rb");
    if (in_fp == NULL) {
      fprintf(stderr, "ERROR: file '%s' could not be opened\n", in_file);
      usage_err(NULL);
    }

    process_open_file(in_fp, in_file);

    fclose(in_fp);
  }
}  /* process_named_file */


int main(int argc, char **argv)
{
  parse_cmdline(argc, argv);

  if (optind == argc) {  /* If no file names supplied, process STDIN. */
    process_open_file(stdin, "(STDIN)");
  }
  else {
    while (optind < argc) {
      process_named_file(argv[optind]);
      optind ++;
    }
  }

  return 0;
}  /* main */
