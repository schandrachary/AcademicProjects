 #include <iostream>
#include <cstdlib>

#include "Block.h"

Block::Block(int index, int counter, int block_size) {
  this->index	= index;
  this->counter	= counter;

  this->num_bytes	= block_size;
  this->bytes	= new char[this->num_bytes];

  this->tag	= -1;
  this->dirty =false;
}

int Block::get_tag() {
  return this->tag;
}

int Block::get_index() {
  return this->index;
}

int Block::get_word(int index) {
  return bytes[index];
}

int Block::set_word(int index, int value) {
  bytes[index] = value;
  return bytes[index];
}

int Block::get_counter() {
  return this->counter;
}
bool Block::get_dirty(void) {
  return this->dirty;
}
void Block::set_counter(int counter) {
  this->counter = counter;
}

Block* Block::copy_block() {
  Block* new_block;
  new_block->counter= this->counter;
  new_block->tag=this->tag ;
  new_block->dirty=this->dirty;

  for(int i=0; i<this->num_bytes; i++) {
      new_block->bytes[i] = this->bytes[i];
  }
  return new_block;
}

void Block::set_block(int counter, int tag_value, Block* new_block) {
  this->counter 	= counter;
  this->tag 	= tag_value;

  for(int i=0; i<num_bytes; i++) {
      this->bytes[i] = new_block->get_word(i);
  }
}

void Block::set_tag(int tag) {
  this->tag = tag;
}
void Block::set_dirty(void) {
  this->dirty = true;
}

Block::~Block() {
  delete this->bytes;
}
