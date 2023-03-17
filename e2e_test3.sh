#!/bin/bash
cd apps
dd if=/dev/urandom of=test_file bs=4096 count=1 
./fs_make.x big_disk.fs 8192
./fs_make.x small_disk.fs 50
./fs_make.x middle_disk.fs 341

./test_fs.x info small_disk.fs > given_output3
./fs_ref.x info small_disk.fs > ref_output3
DIFF=$(diff given_output3 ref_output3) 
if [ "$DIFF" != "" ]
then
    echo "Fails small: $DIFF"
    exit 1
else
    echo "Success!"
fi

./test_fs.x info big_disk.fs > given_output3
./fs_ref.x info big_disk.fs > ref_output3
DIFF=$(diff given_output3 ref_output3) 
if [ "$DIFF" != "" ]
then
    echo "Fails big: $DIFF"
    exit 1
else
    echo "Success!"
fi

./test_fs.x info middle_disk.fs > given_output3
./fs_ref.x info middle_disk.fs > ref_output3
DIFF=$(diff given_output3 ref_output3) 
if [ "$DIFF" != "" ]
then
    echo "Fails middle: $DIFF"
    exit 1
else
    echo "Success!"
fi


./test_fs.x ls middle_disk.fs > ls_given_output3
./fs_ref.x ls middle_disk.fs > ls_ref_output3
DIFF=$(diff ls_given_output3 ls_ref_output3) 
if [ "$DIFF" != "" ]
then
    echo "Fails: $DIFF"
    exit 1
else
    echo "Success!"
fi