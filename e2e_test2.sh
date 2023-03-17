#!/bin/bash
cd apps
dd if=/dev/urandom of=test_file bs=4096 count=1 
./fs_make.x disk_for_our_fs2.fs 100
./fs_make.x disk_for_ref_fs2.fs 100
echo -e "$(<scripts/example2.script )" > testscript
./test_fs.x script disk_for_our_fs2.fs testscript > given_output
./fs_ref.x script disk_for_ref_fs2.fs testscript > ref_output
DIFF=$(diff given_output ref_output) 
if [ "$DIFF" != "" ]
then
    echo "Fails: $DIFF"
    exit 1
else
    echo "Success!"
fi
./test_fs.x ls disk_for_our_fs2.fs > ls_given_output2
./fs_ref.x ls disk_for_our_fs2.fs > ls_ref_output2
DIFF=$(diff ls_given_output2 ls_ref_output2) 
if [ "$DIFF" != "" ]
then
    echo "Fails: $DIFF"
    exit 1
else
    echo "Success!"
fi