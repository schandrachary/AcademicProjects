#include <iostream>
#include <cstdlib>
#include "Set.h"

Set::Set(int assoc, int block_size) {
  this->assoc		= assoc;
  this->block_size	= block_size;
  this->_counter 	= 0;

  // Initializing dirty and Blocks
  this->data = new Block*[assoc];
  for(int i=0; i<assoc; i++) {
      this->data[i] = new Block(i, this->counter(), block_size);
  }
}

Block* Set::get_block(int index) {
  return this->data[index];
}

Block* Set::insert_block(int tag_value, Block* new_block) {
  this->lru = 0;

  for(int i=0; i<assoc; i++) {
      Block* temp = this->data[i];
      if(temp->get_counter() < (this->data[lru])->get_counter())
        this->lru = i;
  }

  Block* temp_block = this->data[this->lru];
  temp_block->set_block(this->counter(), tag_value, new_block);
  return temp_block;
}

Block* Set::insert_block_onwrite(int tag_value, Block* new_block, int write_policy,int word_num,int value) {
  this->lru = 0;
  for(int i=0; i<assoc; i++) {
      Block* temp = this->data[i];
      if(temp->get_counter() < (this->data[lru])->get_counter())
        this->lru = i;
  }
  Block* temp_block = this->data[this->lru];
  Block* evicted_block=this->data[this->lru];
  if(write_policy)
      evicted_block->set_block(this->counter(), tag_value, temp_block);
  evicted_block->set_block(this->counter(), tag_value, new_block);
  temp_block->set_word(word_num,value);
  if(temp_block->get_dirty()==false)
    temp_block->set_dirty();

  return evicted_block;
}

int Set::counter() {
  this->_counter++;
  return this->_counter;
}

void Set::inc_counter(int index) {
  Block* temp_block = this->data[index];
  temp_block->set_counter(this->counter());
}

Set::~Set() {
  for(int i=0; i<assoc; i++)
    delete this->data[i];
  delete this->data;

}
