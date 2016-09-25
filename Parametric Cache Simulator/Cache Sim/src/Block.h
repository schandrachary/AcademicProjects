#ifndef BLOCK_H_
#define BLOCK_H_

class Block {
public:
  Block(int index, int counter, int block_size);

  Block* copy_block(); //Copies the block into new_block

  void set_block(int counter, int tag_value, Block* new_block);
  void set_tag(int tag);
  int set_word(int index, int value);

  int get_tag(void);
  int get_index(void);
  int get_word(int index);

  int get_counter(void);
  bool get_dirty(void);
  void set_counter(int counter);
  void set_dirty(void);


  ~Block();

private:
  char* bytes;	// Array of bytes (integers)

  int counter;	// Unique counter of the block
  int index;	// Index in the array.

  int tag;	// Tag bits of the block
  int num_bytes;	// Number of bytes present in the block
  bool  dirty;  //  dirty bits
};

#endif /* BLOCK_H_ */
