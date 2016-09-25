#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "Mem_model.h"

#define MAINMEMORY_64KB 65536
#define MAINMEMORY_16MB 16777216
#define MAINMEMORY_64MB 67108864
#define MAINMEMORY_128MB 134217728
#define MAINMEMORY_512MB 536870912
#define MAINMEMORY_1GB 1073741824
#define MAINMEMORY_2GB 2147483648
#define MAINMEMORY_4GB 4294967296

using namespace std;
class tracefile{
public:
  string attr;
  string address;
  int access_type;
};

unsigned int hexToInt( const char* hexStr )
{
  unsigned int hexInt;
  sscanf( hexStr, "%x", &hexInt );
  return hexInt;
}


int main(int argc, char** argv) {

  if(argc!=7) {
      cout << "Usage: " << argv[0] << " <cache_size_inB> <associativity> <block_size_inB> <write_policy> "
          "<victim_cache_size_inB> <tracefile>\n";
      return 1;
  }

  int cache_size = atoi(argv[1]);
  int associativity = atoi(argv[2]);
  int block_size = atoi(argv[3]);
  int write_policy = atoi(argv[4]);
  int victim_cache_size = atoi(argv[5]);

  cout<<"------------------------------------------------------------------------------------------------"<<endl;
  cout<<"Configuration:\t";
  cout<<"Cache Size:"<<cache_size<<"\tAssociativity:"<<associativity<<"\tBlock Size:"
      <<block_size;
  if(!write_policy)
    cout<<"\nWrite Policy:Write through with no-write allocate policies";
  else
    cout<<"\nWrite Policy:Write back with write allocate";
  cout<<"\tVictim Size:"<<victim_cache_size<<endl;
  cout<<"------------------------------------------------------------------------------------------------"<<endl;

  ifstream tracedata (argv[6],ios::in); // open the trace file
  if(!tracedata) { // file couldn't be opened
      cerr << "Error: Trace file could not be opened" << endl;
      return -1;
  }
  tracefile buffer;
  Mem_model* memory_module;
  memory_module =  new Mem_model (cache_size,block_size,associativity,MAINMEMORY_4GB,write_policy,victim_cache_size);

  if (tracedata.is_open()) {
      while (!tracedata.eof()) {
          unsigned int addr;
          tracedata >> buffer.attr>>buffer.address>>buffer.access_type;
          std::stringstream hex_addr(buffer.address) ;
          hex_addr>> addr;
          addr=hexToInt(buffer.address.c_str());
          if(buffer.attr=="s"){;
              memory_module->write(addr,69);
          }
          else if (buffer.attr=="l"){
              memory_module->get(addr);
          }
      }
  }
  tracedata.close();
  memory_module->print((char*)argv[6]);
  return 0;
}


