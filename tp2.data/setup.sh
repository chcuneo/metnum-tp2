#!/bin/bash      
mkdir /tmp/ccuneoMN3/
mkdir /tmp/ccuneoMN3/bin/
mkdir /tmp/ccuneoMN3/inputs/
mkdir /tmp/ccuneoMN3/data/
cp -rf ./src/tests/ /tmp/ccuneoMN3/inputs/
wget -x --load-cookies -P /tmp/ccuneoMN3/data/ cookies.txt https://www.kaggle.com/c/digit-recognizer/download/train.csv
