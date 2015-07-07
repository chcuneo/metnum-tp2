#!/bin/bash      
mkdir /tmp/ccuneoMN2/
mkdir /tmp/ccuneoMN2/bin/
mkdir /tmp/ccuneoMN2/inputs/
mkdir /tmp/ccuneoMN2/data/
mkdir ./bin/
cp -rf ./src/tests/ /tmp/ccuneoMN2/
wget -x --load-cookies cookies.txt -P /tmp/ccuneoMN2/data/ https://www.kaggle.com/c/digit-recognizer/download/train.csv
mv /tmp/ccuneoMN2/data/www.kaggle.com/c/digit-recognizer/download/train.csv /tmp/ccuneoMN2/data/train.csv
rm -r /tmp/ccuneoMN2/data/www.kaggle.com/