#!/bin/sh

# make fresh virtual disk
./fs_make.x disk.fs 4096

# create 11 files
for i in $(seq -w 1 10); do echo hello > file${i}; done
echo hi > file11

# add 10 files
for i in $(seq -w 1 10); do ./fs_ref.x add disk.fs file${i}; done

# remove file 5
./fs_ref.x rm disk.fs file05

# add file 11
./fs_ref.x add disk.fs file11

# get fs_ref.x ls
./fs_ref.x ls disk.fs >ref.stdout 2>ref.stderr

# remove all files
for i in $(seq -w 1 11); do ./fs_ref.x rm disk.fs file${i}; done

# add 10 files
for i in $(seq -w 1 10); do ./test_fs.x add disk.fs file${i}; done

# remove file 5
./test_fs.x rm disk.fs file05

# add file 11
./test_fs.x add disk.fs file11

# get test_fs.x ls
./test_fs.x ls disk.fs >lib.stdout 2>lib.stderr

# remove all files
for i in $(seq -w 1 11); do ./test_fs.x rm disk.fs file${i}; done

# put output files into variables
REF_STDOUT=$(cat ref.stdout)
REF_STDERR=$(cat ref.stderr)

LIB_STDOUT=$(cat lib.stdout)
LIB_STDERR=$(cat lib.stderr)

# compare stdout
if [ "$REF_STDOUT" != "$LIB_STDOUT" ]; then
    echo "Stdout outputs don't match..."
    diff -u ref.stdout lib.stdout
else
    echo "Stdout outputs match!"
fi

# compare stderr
if [ "$REF_STDERR" != "$LIB_STDERR" ]; then
    echo "Stderr outputs don't match..."
    diff -u ref.stderr lib.stderr
else
    echo "Stderr outputs match!"
fi

# clean
rm disk.fs
rm ref.stdout ref.stderr
rm lib.stdout lib.stderr
for i in $(seq -w 1 11); do rm file${i}; done