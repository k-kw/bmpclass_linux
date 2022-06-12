cmake_minimum_required(VERSION 3.13)

project(bmp_so CXX)

include_directories(/usr/include /usr/local/include /usr/local/fftw3/include)

add_library(bmp_class SHARED Bmp_class.cpp)