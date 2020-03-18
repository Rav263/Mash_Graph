#! /bin/bash

rm out.ppm
rm ./rt
cmake ..

make -j4

time ./rt

feh out_ant.ppm
