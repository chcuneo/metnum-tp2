#!/bin/bash      
mkdir /tmp/ccuneoMN3/
mkdir /tmp/ccuneoMN3/bin/
mkdir /tmp/ccuneoMN3/inputs/
mkdir /tmp/ccuneoMN3/data/
mkdir ./bin/
cp -rf ./src/tests/ /tmp/ccuneoMN3/
wget -x --load-cookies cookies.txt -P /tmp/ccuneoMN3/data/ https://www.kaggle.com/c/digit-recognizer/download/train.csv
