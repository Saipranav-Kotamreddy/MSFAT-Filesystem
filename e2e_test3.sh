#!/bin/bash
cd apps
dd if=/dev/urandom of=test_file bs=4096 count=1 
./fs_make.x disk_for_our_fs3.fs 8192
./test_fs.x info disk_for_our_fs3.fs > given_output3
./fs_ref.x info disk_for_our_fs3.fs > ref_output3
DIFF=$(diff given_output3 ref_output3) 
if [ "$DIFF" != "" ]
then
    echo "Fails: $DIFF"
    exit 1
else
    echo "Success!"
fi
./test_fs.x ls disk_for_our_fs3.fs > ls_given_output3
./fs_ref.x ls disk_for_our_fs3.fs > ls_ref_output3
DIFF=$(diff ls_given_output3 ls_ref_output3) 
if [ "$DIFF" != "" ]
then
    echo "Fails: $DIFF"
    exit 1
else
    echo "Success!"
fi