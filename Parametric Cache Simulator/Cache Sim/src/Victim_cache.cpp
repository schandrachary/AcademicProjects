#include <iostream>
#include <cstdlib>
#include <cmath>

#include "Victim_cache.h"

using namespace std;

Victim_cache::Victim_cache(int cache_size, int block_size, int assoc, int num_bits, Memory* memory,int write_policy) {
  this->memory          = memory;
  this->cache_size      = cache_size;
  this->block_size      = block_size;
  this->write_policy   =  write_policy;


  this->assoc = assoc;
  this->set_size = this->block_size * assoc; // set size is equal to total cache size


  this->num_bytes               = this->block_size;
  this->num_sets                = this->cache_size/this->set_size; // number of sets will be 1 .

  this->num_bits                = num_bits;
  this->word_bits               = log2(this->num_bytes);
  this->set_bits                = log2(this->num_sets); //set_bits will be 0.
  this->tag_bits                = this->num_bits - (this->set_bits + this->word_bits);

  this->word_mul                = pow(2.0, this->word_bits) -1;
  this->set_mul                 = (this->word_mul + 1) * (pow(2.0, this->set_bits) - 1);//set_mul will be zero
  this->tag_mul                 = pow(2.0, this->word_bits + this->set_bits) * (pow(2.0, this->tag_bits) - 1);//there are no sets.

  // Initializing Statistics
  this->hits = 0;
  for(int i=0; i<2; i++)
    this->misses[i] = 0;

  // Initializing Victim_cache
  this->data = new Set*[this->num_sets];
  for( int i=0; i<num_sets; i++) { // whole cache is a single set.
      this->data[i] = new Set(this->assoc, this->block_size);
  }
}

int Victim_cache::get_hits(void) {
  return this->hits;
}

int Victim_cache::get_write_hits(void) {
  return this->write_hits;
}

int Victim_cache::get_misses(void) {
  return this->misses[0];
}

int Victim_cache::get_write_misses(void) {
  return this->misses[1];
}

Block* Victim_cache::victim_cache_read(unsigned int address) {
  int word_num  = this->word_mul & address;
  int set_num   = (this->set_mul & address)/(this->word_mul + 1);
  int tag_num   = (this->tag_mul & address)/pow(2.0, this->word_bits + this->set_bits);

  Set* cur_set = this->data[set_num];
  for(int i=0; i<assoc; i++) {
      Block* cur_block = cur_set->get_block(i);
      if(cur_block->get_tag() == tag_num) {
          cur_set->inc_counter(cur_block->get_index());
          this->hits++;
          return cur_block;
      }
  }
  this->misses[0]++;
  return (Block*)0;
}

Block* Victim_cache::victim_cache_write(int tag_num,Block* victim_entry) {
  this->write_hits++;
  Set* cur_set = this->data[0]; // Get the set where the address is mapped to
  Block* victim_evict=cur_set->insert_block(tag_num,victim_entry);
  return victim_evict;
}

Victim_cache::~Victim_cache(void) {
  for(int i=0; i<this->num_sets; i++)
    delete this->data[i];
  delete this->data;

}
