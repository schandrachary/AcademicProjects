#ifndef MEM_MODEL_H_
#define MEM_MODEL_H_

#include "Memory.h"
#include "Cache.h"
#include "Victim_cache.h"

#define MAINMEMORY_64KB 65536
#define MAINMEMORY_16MB 16777216
#define MAINMEMORY_64MB 67108864
#define MAINMEMORY_128MB 134217728
#define MAINMEMORY_512MB 536870912
#define MAINMEMORY_1GB 1073741824
#define MAINMEMORY_4GB 4294967296



class Mem_model {
	public:
		Mem_model(int cache_size, int block_size, int assoc, long data_numbers,int write_policy,int victim_cache_size);

		void write(unsigned int address,int value);
		int get(int addr);
		void print(char* str);

		void restart_cache(void);

		~Mem_model(void);

	private:
		Memory* memory;
		Cache* cache;
		Victim_cache* victim;

		int readaccesses;
		int writeacesses;
};

#endif /* MEM_MODEL_H_ */
