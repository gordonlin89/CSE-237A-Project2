#!/bin/sh

make -q || make

test -e plot || mkdir plot

echo mock1
./mock2.out -mock 1 > plot/mock1.dat

echo mock2
./mock2.out -mock 2 > plot/mock2.dat

echo mock3
./mock2.out -mock 3 > plot/mock3.dat

echo plotting
gnuplot mock.gp
