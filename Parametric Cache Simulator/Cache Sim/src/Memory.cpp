#include <iostream>
#include <cstdlib>

#include "Memory.h"

using namespace std;

Memory::Memory(int data_numbers, int block_size) {
  data_numbers=MAINMEMORY_16MB;
  this->num_bytes	= block_size;
  this->num_blocks = data_numbers/this->num_bytes + 1;
  this->writer	= 0;
  this->write_counter=0;
  this->read_counter=0;
  this->data	= new Block*[this->num_blocks];
  for(int i=0; i<num_blocks; i++)
    this->data[i] = new Block(i, i, block_size);
}

int Memory::get_write_counter(void) {
  return this->write_counter;
}
int Memory::get_read_counter(void) {
  return this->read_counter;
}
void Memory::write(int value) {
  writer=writer % MAINMEMORY_16MB;
  int block_num = writer/num_bytes;
  this->data[block_num]->set_word(writer % num_bytes, value);
  this->writer++;
  this->write_counter++;
  }

void Memory::write(unsigned int address,Block* cache_block) {
  address=address % MAINMEMORY_16MB;
  int block_num = address/num_bytes;
  for(int i=0; i<num_bytes; i++) {
      this->write_counter++;
      this->data[block_num]->set_word(i, cache_block->get_word(i));
  }
}
void Memory::write_block(unsigned int block_num,Block* cache_block) {
  block_num = block_num % (MAINMEMORY_16MB %num_blocks);
  for(int i=0; i<num_bytes; i++) {
      this->write_counter++;
      this->data[block_num]->set_word(i, cache_block->get_word(i));
  }
}

Block* Memory::read(unsigned int address) {
  address=address % MAINMEMORY_16MB;
  int block_num = address/num_bytes;
  this->read_counter=this->read_counter+num_bytes;
  return this->data[block_num];

}

Memory::~Memory(void) {
  for(int i=0; i<num_blocks; i++)
    delete this->data[i];
  delete this->data;
}
