#!/bin/sh

# make fresh virtual disk
./fs_make.x disk.fs 4096

# create 33 files
touch file1
touch file2
touch file3
touch file4
touch file5
touch file6
touch file7
touch file8
touch file9
touch file10
touch file11
touch file12
touch file13
touch file14
touch file15
touch file16
touch file17
touch file18
touch file19
touch file20
touch file21
touch file22
touch file23
touch file24
touch file25
touch file26
touch file27
touch file28
touch file29
touch file30
touch file31
touch file32
touch file33
touch file34
touch file35

# get fs_info from reference lib
./fs_ref.x add disk.fs file1
./fs_ref.x add disk.fs file2
./fs_ref.x add disk.fs file3
./fs_ref.x add disk.fs file4
./fs_ref.x add disk.fs file5
./fs_ref.x add disk.fs file6
./fs_ref.x add disk.fs file7
./fs_ref.x add disk.fs file8
./fs_ref.x add disk.fs file9
./fs_ref.x add disk.fs file10
./fs_ref.x add disk.fs file11
./fs_ref.x add disk.fs file12
./fs_ref.x add disk.fs file13
./fs_ref.x add disk.fs file14
./fs_ref.x add disk.fs file15
./fs_ref.x add disk.fs file16
./fs_ref.x add disk.fs file17
./fs_ref.x add disk.fs file18
./fs_ref.x add disk.fs file19
./fs_ref.x add disk.fs file20
./fs_ref.x add disk.fs file21
./fs_ref.x add disk.fs file22
./fs_ref.x add disk.fs file23
./fs_ref.x add disk.fs file24
./fs_ref.x add disk.fs file25
./fs_ref.x add disk.fs file26
./fs_ref.x add disk.fs file27
./fs_ref.x add disk.fs file28
./fs_ref.x add disk.fs file29
./fs_ref.x add disk.fs file30
./fs_ref.x add disk.fs file31
./fs_ref.x add disk.fs file32
./fs_ref.x add disk.fs file33
./fs_ref.x add disk.fs file34
./fs_ref.x add disk.fs file35
./fs_ref.x ls disk.fs >ref.stdout 2>ref.stderr
./fs_ref.x rm disk.fs file1
./fs_ref.x rm disk.fs file2
./fs_ref.x rm disk.fs file3
./fs_ref.x rm disk.fs file4
./fs_ref.x rm disk.fs file5
./fs_ref.x rm disk.fs file6
./fs_ref.x rm disk.fs file7
./fs_ref.x rm disk.fs file8
./fs_ref.x rm disk.fs file9
./fs_ref.x rm disk.fs file10
./fs_ref.x rm disk.fs file11
./fs_ref.x rm disk.fs file12
./fs_ref.x rm disk.fs file13
./fs_ref.x rm disk.fs file14
./fs_ref.x rm disk.fs file15
./fs_ref.x rm disk.fs file16
./fs_ref.x rm disk.fs file17
./fs_ref.x rm disk.fs file18
./fs_ref.x rm disk.fs file19
./fs_ref.x rm disk.fs file20
./fs_ref.x rm disk.fs file21
./fs_ref.x rm disk.fs file22
./fs_ref.x rm disk.fs file23
./fs_ref.x rm disk.fs file24
./fs_ref.x rm disk.fs file25
./fs_ref.x rm disk.fs file26
./fs_ref.x rm disk.fs file27
./fs_ref.x rm disk.fs file28
./fs_ref.x rm disk.fs file29
./fs_ref.x rm disk.fs file30
./fs_ref.x rm disk.fs file31
./fs_ref.x rm disk.fs file32
./fs_ref.x rm disk.fs file33
./fs_ref.x rm disk.fs file34
./fs_ref.x rm disk.fs file35

# get fs_info from my lib
./test_fs.x add disk.fs file1
./test_fs.x add disk.fs file2
./test_fs.x add disk.fs file3
./test_fs.x add disk.fs file4
./test_fs.x add disk.fs file5
./test_fs.x add disk.fs file6
./test_fs.x add disk.fs file7
./test_fs.x add disk.fs file8
./test_fs.x add disk.fs file9
./test_fs.x add disk.fs file10
./test_fs.x add disk.fs file11
./test_fs.x add disk.fs file12
./test_fs.x add disk.fs file13
./test_fs.x add disk.fs file14
./test_fs.x add disk.fs file15
./test_fs.x add disk.fs file16
./test_fs.x add disk.fs file17
./test_fs.x add disk.fs file18
./test_fs.x add disk.fs file19
./test_fs.x add disk.fs file20
./test_fs.x add disk.fs file21
./test_fs.x add disk.fs file22
./test_fs.x add disk.fs file23
./test_fs.x add disk.fs file24
./test_fs.x add disk.fs file25
./test_fs.x add disk.fs file26
./test_fs.x add disk.fs file27
./test_fs.x add disk.fs file28
./test_fs.x add disk.fs file29
./test_fs.x add disk.fs file30
./test_fs.x add disk.fs file31
./test_fs.x add disk.fs file32
./test_fs.x add disk.fs file33
./test_fs.x add disk.fs file34
./test_fs.x add disk.fs file35
./test_fs.x ls disk.fs >lib.stdout 2>lib.stderr

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
rm file1 file2 file3 file4 file5 file6 file7 file8 file9 file10 file11 file12 file13 file14 file15 file16 file17 file18 file19 file20 file21 file22 file23 file24 file25 file26 file27 file28 file29 file30 file31 file32 file33