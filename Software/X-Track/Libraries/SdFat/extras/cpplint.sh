#!/bin/sh
export PATH=/cygdrive/c/Python27:/cygdrive/c/Python27/DLLs:/cygdrive/c/Python27/Scripts:$PATH
echo $PATH
python cpplint.py --filter=-build/include,-runtime/references,-build/header_guard ../src/*.* ../src/*/*.* 2>cpplint.txt