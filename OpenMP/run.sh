#! /bin/sh
#
# build.sh
# Copyright (C) 2018 Lucas Costa Campos <rmk236@gmail.com>
#
# Distributed under terms of the MIT license.
#

export OMP_NUM_THREADS=3
for name in $(find -name "*.cpp" | sed -e "s:.cpp::"); do
    echo $name
    ./$name
done
