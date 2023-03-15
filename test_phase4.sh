#!/bin/bash
cd apps 
dd if=/dev/urandom of=test_file bs=4096 count=1 &> /dev/null
./fs_make.x test.fs 100 &> /dev/null

echo "Creating file:"
echo -e "MOUNT\nCREATE\tfile\nCREATE\tfile2\nCREATE\tfile3\nUMOUNT\n" > something.script
./fs_ref.x script test.fs something.script &> /dev/null

./test_phase4.x &> /dev/null
if [ $? != 0 ]
then
    echo "Phase 4 error test failed"
    exit
fi

echo "Testing read:"
echo -e "MOUNT\nOPEN\tfile\nREAD\t1368\tFILE\ttest_file1.txt\nCLOSE\nUMOUNT\n" > something.script
./fs_ref.x script test.fs something.script 
if [ $? != 0 ]
then
    echo "Phase 4 error test failed"
    exit
fi


echo -e "MOUNT\nOPEN\tfile2\nREAD\t9034\tFILE\ttest_file2.txt\nCLOSE\nUMOUNT\n" > something.script
./fs_ref.x script test.fs something.script
if [ $? != 0 ]
then
    echo "Phase 4 error test failed"
    exit
fi


echo -e "MOUNT\nOPEN\tfile3\nREAD\t19000\tFILE\ttest_file3.txt\nCLOSE\nUMOUNT\n" > something.script
./fs_ref.x script test.fs something.script
if [ $? != 0 ]
then
    echo "Phase 4 error test failed"
    exit
fi

echo "Phase 4 tests successful!"
# # ----- Old version ------------
# #!/bin/bash
# cd apps 
# dd if=/dev/urandom of=test_file bs=4096 count=1
# ./fs_make.x test.fs 100 
# echo "Creating file:"
# echo -e "MOUNT\nCREATE\tfile\nUMOUNT\n" > something.script
# cat something.script
# ./fs_ref.x script test.fs something.script

# # echo "Writing to file:"
# # echo -e "MOUNT\nOPEN\tfile\nWRITE\tDATA\ta\nCLOSE\nUMOUNT\n" > something.script
# # cat something.script
# # ./fs_ref.x script test.fs something.script

# ./test_phase4.x


# echo "Testing read:"
# echo -e "MOUNT\nOPEN\tfile\nREAD\t10\tDATA\taaaaaaaaaa\n" > something.script
# cat something.script
# ./fs_ref.x script test.fs something.script

