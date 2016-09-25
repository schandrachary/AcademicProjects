#ifndef SET_H_
#define SET_H_

#include "Block.h"

class Set {
public:
  Set(int assoc, int block_size);

  Block* get_block(int index);

  Block* insert_block(int tag_value, Block* new_block);
  Block* insert_block_onwrite(int tag_value, Block* new_block,int write_policy,int word_num,int value);
  int counter(void);
  void inc_counter(int index);
  ~Set();

  int lru;
private:
  Block** data;	// Array of Blocks

  int assoc;	// Associativity
  int block_size;	// Size of each block
  int _counter;	// Counter for giving unique index to each block
  // Index of block having the least value of counter
};

#endif /* SET_H_ */
