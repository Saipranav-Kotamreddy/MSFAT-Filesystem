#!/bin/bash
cd apps
dd if=/dev/urandom of=test_file bs=4096 count=1 
./fs_make.x disk_for_our_fs.fs 100
./fs_make.x disk_for_ref_fs.fs 100
echo -e "$(<scripts/example.script )" > testscript
./test_fs.x script disk_for_our_fs.fs testscript > given_output
./fs_ref.x script disk_for_ref_fs.fs testscript > ref_output
DIFF=$(diff given_output ref_output) 
if [ "$DIFF" != "" ]
then
    echo "Fails: $DIFF"
else
    echo "Success!"
fi
