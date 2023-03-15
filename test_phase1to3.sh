#!/bin/bash
cd apps 
dd if=/dev/urandom of=test_file bs=4096 count=1 &> /dev/null
./fs_make.x test.fs 100  &> /dev/null
./test_phase1.x
if [ $? != 0 ]
then
    echo "Phase 1 failed"
    exit
fi

dd if=/dev/urandom of=test_file bs=4096 count=1 &> /dev/null
./fs_make.x test.fs 100 &> /dev/null
./test_phase2_part1.x
if [ $? != 0 ]
then
    echo "Phase 2 part 1 failed"
    exit
fi

dd if=/dev/urandom of=test_file bs=4096 count=1 &> /dev/null
./fs_make.x test.fs 100 &> /dev/null
./test_phase2_part2.x
if [ $? != 0 ]
then
    echo "Phase 2 part 2 failed"
    exit
fi


dd if=/dev/urandom of=test_file bs=4096 count=1 &> /dev/null
./fs_make.x test.fs 100  &> /dev/null
./test_phase3_part1.x 

if [ $? != 0 ]
then
    echo "Phase 3 part 1 failed"
    exit
fi

dd if=/dev/urandom of=test_file bs=4096 count=1 &> /dev/null
./fs_make.x test.fs 100 &> /dev/null
./test_phase3_part2.x

if [ $? != 0 ]
then
    echo "Phase 3 part 2 failed"
    exit
fi

echo "All phases succeeded!"