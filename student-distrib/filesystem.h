#ifndef FILESYS_H
#define FILESYS_H
#include "types.h"
#include "lib.h"

#define MAX_OPEN_FILES 8 
#define MAX_FILE_NAME_LENGTH 32
#define BYTES_RESERVED 24
#define MAX_DATA_BLOCKS_PER_INODE 1023 // (4kB / 4B) - 1

/*GENERAL INFO: 
* filesystem memory is divided into 4kB blocks

*/

// All of these structs are defined in Appendix A
// directory entry:
typedef struct dentry_t {
  uint8_t file_name[MAX_FILE_NAME_LENGTH];
  uint32_t file_type;
  uint32_t inode_number;
  uint8_t reserved[BYTES_RESERVED];
} dentry_t;

typedef struct file_descriptor {
  uint32_t jump_table;
  uint32_t inode;
  uint32_t file_position;
  uint32_t flags;
} file_descriptor;

typedef struct inode_block {
  uint32_t length_in_bytes;
  uint32_t data_block_nums[MAX_DATA_BLOCKS_PER_INODE];
} inode_block;

#define BYTES_IN_A_DATA_BLOCK 4096

typedef struct data_block {
  uint8_t data[BYTES_IN_A_DATA_BLOCK];
} data_block;

// we can kinda cheat and see that we have 0x11 directory entries 0x40 inodes, and 0x3B data blocks
// ie 17, 64 and 59
uint32_t num_directory_entries;
uint32_t num_inodes;
uint32_t num_data_blocks;

uint32_t filesys_start_address;

// 63 because (4KB-64) / 64 (first 64 bits reserved for statistics of block)
dentry_t directory_entries[63];

inode_block inodes[64]; // again kinda hardcoding this because I saw there were 64 inodes from multiboot

// defined in Appendix A
uint32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
uint32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
uint32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

void init_filesystem(uint32_t filesystem_start_address);

#endif