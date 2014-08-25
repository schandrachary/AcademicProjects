#ifndef VICTIM_CACHE_H_
#define VICTIM_CACHE_H_

#include "Set.h"
#include "Memory.h"

/* Class: Victim_cache
 *
 *
 * The number of bits for tag, set and word are calculated from
 * the block size, cache size and associativity.
 */


class Victim_cache {
public:
  Victim_cache(int cache_size, int block_size, int assoc, int bits_n, Memory* memory,int write_policy);

  int get_hits(void);
  int get_write_hits(void);
  int get_misses(void);
  int get_write_misses(void);
  Block* victim_cache_read(unsigned int address);
  Block* victim_cache_write(int tag_num,Block* victim_entry);

  ~Victim_cache(void);

private:
  Set** data;

  Memory* memory;

  int cache_size; // Size of cache in bytes
  int block_size;       // Size of block in bytes
  int set_size; // Size of set in bytes
  int assoc;            // Associativity

  int num_bytes;        // Number of bytes per block
  int num_bits; // Number of bits in Main memory address
  int num_sets; // Number of sets


  int set_bits;   // Number of bits for Set
  int tag_bits; // Number of bits for Tag
  int word_bits;        // Number of bits for Word


  int set_mul; // Number to be bitwise-multiplied with location to get set index
  int tag_mul;  // Number to be bitwise-multiplied with location to get tag index
  int word_mul; // Number to be bitwise-multiplied with location to get word index

  int write_policy;// writepolicy. 0 for write through and 1 for writeback

  int hits;       // Number of hits
  int write_hits; // Number of write hits
  int misses[2];        // Number of misses
};

#endif /* VICTIM_CACHE_H_ */
