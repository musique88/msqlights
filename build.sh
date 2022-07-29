#!/bin/bash

clang++ src/*.cpp tinyosc/tinyosc.c -lraylib -lola -lolacommon -g -omsqlights -I. -Wall
