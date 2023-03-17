#!/bin/bash
cd apps 
dd if=/dev/urandom of=test_file bs=4096 count=1 &> /dev/null
./fs_make.x test.fs 100  &> /dev/null

echo "Creating file:"
echo -e "MOUNT\nCREATE\tfile\nCREATE\tfile2\nCREATE\tfile3\nUMOUNT\n" > something.script
./fs_ref.x script test.fs something.script &> /dev/null

./test_phase4_read.x
if [ $? != 0 ]
then
    echo "Phase 4 read failed"
    exit 1
fi