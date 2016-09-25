#ifndef MEMORY_H_
#define MEMORY_H_

#include "Block.h"

#define MAINMEMORY_16MB 16777216
class Memory {
public:
  Memory(int data_numbers, int block_size);

  int get_read_counter(void) ;
  int get_write_counter(void) ;

  Block* read(unsigned int address);
  void write(int value);
  void write(unsigned int address,Block* cache_block);
  void write_block(unsigned int address,Block* cache_block);

  ~Memory(void);

private:
  Block** data;

  int num_blocks;
  int num_bytes;
  int writer;

  int write_counter;
  int read_counter;
};

#endif /* MEMORY_H_ */
