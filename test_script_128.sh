#!/bin/sh

# make fresh virtual disk
./fs_make.x disk.fs 4096

# create 33 files
for i in $(seq -w 1 128); do touch file${i}; done

# get fs_info from reference lib
for i in $(seq -w 1 128); do ./fs_ref.x add disk.fs file${i}; done
./fs_ref.x ls disk.fs >ref.stdout 2>ref.stderr

for i in $(seq -w 1 128); do ./fs_ref.x rm disk.fs file${i}; done

# get fs_info from my lib
for i in $(seq -w 1 128); do ./test_fs.x add disk.fs file${i}; done
./test_fs.x ls disk.fs >lib.stdout 2>lib.stderr
for i in $(seq -w 1 128); do ./test_fs.x rm disk.fs file${i}; done


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
for i in $(seq -w 1 128); do rm file${i}; done