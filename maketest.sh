#!/bin/bash

mkdir test
cd test


echo test > test.txt
wget https://httpbin.org/image/png
mv png image.png


mkdir a_lot_of_dirs
cd a_lot_of_dirs

for i in {1..100..1}; do mkdir $i; cd $i; done

echo hurraaay! > congrats.txt

for i in {1..101..1}; do cd ..; done


mkdir large_files
cd large_files


yes 1 | dd of=1GiB bs=1024 count=$((2 ** 20)) status=progress
yes 2 | dd of=2GiB bs=1024 count=$((2 * $((2 ** 20)))) status=progress
yes 4 | dd of=4GiB bs=1024 count=$((4 * $((2 ** 20)))) status=progress


cd ..
mkdir weird_names
cd weird_names


echo name1 > ' 199;'
echo name2 > ./'-f'
echo name3 > 's p a c e'
