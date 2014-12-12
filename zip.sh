#/bin/bash

cd src/

7z a -tzip Unknown.zip *.cpp *.hpp *.c *.h

mv Unknown.zip ../
