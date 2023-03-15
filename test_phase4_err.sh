#!/bin/bash
cd apps 
dd if=/dev/urandom of=test_file bs=4096 count=1
./fs_make.x test.fs 6 

echo "Creating file:"
echo -e "MOUNT\nCREATE\tfile\nCREATE\tfile2\nCREATE\tfile3\nUMOUNT\n" > something.script
cat something.script
./fs_ref.x script test.fs something.script

./test_phase4_err.x

echo "Testing read:"
echo -e "MOUNT\nOPEN\tfile\nREAD\t1368\tFILE\ttest_file1.txt\n" > something.script
./fs_ref.x script test.fs something.script

echo -e "MOUNT\nOPEN\tfile2\nREAD\t9034\tFILE\ttest_file2.txt\n" > something.script
./fs_ref.x script test.fs something.script

head test_file3.txt -c 4096 > test_file3_small.txt

echo -e "MOUNT\nOPEN\tfile3\nREAD\t4096\tFILE\ttest_file3_small.txt\n" > something.script
./fs_ref.x script test.fs something.script

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

