#include <iostream>
#include <cstdlib>
#include <cmath>

#include "Cache.h"

using namespace std;

Cache::Cache(int cache_size, int block_size, int assoc, int num_bits, Memory* memory,int write_policy,Victim_cache* victim) {
  this->memory		= memory;
  this->cache_size	= cache_size;
  this->block_size	= block_size;
  this->write_policy   =  write_policy;
  this->victim         =  victim;

  if(assoc==0) {
      this->assoc = cache_size/block_size;
      this->set_size = cache_size;
  } else {
      this->assoc = assoc;
      this->set_size = this->block_size * assoc;
  }

  this->num_bytes		= this->block_size;
  this->num_sets 	 	= this->cache_size/this->set_size;

  this->num_bits 		= num_bits;
  this->word_bits		= log2(this->num_bytes);
  this->set_bits		= log2(this->num_sets);
  this->tag_bits		= this->num_bits - (this->set_bits + this->word_bits);


  this->word_mul 		= pow(2.0, this->word_bits) -1;
  this->set_mul 		= (this->word_mul + 1) * (pow(2.0, this->set_bits) - 1);
  this->tag_mul 		= pow(2.0, this->word_bits + this->set_bits) * (pow(2.0, this->tag_bits) - 1);

  // Initializing Statistics
  this->hits = 0;
  for(int i=0; i<4; i++)
    this->misses[i] = 0;

  // Initializing Cache
  this->data = new Set*[this->num_sets];
  for( int i=0; i<num_sets; i++) {
      this->data[i] = new Set(this->assoc, this->block_size);
  }

  // Initializing Cap Cache
  this->data_cap = new Set(this->cache_size/this->block_size, this->block_size);
}

void Cache::flush(void) {
  for(int i=0; i<num_sets; i++)
    delete this->data[i];
  delete this->data;
  delete this->data_cap;

  this->data = new Set*[this->num_sets];
  for(int i=0; i<num_sets; i++)
    this->data[i] = new Set(this->assoc, this->block_size);
  this->data_cap = new Set(this->cache_size/this->block_size, this->block_size);

  this->hits = 0;
  for(int i=0; i<4; i++)
    this->misses[i] = 0;
}

int Cache::get_hits(void) {
  return this->hits;
}

int Cache::get_write_hits(void) {
  return this->write_hits;
}

int Cache::get_misses(void) {
  return this->misses[0];
}

int Cache::get_write_misses(void) {
  return this->misses[1];
}
int Cache::victim_get_hits(void) {
  return this->victim_hits;
}

int Cache::victim_get_write_hits(void) {
  return this->victim_write_hits;
}

int Cache::victim_get_misses(void) {
  return this->misses[2];
}

int Cache::victim_get_write_misses(void) {
  return this->misses[3];
}
int Cache::cache_read(unsigned int address) {
  int word_num	= this->word_mul & address;
  int set_num	= (this->set_mul & address)/(this->word_mul + 1);
  int tag_num	= (this->tag_mul & address)/pow(2.0, this->word_bits + this->set_bits);

  Set* cur_set = this->data[set_num];
  for(int i=0; i<assoc; i++) {
      Block* cur_block = cur_set->get_block(i);

      if(cur_block->get_tag() == tag_num) {
          cur_set->inc_counter(cur_block->get_index());
          this->hits++;
          return cur_block->get_word(word_num);
      }
  }

  this->misses[0]++;
  Block* cur_mem_block= (Block*) 0;;
  if(this->victim)
    cur_mem_block=this->victim->victim_cache_read(address);

  if(cur_mem_block){
      this->victim_hits++;
      cur_mem_block->get_word(word_num);
      return 0;
  }else{
      this->misses[2]++;//read miss in the victim cache
      cur_mem_block = this->memory->read(address);//read from the main memory
      Block* evicted_block=cur_set->insert_block(tag_num,cur_mem_block);
      if(this->victim){
          Block* victim_evict =this->victim->victim_cache_write(evicted_block->get_tag(),evicted_block);
          if(victim_evict->get_dirty()){
              this->memory->write_block(address,victim_evict); // write to a random location , till i figure out how to
              //get the address of the evicted block from the victim cache
          }
      }
      cur_mem_block->get_word(word_num);
      return 0;
  }
}

int Cache::cache_write(unsigned int address,int value) {

  int word_num  = this->word_mul & address;
  int set_num   = (this->set_mul & address)/(this->word_mul + 1);
  int tag_num   = (this->tag_mul & address)/pow(2.0, this->word_bits + this->set_bits);

  Set* cur_set = this->data[set_num]; // Get the set where the address is mapped to
  for(int i=0; i<assoc; i++) {
      Block* cur_block = cur_set->get_block(i);
      if(cur_block->get_tag() == tag_num) { // Get the block with matching index
          cur_set->inc_counter(cur_block->get_index());
          this->write_hits++;
          cur_block->set_word(word_num,value);//Write the word
          if(!this->write_policy) {
              this->memory->write(address,cur_block);
          }
          else{
              cur_block->set_dirty();
          }
          return 0;
      }
  }
  this->misses[1]++; // increment L1 write miss

  Block* cur_mem_block= (Block*) 0;
  if(this->victim)
    cur_mem_block=this->victim->victim_cache_read(address);
  if(!cur_mem_block){
      this->misses[3]++;
      cur_mem_block = this->memory->read(address); // if block not present in victim cache , read from main memory
  }
  else{

      this->victim_write_hits++;
  }

  if(!this->write_policy) {

      cur_mem_block->set_word(address % this->num_bytes,value);
      return 0;
  }
  else{
      this->memory->write(value);
      cur_mem_block = this->memory->read(address);
      Block* evicted_block=cur_set->insert_block_onwrite(tag_num, cur_mem_block, word_num,this->write_policy,value);
      if(this->victim){
          Block* victim_evict =this->victim->victim_cache_write(evicted_block->get_tag(),evicted_block);
          if(victim_evict->get_dirty()){
              this->memory->write_block(address,victim_evict); // write to a random location , till i figure out how to
              //get the address of the evicted block from the victim cache
          }
      }
  }
  return 0;
}

Cache::~Cache(void) {
  for(int i=0; i<this->num_sets; i++)
    delete this->data[i];
  delete this->data;
  delete this->data_cap;
}
