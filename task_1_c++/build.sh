#! /bin/bash
rm rectangle.bmp
g++ main.cpp Bitmap.cpp -O3

./a.out
feh rectangle.bmp
