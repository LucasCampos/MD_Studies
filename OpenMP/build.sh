#! /bin/sh
#
# build.sh
# Copyright (C) 2018 Lucas Costa Campos <rmk236@gmail.com>
#
# Distributed under terms of the MIT license.
#

for name in $(find -name "*.cpp" | sed -e "s:.cpp::"); do
    echo $name
    g++ $name.cpp -o $name -I ../ -O3 -march=native -fopenmp
done
