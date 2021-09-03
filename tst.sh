#!/bin/bash
# tst.sh - build and test the project.

ASSRT() {
  eval "test $1"

  if [ $? -ne 0 ]; then
    echo "ASSRT ERROR `basename ${BASH_SOURCE[1]}`:${BASH_LINENO[0]}, not true: '$1'" >&2
    exit 1
  fi
  ASSERTS=`expr $ASSERTS + 1`
}  # ASSRT

ASSERTS=0

gcc -Wall -o nonascii nonascii.c ; ASSRT "$? -eq 0"

./nonascii -h >x.out ; ASSRT "$? -eq 0"
egrep "^[Ww]here:" x.out >/dev/null ; ASSRT "$? -eq 0"

xxd -r <short.hex >x.short
./nonascii >x.out x.short ; ASSRT "$? -eq 0"
LINES=`wc -l <x.out`; ASSRT "$LINES -eq 2"

xxd -r <short.hex >x.short
./nonascii -n 2 >x.out x.short ; ASSRT "$? -eq 0"
LINES=`wc -l <x.out`; ASSRT "$LINES -eq 2"

xxd -r <short.hex >x.short
./nonascii -n 1 >x.out x.short ; ASSRT "$? -eq 0"
LINES=`wc -l <x.out`; ASSRT "$LINES -eq 1"

xxd -r <long.hex >x.long
./nonascii >x.out x.long ; ASSRT "$? -eq 0"
LINES=`wc -l <x.out`; ASSRT "$LINES -eq 1"

./nonascii -n 1 >x.out <x.long x.short - x.short; ASSRT "$? -eq 0"
LINES=`wc -l <x.out`; ASSRT "$LINES -eq 3"

echo "Success: $ASSERTS asserts"
