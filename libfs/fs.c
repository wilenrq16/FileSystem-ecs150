#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "disk.h"
#include "fs.h"

/* DEFINITIONS */
#define FAT_EOC 0xFFFF
#define BLOCK_SIZE 4096

/* SUPERBLOCK */
struct __attribute__((__packed__)) superblock {
    char signature[8];
    uint16_t total_blk_count;
    uint16_t rdir_blk;
    uint16_t data_blk;
    uint16_t data_blk_count;
    uint8_t fat_blk_count;
    char padding[4079];
};

/* ROOT DIRECTORY */
struct __attribute__((__packed__)) file {
    char filename[16];
    uint32_t file_size;
    uint16_t data_index;
    char padding[10];
};

/* FILE DESCRIPTOR */
struct file_descriptor {
    struct file* file;
    int offset;
};

/* DECLARING VARIABLES */
struct superblock* super_block;
uint16_t *fat_table;
struct file* root_dir;
uint16_t * fat_array; // Stores the FAT ARRAY

// Open Files
struct file_descriptor* fd_table[FS_OPEN_MAX_COUNT];
uint8_t num_open = 0;

/* HELPER FUNCTIONS */
int get_fat_free_blk() {
    int fat_free_blk = 0;
    for (int i = 0; i < super_block->data_blk_count; i++)
    {
        if (fat_table[i] == 0)
        {
            fat_free_blk++;
        }
    }
    return fat_free_blk;
}

int get_rdir_free_blk() {
    int rdir_free_blk = 0;
    for (int i = 0; i < FS_FILE_MAX_COUNT; i++)
    {
        if (root_dir[i].filename[0] == '\0')
        {
            rdir_free_blk++;
        }
    }
    return rdir_free_blk;
}

/* FS FUNCTIONS */
int fs_mount(const char *diskname)
{
    /* Check if disk is available */
    if (block_disk_open(diskname) == -1)
    {
        perror("opening disk");
        return -1;
    }

    /* Create superblock @super_block */
    super_block = malloc(sizeof(struct superblock));

    /* Check @super_block malloc */

    /* Read into super_block */
    block_read(0, super_block);

    /* Can probably add some checks here to check superblock */

    /* Create root directory @root_dir
     * Total size = FS_FILE_MAX_COUNT (or 128)
     */
    root_dir = (struct file*)malloc(sizeof(struct file) * FS_FILE_MAX_COUNT);

    /* Read into root_dir */
    block_read(super_block->rdir_blk, root_dir);

    /* Initialize @data_blk and @file_table (FAT) */
    uint16_t * data_blk = malloc(sizeof(uint16_t)*BLOCK_SIZE);
    fat_table = malloc(sizeof(uint16_t)*super_block->fat_blk_count*BLOCK_SIZE);

    /* @index - Add with @i in order to get correct index for FAT */
    int index = 0;
    for (int i = 1; i <= super_block->fat_blk_count; i++)
    {
        block_read(i, data_blk);
        memcpy(fat_table + index, data_blk, BLOCK_SIZE);
        index += 4096;
    }   

    /* Success */
    return 0;
}

int fs_umount(void)
{
    /* All meta-information and file data must hve been written out to disk */
    block_write(super_block->rdir_blk, root_dir);
    block_write(0, super_block);
    
    /* Delete */
    free(root_dir);
    free(fat_table);
    free(super_block); 

    /* Success */
    return 0;
}

int fs_info(void)
{
    int fat_free_blk = get_fat_free_blk();
    int rdir_free_blk = get_rdir_free_blk();
    printf("FS Info:\n");
    printf("total_blk_count=%d\n", super_block->total_blk_count);
    printf("fat_blk_count=%d\n", super_block->fat_blk_count);
    printf("rdir_blk=%d\n", super_block->rdir_blk);
    printf("data_blk=%d\n", super_block->data_blk);
    printf("data_blk_count=%d\n", super_block->data_blk_count);
    printf("fat_free_ratio=%d/%d\n", fat_free_blk, super_block->data_blk_count);
    printf("rdir_free_ratio=%d/%d\n", rdir_free_blk, FS_FILE_MAX_COUNT);
    return 0;
}

int fs_create(const char *filename)
{
    /* Check filename, could probably have some more checks here */
    if (strlen(filename) + 1 > FS_FILENAME_LEN)
    {
        perror("filename too long");
        return -1;
    }

    /* Start iterating through @root_dir to find empty entry & check existing
     * FS_OPEN_MAX_COUNT = 128 
     */
    int to_write = 0;
    int index = 0;
    for (int i = 0; i < FS_OPEN_MAX_COUNT; i++)
    {
        /* Empty entries first character == '\0' */
        if ((root_dir[i].filename[0] == '\0') && (to_write == 0))
        {
            index = i;
            to_write = 1;
        }
        else if (root_dir[i].filename[0] != '\0') 
        {
            if (strcmp(root_dir[i].filename, filename) == 0)
            {
                perror("file is currently running");
                return -1;
            }
        }
    }

    /* Check if @root_dir is full */
    if (to_write == 0)
    {
        perror("no more space in root_dir");
        return -1;
    }

    /* Success
     * Specify name of content and reset other information 
     */
    strcpy(root_dir[index].filename, filename);
    root_dir[index].file_size = 0;
    root_dir[index].data_index = FAT_EOC;
    return 0;
}

int fs_delete(const char *filename)
{
    /* Check if filename is valid */
    if (strlen(filename) + 1 > FS_FILENAME_LEN)
    {
        perror("filename too long");
        return -1;
    }

    /* Start delete process */
    int i;
    for (i = 0; i < FS_OPEN_MAX_COUNT; i++)
    {
        /* Check if root_dir[i] contains a file before strcmp */
        if (root_dir[i].filename[0] != '\0')
        {
            /* Let's check if this is the file we want */
            if (strcmp(root_dir[i].filename, filename) == 0)
            {
                /* "Delete" the file */
                root_dir[i].filename[0] = '\0';
                break;
            }
        }
    }

    /* Check if file is not in @root_dir */
    if (i == FS_OPEN_MAX_COUNT)
    {
        perror("file is not inside root_dir");
        return -1;
    }

    /* Sucess */
    return 0;
}

int fs_ls(void)
{
    printf("FS Ls:\n");
    for (int i = 0; i < FS_FILE_MAX_COUNT; i++ )
    {
        if(root_dir[i].filename[0] != '\0')
        {
            printf("filename: %s, file_size: %d, data_index: %d\n",root_dir[i].filename, root_dir[i].file_size, root_dir[i].data_index);
        }
    }
    return 0;
}

int fs_open(const char *filename)
{
    /* Check if filename is valid */
    if (strlen(filename) + 1 > FS_FILENAME_LEN)
    {
        perror("filename too long");
        return -1;
    }

    // Check if too many files are open
    if (num_open == FS_OPEN_MAX_COUNT-1)
    {
        return -1;
    }

    /* Find the file */
    int i;
    for (i = 0; i < FS_OPEN_MAX_COUNT; i++)
    {
        // Check next element is a valid file
        if (root_dir[i].filename[0] != '\0')
        {
            // Check if this is the file
            if (strcmp(root_dir[i].filename, filename) == 0)
            {
                // Open File
                struct file_descriptor* open_file = (struct file_descriptor*) malloc(sizeof(struct file_descriptor));
                open_file->file = &(root_dir[i]);
                open_file->offset = 0;
                num_open++;

                // Add to next open fd_descriptor
                int a;
                for(a = 0; a < FS_OPEN_MAX_COUNT; a++)
                {
                    if(fd_table[a] == NULL)
                    {
                        fd_table[a] = open_file;
                        return a; // a is the file descriptor
                    }
                }

            }
        }
    }

    // File Not Found
    return -1;
}

int fs_close(int fd)
{
    // Check if too many files are open
    if (num_open == FS_OPEN_MAX_COUNT-1)
    {
        return -1;
    }

    // Check if fd is out of bounds
    if(fd < 0 || fd >= FS_OPEN_MAX_COUNT) 
    {
        return -1;
    }
    
    // Attempt lookup to determine if file exists at fd
    if(fd_table[fd] == NULL)
    { // There was no file open at fd
        return -1; 
    } else {
        // Free the fd
        struct file_descriptor* close_fd = fd_table[fd];
        fd_table[fd] = NULL;

        free(close_fd);
        num_open--;
    }

    return 0;
}

int fs_stat(int fd)
{
    // Check if too many files are open
    if (num_open == FS_OPEN_MAX_COUNT-1)
    {
        return -1;
    }

    // Check if fd is out of bounds
    if(fd < 0 || fd >= FS_OPEN_MAX_COUNT) 
    {
        return -1;
    }
    
    // Attempt lookup to determine if file exists at fd
    if(fd_table[fd] == NULL)
    { // There was no file open at fd
        return -1; 
    } else {
        // Free the fd
        return fd_table[fd]->file->file_size;
    }

    return 0;
}

int fs_lseek(int fd, size_t offset)
{
    // TODO: OFFSET BOUND
    // Check if too many files are open
    if (num_open == FS_OPEN_MAX_COUNT-1)
    {
        return -1;
    }

    // Check if fd is out of bounds
    if(fd < 0 || fd >= FS_OPEN_MAX_COUNT) 
    {
        return -1;
    }
    
    // Attempt lookup to determine if file exists at fd
    if(fd_table[fd] == NULL)
    { // There was no file open at fd
        return -1; 
    } else { 
        fd_table[fd]->offset++;
    }
    return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
    return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
    // TODO: OFFSET BOUND
    // Check if too many files are open
    if (num_open == FS_OPEN_MAX_COUNT-1)
    {
        return -1;
    }

    // Check if fd is out of bounds
    if(fd < 0 || fd >= FS_OPEN_MAX_COUNT) 
    {
        return -1;
    }
    
    // Attempt lookup to determine if file exists at fd
    struct file_descriptor * file = fd_table[fd];
    if(file == NULL)
    { // There was no file open at fd
        return -1; 
    } 

    // BEGIN: File read
    void * bounce = malloc(BLOCK_SIZE);
    int buffer_offset = 0;
    while(count > 0) { // data requested to be written
        if(file->offset >= file->file->file_size)
        { // End of File
            break;
        }

        int block_sequence_index = (file->offset)/BLOCK_SIZE; // Block number in list
        uint16_t current_block = file->file->data_index; // Current Block actual block
        
        // Traverse through FAT to extract Block_Number
        for(int i = 0; i < block_sequence_index; i++)
        {
            current_block = fat_table[current_block]; // Extract next block
        }

        block_read(current_block+super_block->data_blk,bounce); // Read Block this is a bounce block
        
        if(count < BLOCK_SIZE) { 
            memcpy(buf+buffer_offset, bounce, count); // Copy partial block
            file->offset += count; // update offset
            break;
        } else {
            memcpy(buf+buffer_offset, bounce, BLOCK_SIZE); // Copy entire block
            buffer_offset += BLOCK_SIZE; // update buffer offset for next iteration
            file->offset += BLOCK_SIZE;  // update read offset
            count -= BLOCK_SIZE; // update count
        }
    }
 
    free(bounce);
    return 0;
}

