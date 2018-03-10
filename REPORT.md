# Project 4: File System

## Phase 0: The ECS150-FS Specification  

### Superblock  

To implement the superblock, we used a struct with the similar format to the  
instructions. We made sure it is exactly the same as described in order for  
`block_read()` to work properly.  
  
The struct `superblock` contains: the `signature`, total number of data blocks  
'total_blk_count', the starting index of the root directory 'rdir_blk', the  
starting index of the data blocks 'data_blk', the amount of data blocks  
'data_blk_count', the amount of blocks for fat 'fat_blk_count', and a char array  
which is unused 'padding'.

Our variable for the superblock that we are using is `super_block`
  
### FAT  
  
To implement the FAT, we used two different variables to represent the fat table  
'fat_table' and also the fat array 'fat_array'. They are both uint16_t arrays.  
  
'fat_table' is the fat blocks, while 'fat_array' is an array of `fat_table`  
    
### Root Directory 
  
To implement the root directory, we created a struct similar to the instructions  
`file`. The struct contains the 16 char array file name `filename`, the file size  
`file_size`, the index of the first data block 'data_index', and a 10 char array  
that will be unused 'padding'.  
  
We created a variable `root_dir` which is a `file` array.  
  
## Phase 1: Mounting/Unmounting

### Mount

To implement the mounting function, we first had to check if the disk is  
available, if it is, then we can continue the mounting process. The first  
thing that we did was initialize `super_block` with malloc, then we read  
in the correct data using `block_read(0, super_block)`. After we then  
initalized the root directory, and read in the data with the root directory  
index from the superblock. In order to initalize the FAT, we malloced `data_blk`  
with a size of `uint16_t * BLOCK_SIZE`, and we multiplied that same number  
by the number of FAT blocks which is given from `super_block`. To copy the data  
we used a for loop with the number of fat blocks, and read it using the index  
from the for loop and `data_blk`. After we used `memcpy()` in order to copy the  
contents of the FAT blocks. We also had to implement `index` which is a  
multiple of `BLOCK_SIZE` which we used for `fat_table`.  
  
### Unmount  
  
The first thing that was done in `fs_umount()` was writing out the  
meta-information and file data out to the disk by using `block_write()`. After  
we wrote out to the disk, we simply freed our three structures `root_dr`,   
`fat_table`, and `super_block`   
  
### Info  
  
To implement our info function, we based the print statements off the tester  
`fs_ref.x`. In order to get `fat_free_ratio` and `rdir_free_ratio`, we had to  
implement two different helper functions.   
  
The first function is `get_fat_free_blk()`, which simply searches through  
`fat_table` and increments the counter `fat_free_blk`, whenever the entry is  
empty.  
  
The second function `get_rdir_free_blk()` does the same as `get_fat_free_blk`,  
but instead it iterates through `root_dir` and checks if it's empty by  
comparing the first character of the filename and '\0'  
  
### Testing Phase 1  
  
Testing phase 1 was a fairly simple process. We used `fs_make.x` to generate  
disks of different names and sizes, then tested it using our `test_fs.x` and   
the tester `fs_ref.x` to compare.  
   
To take account of disknames that cannot be opened and invalid file systems in  
`fs_mount()`, we implemented a check using `block_disk_open()` and checking if  
it returns -1. If it returns -1, it means that it had a problem opening the  
disk, therefore our program returns -1 in this case as well. We also used a  
check in `fs_umount()` where we checked if `block_disk_close()` returns -1.  
This only took account of cases in which there were no underlying virtual disks  
or if the virtual disk cannot be closed.  
  
## Phase 2: File Creation/Deletion   
  
### Create  
  
The first thing we did in `fs_create()` was to check for a valid filename. We  
two checks: one to check if the filename + '\0' would be greater than  
`FS_FILENAME_LEN` or 16, the other is to check if the filename is non-NULL  
terminated. If it's a valid filename, then it starts iterating through  
`root_dir` to find an empty entry and to check if the file is already running.  
For this part, we added two variables `to_write` and `index`. Since `root_dir`  
is an array, we iterated through the whole array to grab the index of the empty  
entry and to check for duplicates. If it finds an empty entry, then we set  
`index` to equal the iterator `i`, which would be where we copy `filename` into  
the `file` block. the variable `to_write` is initailized as 0, but then changes  
to 1 only if there is an empty entry. If it equals 0 after iterating, then that  
means there are no empty entries in the root directory. Otherwise, if it equals  
to 1, then we are able to create the file.  
  
### Delete  
In `fs_delete()`, we used the same check for the filename that was in `fs_create().`  
After the check, we were able to iterate through the root directory and check if  
the file exists. The first character of empty entries in the root directory is '\0',  
so we first checked using that, and if it is not empty, then we proceeded with  
`strcmp()` to compare the file name of the respective entry and 'filename', the  
file we want to delete. If it is a match, then we delete it by setting the first  
character of the file to '\0'. We then traverse through the Fat table starting  
with the data index and we set all values to 0 until we hit the FAT_EOC file  
0xFFFF. This essentially removes the blocks but doesn't actually clear the data  
from the data blocks.  
  
### Ls  
Since we are using an array, we iterate through `root_dir` `FS_FILE_MAX_COUNT`  
or 128 times. We need to iterate through the whole array, and not just until we  
find an empty entry, since there can be deleted files in between created files.  
While iterating, we check if the file is not empty by using the same check as  
stated previously, by comparing the first character of the file name to '\0'.  
Then we print out a statement showing the file's name, size, and index.  
  
### Testing Phase 2  
   
In phase 2, we created files using `echo` and `touch` with different filenames  
and tested it with `fs_ref.x` to check if it was valid or not. We then compared  
with our program and took account of valid file names by addressing and  
checking them in the beginning of our functions.   
  
## Phase 3: File Descriptor Operations  
  
In this stage we needed to create a new struct for the file descriptors in  
order to handle reading and writing. We made `file_descriptor` a struct  
consisting of two variables: `struct file* file` and `int offset`. The variable  
`file` contains a file and the offset is the current reading/writing position  
in the file.   
    
We then declared the variable `struct file_descriptor *fd_table` as an array of  
file descriptors.   
  
We also have the uint8_t counter `num_open` that keeps track of the number of 
opened files.  
  
### Open
The function `fs_open()` opens a file and returns a file descriptor which can  
then be used for subsequent operations. The first thing we did was use the same   
filename checks that we used in phase 2 in order to check valid file names.   
After we checked if too many files were open. Once it passes the beginning  
checks, it then iterates through `root_dir`. It then finds the file that it  
wants to open, in which we set as the `file_descriptor` variable `open_file`.  
We then fill in the contents of `open_file` with the file that we obtain from  
`root_dir` and the default opened offset 0, and also increment `num_open`.  
We then iterate through `fd_table` to grab an empty entry to store  
`open_file`, and then return the file descriptor.  
  
### Closing 
The function `fs_close()` closes the file descriptor. Here we checked if there  
are too many files opened or if the file descriptor is valid and not out of  
bounds. If it's valid, then we check `fd_table` at the index and check if it  
is NULL. If it is not then we set the `fd_table` entry to NULL and free the  
file descriptor by setting the respective file descriptor to another variable  
`close_fd`. We also decremented `num_open`.  
  
### Stat 
The function `fs_stat()` gives the file size of the file descriptor. In order to 
do that we checked if `fd_table` at the index is valid and return the file size.   
  
### LSeek  
The function `fs_lseek()` explicitly sets the offset, and is implemented by  
finding the file descriptor at `fd` in `fd_table` and sets the offset of the  
file descriptor to the value given.  
  
### Testing Phase 3  
  
For this phase, in order to test `fs_open()`, we added a new command to  
`test_fs.c` called `open_err`, where we opened the same file 40 times  
knowing that the limit is 32. This allows us to perform error checks  
and prevents us from adding another file descriptor when `fd_table` is  
full.  
    
## Phase 4: File Reading/Writing  
  
This phase is the most involved of the phases and required a deeper  
understanding of how the file system works. The problem with this phase was  
properly updating and extracting data from blocks since not all operations did  
not involve and entire block of data. We needed to properly keep track of where  
the offsets were within blocks and when new blocks needed to be allocated.  
  
### Read    
We first began by implementing `fs_read()` as recommended. After doing  
preliminary checks to determine that the file was open we then began reading the  
file. This process involved reading data block by block depending on how much  
data was needed. We first used the `data_index` from the appropriate file within  
a file descriptor this is the starting address of the file the 0th block for  
this file. We then looked at the current offset and determined which block in  
the sequence  we were offset to. Using a loop we traversed the `fat_table` array  
to the block index in the sequence (not the block number) we then extracted this  
block number.  
  
With the correct block number the next step was to read from the block. We read  
from the block using the `block_read()` function into a bounce block, `bounce`.  
From here we have several options depending on how much data was to be placed  
into the buffer. If the modulo of the offset was not zero it means we needed to  
read from an offset within the block. We then were only able to read a maximum  
of the block size - offset with a minimum read being count. This whole process  
is done for the first read. We used `memcpy` to appropriately extract the data  
from the bounce with the correct offset and lengths into `buf`. We then  
increment the appropriate offsets and decrement count. If there is still data  
that needs to be read (for example if count > 4096 bytes) we repeat this entire  
process again. Once reading is complete it returns the number of bytes read. If  
the read requests more bytes than in the file we simply return the number of  
bytes until the EOF is reached.  
  
### Write  
With read complete write is almost exactly the same except the opposite wih a  
couple of modifications. First, we must consider the case where we ae simply  
overwriting data. if the offset is before the end then this is the case which  
means we may also need to copy some of the data out of the block we are  
modifying if the offset is not at the beginning of the block. In order to do  
this we checked for the case where data was being rewritten and extracted the  
block into bounce. From here we modified the data according to buffer and  
rewrote the block back. We repeated this process until all data required was  
written or until we ran out of blocks.  
  
### Testing Phase 4
We wrote another command in test_fs.c called `inc_read' and `inc_write` they do  
the exact same operation as cat except we do the operation broken in 2 smaller  
of the operations. For example if we wanted to read 100bytes we first read 50   
bytes then read another 50 bytes.   
  
## Comprehensive Testing  
  
Here we created a lot of different test script shells in order to test the  
program with every component as a whole.  
  
In our test script `test_script_1.sh`, we simply added files (1-10), deleted file  
5 and then added file 11. This tests if the file would be placed in the right  
order and if it would take the next open entry (where file 5 was previously).  
   
Test scripts `test_script_2.sh`, `test_script_3.sh`, `test_script_4.sh`, and  
`test_script_5.sh` tests the cat command to make sure it is printing out  
the content correctly.  
  
Test script `test_script6.sh` tests the size of the filename, and shows that  
filenames that exceed 16 characters are forbidden  
  
Test script `test_script7.sh` tests removing a file that doesn't exit, which  
results in an error  
  
Test script `test_script8.sh` tests the creation of a disk that is smaller  
than the block size.  
  
Test script `test_script_128.sh` tests what occurs when we fill up the whole  
root directory, which causes no errors. On the other hand `test_script_129.sh`  
and `test_script129_empty.sh` tests the cases where we add more than 129 files  
to the root directory.  
