#!/bin/sh

# Error test case
../bin/Debug/cpp_parser -Il ./ -Ig /usr/include/c++/4.4 -Ig /usr/include -Ig /usr/include/c++/4.4/x86_64-linux-gnu -Ig /usr/include/c++/4.4/tr1 -Ig /usr/include/linux ./error.hpp > ./output.txt

cat output.txt
