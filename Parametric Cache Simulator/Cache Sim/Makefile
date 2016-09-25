CC= g++
CFLAGS= -Wall -pedantic -g
RM= rm -rf

all: cache_sim

cache_sim: src/Cache.o  src/Victim_cache.o src/Memory.o src/Set.o src/Block.o src/Mem_model.o src/main.cpp
	$(CC) $(CFLAGS) $^ -o $@
	$(RM) src/*.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) src/*.o
	$(RM) cache_sim
