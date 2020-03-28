#! /bin/bash

cmake ..
make -j8
./rt
feh rectangle.bmp
rm rectangle.bmp

