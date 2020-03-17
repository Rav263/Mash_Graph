#! /bin/bash

rm out.ppm
cmake ..

make -j4

time ./rt

feh out_ant.ppm
