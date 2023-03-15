# FROM gcc:latest

# WORKDIR /proj
# COPY . .
# # RUN gcc -o DockerWorld dockerworld.c

# # CMD [“./DockerWorld”]


############### For testing ls ###############

# FROM ubuntu:latest
# RUN apt-get update
# RUN apt-get install -y libssl-dev
# RUN apt-get install make
# WORKDIR /proj
# COPY ./apps .
# RUN dd if=/dev/urandom of=test_file bs=4096 count=1 ; ./fs_make.x test.fs 100 ; 
# RUN echo "MOUNT\nCREATE\tfile\nCREATE\tfile2\nOPEN\tfile\nWRITE\tDATA\t00000\nCLOSE\n" > something.script ; ./fs_ref.x script test.fs something.script
# RUN ./fs_ref.x ls test.fs
# RUN ./test_fs.x ls test.fs

##############################################


# RUN echo "MOUNT\nOPEN\tfile\WRITE\tDATA\t00000\nCLOSE\n" > something.script ; ./fs_ref.x script test.fs something.script
# RUN ./fs_ref.x ls test.fs
# RUN ./test_fs.x ls test.fs
# RUN cd apps ; dd if=/dev/urandom of=test_file bs=4096 count=1 ; ./fs_make.x test.fs 100 
# RUN cd apps ; echo -e "MOUNT\nCREATE\tfile\nCREATE\tfile2\n" > something.script ; ./fs_ref.x script test.fs something.script
# RUN cd apps ; ./fs_ref.x ls test.fs
# RUN cd apps ; ./test_fs.x ls test.fsRUN cd apps ; dd if=/dev/urandom of=test_file bs=4096 count=1 ; ./fs_make.x test.fs 100 ; ./fs_ref.x script test.fs scripts/lsexample.script 
#
#RUN cd apps ; dd if=/dev/urandom of=test_file bs=4096 count=1 ; ./fs_make.x test.fs 8192 ; ./fs_ref.x info test.fs ; ./fs_ref.x script test.fs scripts/lsexample.script
#RUN cd apps ; dd if=/dev/urandom of=test_file bs=4096 count=1 ; ./fs_make.x test.fs 100 ; ./test_fs.x script test.fs scripts/phase2_test.script
#RUN cd apps ; dd if=/dev/urandom of=test_file bs=4096 count=1 ; ./fs_make.x test.fs 100 ; ./test_phase2.x 

FROM ubuntu:latest
RUN apt-get update
RUN apt-get install -y libssl-dev
RUN apt-get install make
RUN apt-get install -y gdb
WORKDIR /proj
COPY . .
ADD "https://www.random.org/cgi-bin/randbyte?nbytes=10&format=h" skipcache
RUN ./test_phase1to3.sh
RUN ./test_phase4.sh
RUN ./test_phase4_err.sh
# RUN cd apps ; dd if=/dev/urandom of=test_file bs=4096 count=1 ; ./fs_make.x test_ls.fs 100 ; ./fs_make.x test_ls2.fs 100 
# RUN cd apps ; echo "MOUNT\nCREATE\tfile\nCREATE\tfile2\n" > something.script ; ./fs_ref.x script test_ls.fs something.script > ref_output ; ./test_fs.x script test_ls2.fs something.script > output ; diff ref_output output > sizeofidff ;  wc -l sizeofidff


#; ./test_phase3_part1.x ; ./test_phase3_part2.x
