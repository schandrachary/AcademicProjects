#ifndef CACHE_H_
#define CACHE_H_

#include "Set.h"
#include "Memory.h"
#include "Victim_cache.h"


/* Class: Cache
 * The number of bits for tag, set and word are calculated from
 * the block size, cache size and associativity.
 */


class Cache {
public:
  Cache(int cache_size, int block_size, int assoc, int bits_n, Memory* memory,int write_policy,Victim_cache* victim);

  int get_hits(void);
  int get_write_hits(void);
  int get_misses(void);
  int get_write_misses(void);
  int victim_get_hits(void);
  int victim_get_write_hits(void);
  int victim_get_misses(void);
  int victim_get_write_misses(void);
  int cache_read(unsigned int address);
  int cache_write(unsigned int address,int value);
  void flush(void);

  ~Cache(void);

private:
  Set** data;
  Set* data_cap;
  Memory* memory;
  Victim_cache* victim;

  int cache_size; // Size of cache in bytes
  int block_size;	// Size of block in bytes
  int set_size;	// Size of set in bytes
  int assoc;		// Associativity

  int num_bytes;	// Number of bytes per block
  int num_sets;	// Number of sets
  int num_bits;	// Number of bits in Main memory address

  int set_bits;	// Number of bits for Set
  int tag_bits;	// Number of bits for Tag
  int word_bits;	// Number of bits for Word

  int set_mul;	// Number to be bitwise-multiplied with location to get set index
  int tag_mul;	// Number to be bitwise-multiplied with location to get tag index
  int word_mul;	// Number to be bitwise-multiplied with location to get word index

  int write_policy;// writepolicy. 0 for write through and 1 for writeback

  int hits;       // Number of hits
  int write_hits; // Number of write hits
  int victim_hits;
  int victim_write_hits;
  int misses[4];	// Number of misses
};

#endif /* CACHE_H_ */
