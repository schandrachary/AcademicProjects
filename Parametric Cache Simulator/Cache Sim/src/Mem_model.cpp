#include <iostream>
#include <cstdlib>
#include <cmath>

#include "Mem_model.h"

using namespace std;

Mem_model::Mem_model(int cache_size, int block_size, int assoc, long data_numbers,int write_policy,int victim_cache_size) {
  this->readaccesses	= 0;
  this->memory	= new Memory(MAINMEMORY_1GB, block_size);
  this->victim=(Victim_cache*) 0;
  if(victim_cache_size){
      this->victim = new Victim_cache(victim_cache_size, block_size, victim_cache_size/block_size,
          log2(data_numbers), (Memory*) 0,write_policy);
      this->cache	= new Cache(cache_size, block_size, assoc, log2(data_numbers), this->memory,write_policy,this->victim);
  }
  else
    this->cache = new Cache(cache_size, block_size, assoc, log2(data_numbers), this->memory,write_policy,(Victim_cache*) 0);
}

void Mem_model::print(char* str) {
  cout << endl;
  cout<<"-------------------------------------------------------------"<<endl;
  cout << "Cache Statistics for trace file:" << str << endl;
  cout<<"-------------------------------------------------------------"<<endl;
  cout << "\tL1 Read Accesses " << this->readaccesses << endl;
  cout << "\tRead Hits: " << this->cache->get_hits() << endl;
  cout << "\tRead Misses: " << this->cache->get_misses() << endl;
  cout << "\tL1 Write Accesses: " << this->writeacesses << endl;
  cout << "\tWrite Hits: " << this->cache->get_write_hits()<< endl;
  cout << "\tWrite Misses: " << this->cache->get_write_misses() << endl;
  float L1_miss_rate = 100.0 * ((float)(this->cache->get_write_misses() + this->cache->get_misses())/
      (float)(this->readaccesses+this->writeacesses));
  cout << "\tL1 Cache Miss Rate: " << L1_miss_rate << "%"<<endl;
  cout << endl;

  if(this->victim){
      cout << "\tNumber of victim cache reads: " << this->cache->victim_get_hits()+this->cache->victim_get_misses() << endl;
      cout << "\tNumber of victim cache read misses: " << this->cache->victim_get_misses() << endl;
      cout << "\tNumber of victim cache writes " << this->cache->victim_get_write_misses()+this->cache->victim_get_write_hits()<< endl;
      cout << "\tNumber of victim cache write misses " << this->cache->victim_get_write_misses()<< endl;
      float victim_miss_rate = 100.0 *
          ((float)(this->cache->victim_get_misses() + this->cache->victim_get_write_misses()) /
              (float)((this->cache->victim_get_hits()+this->cache->victim_get_misses())+
                  (this->cache->victim_get_write_misses()+this->cache->victim_get_write_hits())));
      cout << "\tVictim Cache Miss Rate: " << victim_miss_rate << "%"<<endl;
      cout << endl;
  }

  cout << "\tReads from main memory: " << this->memory->get_read_counter()*sizeof(int)<<" B"<< endl;
  cout << "\tWrites to main memory: " << this->memory->get_write_counter()* sizeof(int)<<" B"<< endl;
  cout << endl;
}

void Mem_model::restart_cache(void) {
  this->readaccesses = 0;
  this->cache->flush();
}

void Mem_model::write(unsigned int address, int value) {
  this->writeacesses++;
  int writeback=this->cache->cache_write(address,value);
}

int Mem_model::get(int addr) {
  this->readaccesses++;
  return this->cache->cache_read(addr);
}

Mem_model::~Mem_model(void) {
  delete this->memory;
  delete this->cache;
}
