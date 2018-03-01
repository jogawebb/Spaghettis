#! /usr/bin/env bash

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------

# Workaround for annoying GCC 4.2.1 bug ( https://gcc.gnu.org/bugzilla/show_bug.cgi?id=33144 ).
# Avoid march=native flag on Mac OS X 10.6 system.

if [ "$OSTYPE" != "darwin10.0" ]; then

g++ -std=c++11 -march=native main.cpp -I../libraries/belle/Source -O3 -ffast-math -ldl -lpthread -lm -o tests

else

g++ main.cpp -I../libraries/belle/Source -O3 -ffast-math -ldl -lpthread -lm -o tests

fi

# ------------------------------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------------------------------
