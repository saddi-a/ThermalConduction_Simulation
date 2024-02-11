#!/bin/csh

make Install

cp -r ../1D/data ./
mkdir data/Results

echo "compile done"

./main.out

echo "run done"

cd data
python3 dispRes1D.py
python3 TypesDeMateriauxAffichage.py 

echo "data visualisation done"
