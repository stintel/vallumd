#!/bin/sh

pwd
ls -la
cmake .
cpack -G DEB
