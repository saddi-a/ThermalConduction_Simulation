#!/bin/csh

make Install

cp -r ../1D/data ./

echo "compile done"

./main.out

echo "run done"

cd data
python3 dispRes1D.py

echo "data visualisation done"
