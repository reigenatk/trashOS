#include "filesystem.h"

#define SIXTY_FOUR_BYTES 0x40
#define FOUR_KB 0x1000


/*
When successful, the first two calls fill in the dentry t
block passed as their second argument with the file name, file
type, and inode number for the file, then return 0
*/
uint32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
  int i;
  for (i = 0; i < num_directory_entries; i++) {
    if (strncmp((int8_t*) directory_entries[i].file_name, (int8_t*) fname, strlen(fname)) == 0) {
      // if the same filename
      strcpy((int8_t*) dentry->file_name, (int8_t*) directory_entries[i].file_name);
      dentry->file_type = directory_entries[i].file_type;
      dentry->inode_number = directory_entries[i].inode_number;
      memcpy(dentry->reserved, directory_entries[i].reserved, BYTES_RESERVED);
      return 0;
    }
  }
  return -1;
}

uint32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
  // index out of bounds?
  if (index < 0 || index >= num_inodes) {
    return -1;
  }

  strcpy((int8_t*) dentry->file_name, (int8_t*) directory_entries[index].file_name);
  dentry->file_type = directory_entries[index].file_type;
  dentry->inode_number = directory_entries[index].inode_number;
  memcpy(dentry->reserved, directory_entries[index].reserved, BYTES_RESERVED);
  return 0;
}

uint32_t get_data_block_num(int block_no, int inode) {
  uint32_t length_of_data = inodes[inode].length_in_bytes;

  // this is the max block idx we should ever use. All block idx should be less than this num
  uint32_t limit = num_data_blocks;
  uint32_t ret = inodes[inode].data_block_nums[block_no];
  if (ret < 0 || ret >= limit) {
    return -1;
  }
  return ret; 
}

/*
The last routine works much like the read system call, reading up to
length bytes starting from position offset 
in the file with inode number inode and returning the number of bytes
read and placed in the buffer. A return value of 0 thus indicates 
that the end of the file has been reached.

Also note that the read data call can only check that the given inode is within the
valid range. It does not check that the inode 
actually corresponds to a file (not all inodes are used). However, if a bad
data block number is found within the file bounds of the 
given inode, the function should also return -1.
*/
uint32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
  uint32_t oldlength = length;
  uint8_t* bufcopy = buf;
  if (inode < 0 || inode >= num_inodes) {
    return -1;
  }

  uint32_t length_of_data = inodes[inode].length_in_bytes;
  // printf("%d", length_of_data);

  // if we're asking to start past the length of data then obviously this is wrong
  if (offset < 0 || offset >= length_of_data) {
    return -1;
  }


  // this is the max block idx we should ever use. All block idx should be less than this num
  uint32_t limit = (length_of_data / FOUR_KB) + 1;

  uint32_t starting_block_idx = offset / FOUR_KB; // for instance, offset 4096 = block 1
  uint32_t offset_within_block = offset % FOUR_KB;
  uint32_t ptr_to_blocks = filesys_start_address + num_inodes * FOUR_KB;

  // let's handle the first block then all the later ones
  uint32_t remaining = FOUR_KB - offset_within_block;
  uint32_t translated_block = get_data_block_num(starting_block_idx, inode);
  if (translated_block == -1) {
    return -1;
  }
  uint32_t ptr = ptr_to_blocks + translated_block * FOUR_KB;
  if (length <= remaining) {
    memcpy(bufcopy, (void*) ptr+offset, length);
    return oldlength;
  }
  else {
    memcpy(bufcopy, (void*)  ptr+offset, remaining);
    length -= remaining;
    bufcopy += remaining;
    starting_block_idx++;
  }

  while (1) {
    if (starting_block_idx == limit) {
      // then we are EOF because we've used up all the blocks + still aren't done
      return 0;
    }

    if (length <= FOUR_KB) {
      // then we can finish rest of data in current block
      translated_block = get_data_block_num(starting_block_idx, inode);
      if (translated_block == -1) {
        return -1;
      }
      ptr = ptr_to_blocks + translated_block * FOUR_KB;
      memcpy(bufcopy, (void*) ptr, length);
      return oldlength;
    }
    else {
      // for when we cannot finish in current block
      // get FOURKB of data and go to next block
      translated_block = get_data_block_num(starting_block_idx, inode);
      if (translated_block == -1) {
        return -1;
      }
      ptr = ptr_to_blocks + translated_block * FOUR_KB;
      memcpy(bufcopy, (void*) ptr, FOUR_KB);
      ptr += FOUR_KB;
      length -= FOUR_KB;
    }
    starting_block_idx++;
  }

}

void init_filesystem(uint32_t filesystem_start_address) {
  // each one is 4 Bytes
  num_directory_entries = *((uint32_t*) filesystem_start_address);
  num_inodes = *((uint32_t*) (filesystem_start_address + 0x4));
  num_data_blocks = *((uint32_t*) (filesystem_start_address + 0x8));

  filesys_start_address = filesystem_start_address;

  //12+52 = 64B later we have x amount of 64B entries, where x = # of directory entries
  int i;
  uint32_t offset = SIXTY_FOUR_BYTES;
  for (i = 0; i < num_directory_entries; i++) {
    directory_entries[i] = *(dentry_t*)(filesystem_start_address + offset);
    offset += SIXTY_FOUR_BYTES;
  }

  offset = FOUR_KB;
  // read in all the inodes
  for (i = 0; i < num_inodes; i++) {
    inodes[i] = *(inode_block*)(filesystem_start_address + offset);
    offset += FOUR_KB;
  }

  // read in all data blocks 

  // get pointer to dblocks


}