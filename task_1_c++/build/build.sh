#! /bin/bash

rm out.ppm
rm ./rt
cmake ..

make -j4

time ./rt $1 $2 $3 $4 $5 $6 $7 $8 $9 $10

feh out.ppm
