/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

class Cache{
public:
	bool empty;
    bitset<32> trace_add;
    int ways;
    int s, b, t;
    bitset<32> offset_bits, index_bits, tag_bits;
    vector<vector<unsigned long> > Tag_array;
    vector<vector<unsigned long> > LRU;
    vector<vector<unsigned long> > Valid_bit;

    void set_values(int size, int blocks, int sets)
    {
      ways = sets;
      if(ways == 0)
      {
    	  ways = (size*1024)/blocks;
      }
      s = log2((size*1024)/(blocks*sets)) ;         // set index bits or number of blocks
      b = log2(blocks) ;                            // Offset bits to access single byte in the n byte block
      t = 32 - s - b ;                              // Tag bits
      cout<< "index bits: "<< s << endl;
      cout<< "Offset bits: "<< b << endl;
      cout << "tag bits: " << t << endl;
      Tag_array = vector<vector<unsigned long> >(pow(2,s),vector<unsigned long>(sets));
      LRU = vector<vector<unsigned long> >(pow(2,s),vector<unsigned long>(sets));
      Valid_bit = vector<vector<unsigned long> >(pow(2,s),vector<unsigned long>(sets));
      cout << "tag: "<< Tag_array.size()<<" " <<Tag_array[0].size() <<endl;
    }
    void extract_bits(bitset<32> trace_add)
    {
      cout<< "Trace Add: "<< trace_add << endl;
      bitset<32> offtemp;
      offtemp = pow(2,b) - 1;      //mask for offset
      //cout << offtemp << endl;
      offset_bits = (trace_add & offtemp);
      //cout<<"Offset bits: "<< offset_bits<< endl;
      trace_add = trace_add >> b;
      // cout<< "Trace Add: "<< trace_add << endl;
      bitset<32> intemp;
      intemp = pow(2, s) - 1;
      //cout << intemp << endl;
      index_bits = (trace_add & intemp);
      cout<<"Index bits: "<< index_bits.to_ulong()<< endl;
      trace_add = trace_add >> s;
      //cout<<"trace: "<< trace_add<< endl;
      tag_bits = trace_add;
      cout<<"tag bits: "<< tag_bits.to_ulong()<< endl;
    }

    int hit_check(){
    	for(unsigned long i=0; i < ways; i++){
    		if(Tag_array[index_bits.to_ulong()][i] == tag_bits.to_ulong()){
    			return i;
    		}
    	}
    	return -1;
    }

    void allocate(){
    	empty = false;
    	for(unsigned long i=0; i < ways; i++){
    		if(Valid_bit[index_bits.to_ulong()][i] == 0){
    			Tag_array[index_bits.to_ulong()][i] = tag_bits.to_ulong();
    			empty = true;
    			break;
    		}
    	}
    	if(!empty){
    		for(unsigned long i=0; i < ways; i++){
    			if(LRU[index_bits.to_ulong()][i] == ways){
    				Tag_array[index_bits.to_ulong()][i] = tag_bits.to_ulong();
					Valid_bit[index_bits.to_ulong()][i] = 1;
    			}
    		}
    	}
    }

    void lru_update(int index, bool hit){
    	if(hit == 1){
			for(unsigned long i=0 ; i< ways; i++){
				if(LRU[index_bits.to_ulong()][i] < LRU[index_bits.to_ulong()][index] && LRU[index_bits.to_ulong()][i] != 0){
					LRU[index_bits.to_ulong()][i]++;
				}
			}
			LRU[index_bits.to_ulong()][index] = 1;
		} else if(hit == 0){
			if(!empty){
				for(unsigned long i=0 ; i< ways; i++){
					if(LRU[index_bits.to_ulong()][i] == ways){
						Tag_array[index_bits.to_ulong()][i] = tag_bits.to_ulong();
						Valid_bit[index_bits.to_ulong()][i] = 1;
						LRU[index_bits.to_ulong()][i] = 1;
					} else {
						LRU[index_bits.to_ulong()][i]++;
					}
				}
			} else {
				for(unsigned long i=0 ; i< ways; i++){
					if(Valid_bit[index_bits.to_ulong()][i] == 0){
						Valid_bit[index_bits.to_ulong()][i] = 1;
						LRU[index_bits.to_ulong()][i] = 1;
						break;
					} else {
						LRU[index_bits.to_ulong()][i]++;
					}
				}
			}
		}
    }
};

int main(int argc, char* argv[]){



    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L2blocksize;
      cache_params>>cacheconfig.L2setsize;
      cache_params>>cacheconfig.L2size;
      }



   // Implement by you:
   // initialize the hirearch cache system with those configs
   // probably you may define a Cache class for L1 and L2, or any data structure you like

  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;

  Cache L1;
  L1.set_values(cacheconfig.L1size, cacheconfig.L1blocksize, cacheconfig.L1setsize);
  Cache L2;
  L2.set_values(cacheconfig.L2size, cacheconfig.L2blocksize, cacheconfig.L2setsize);


    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";

    traces.open(argv[2]);
    tracesout.open(outname.c_str());

    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;

    if (traces.is_open()&&tracesout.is_open()){
        while (getline (traces,line)){   // read mem access file and access Cache

            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);

            L1.extract_bits(accessaddr);
            L2.extract_bits(accessaddr);
            int L1index = L1.hit_check();
            int L2index = L2.hit_check();

            bool L1hit = (L1index >= 0)?true:false;
            bool L2hit = (L2index >= 0)?true:false;
           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0)

             {
            	  if(L1hit == 1){
            		  L1AcceState = 1;
            		  L2AcceState = 0;
            		  L1.lru_update(L1index, L1hit);
            	  } else if (L1hit == 0 && L2hit == 1){
            		  L1AcceState = 2;
            		  L2AcceState = 1;
            		  L1.allocate();
            		  L1.lru_update(L1index, L1hit);
            		  L2.lru_update(L2index, L2hit);
            	  } else {
            		  L1AcceState = 2;
            		  L2AcceState = 2;
            		  L1.allocate();
            		  L2.allocate();
            		  L1.lru_update(L1index, L1hit);
            		  L2.lru_update(L2index, L2hit);
            	  }

                 }
             else
             {
                  // write access to the L1 Cache,
                  //and then L2 (if required),
                  //update the L1 and L2 access state variable;
           	  if(L1hit == 1 && L2hit == 1){
           		  L1AcceState = 3;
           		  L2AcceState = 3;
        		  L1.lru_update(L1index, L1hit);
        		  L2.lru_update(L2index, L2hit);
           	  } else if (L1hit == 0 && L2hit == 1){
           		  L1AcceState = 4;
           		  L2AcceState = 3;
        		  L2.lru_update(L2index, L2hit);
           	  } else {
           		  L1AcceState = 4;
           		  L2AcceState = 4;
           	  }
                  }

            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";

    return 0;
}
