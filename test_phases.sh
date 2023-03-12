#!/bin/bash
cd apps 
dd if=/dev/urandom of=test_file bs=4096 count=1
./fs_make.x test.fs 100 
./test_phase4.x
#phase_tests="./test_phase4.x"

# for test in $phase_tests
# do
#     ./$test &> /dev/null
#     if [ $? != 0 ] 
#     then
#         echo "Failed for $test"
#         exit 1
#     fi
# done

# echo "Success for phase test files!"
