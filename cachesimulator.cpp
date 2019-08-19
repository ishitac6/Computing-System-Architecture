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

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache {
      
      }
*/ 


class cache
{
  public: 
    bitset<32> trace_add;
    int s, b, t;
    bitset<32> offset_bits, index_bits, tag_bits;
    vector<vector<unsigned long>> Tag_array;
    vector<vector<unsigned long>> LRU;
    vector<vector<unsigned long>> Valid_bit;

    void set_values(int size, int blocks, int sets)
    { 
      if(sets = 0)
      {
        sets = (size*1024)/blocks ;       //  For Full associativity
      }
      s = log2((size*1024)/(blocks*sets)) ;         // set index bits or number of blocks
      b = log2(blocks) ;                            // Offset bits to access single byte in the n byte block
      t = 32 - s - b ;                              // Tag bits
      cout<< "index bits: "<< s << endl;
      cout<< "Offset bits: "<< b << endl;
      cout << "tag bits: " << t << endl;
      Tag_array = vector<vector<unsigned long>>(pow(2,s),vector<unsigned long>(sets)); 
      LRU = vector<vector<unsigned long>>(pow(2,s),vector<unsigned long>(sets));
      Valid_bit = vector<vector<unsigned long>>(pow(2,s),vector<unsigned long>(sets));
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

};

int array_cmp(vector<vector<unsigned long>> &array, bitset<32> tag, bitset<32> index, int size)
{
  int tag_match = 0;
  bitset<32> tagg; 
  tagg = tag;
  bitset<32> indexx;
  indexx = index;
  // cout << "Function indexx: " <<indexx.to_ulong() << endl;
  int sizee = size;
  // cout << "Function sizee: " <<sizee << endl;
  for(int j = 0; j < sizee; j++)
  {
    
    if(array[indexx.to_ulong()][j] == tagg.to_ulong())
       {
        // cout << " in: " <<indexx.to_ulong() << endl; 
        cout << "tag_ar: "<< array[indexx.to_ulong()][j] << "\t";
        tag_match = 1 ;
        // cout<< "Tag match in if: "<< tag_match << endl;
        break;
       }
    else
      tag_match = 0;
      
  }
  cout << endl;
  // cout<< "Tag match outside if: "<< tag_match << endl;
  return tag_match;
}
unsigned long shiftbits(bitset<32> inst, int start)  //18004633339 fed ex, 5008, 5th avenue, brooklyn new york, 11220. )
{
    unsigned long ulonginst;
    return ((inst.to_ulong())>>start);
    
}


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
   
  cache L1;
  cache L2;



  L1.set_values(cacheconfig.L1size,cacheconfig.L1blocksize,cacheconfig.L1setsize);
  L2.set_values(cacheconfig.L2size,cacheconfig.L2blocksize,cacheconfig.L2setsize);


  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
   
   
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
    
    if (traces.is_open()&&tracesout.is_open())
    {    
        while (getline (traces,line))
        {   // read mem access file and access Cache
            
            istringstream iss(line); 
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
            L1.extract_bits(accessaddr) ;
            L2.extract_bits(accessaddr) ;
            // L1.Tag_array[][]
            int tag_match = 0;
            int valid_set = 0;

           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0)
             {   // read access to the L1 Cache, 
                 //  and then L2 (if required), 
                 //  update the L1 and L2 access state variable;
              // Check for read hit or miss, for that first compare the tags              
              tag_match = array_cmp(L1.Tag_array, L1.tag_bits, L1.index_bits, cacheconfig.L1setsize);
              cout << "Return tag_match L1 " << tag_match << endl;
              if (tag_match == 1)
                {
                  cout<< "L1 tag match" << endl;
                  valid_set = array_cmp(L1.Valid_bit, 1 , L1.index_bits, cacheconfig.L1setsize);
                  if(valid_set)
                  {
                   L1AcceState = RH ;
                   L2AcceState = NA ; 
                   cout<< "L1 Read hit" << endl;
                  }
                }
              else if (!tag_match)
                {
                  cout<< "L1 read Miss" << endl;
                  L1AcceState = RM ;
                  tag_match = array_cmp(L2.Tag_array, L2.tag_bits, L2.index_bits, cacheconfig.L2setsize);
                  cout << "Return tag_match L2 " << tag_match << endl;
                   if (tag_match == 1)        // check both tag match and valid bit
                    {
                      cout<< "L2 tag match" << endl;
                      valid_set = array_cmp(L2.Valid_bit, 1 , L2.index_bits, cacheconfig.L2setsize);
                      if(valid_set)
                       {
                        L2AcceState = RH ; 
                        cout<< "L2 Read hit" << endl;
                       }
                       for(int j = 0; j < cacheconfig.L1setsize; j++)
                          {
                            if(L1.Valid_bit[L1.index_bits.to_ulong()][j] == 0)
                              {
                                L1.Tag_array[L1.index_bits.to_ulong()][j] = L1.tag_bits.to_ulong();
                                L1.Valid_bit[L1.index_bits.to_ulong()][j] = 1;
                                // LL1[L1.index_bits.to_ulong()].append_MRU(j)
                                cout << "Writes in L1 way: " << j <<endl;
                                cout << "Printing L1 tag array " << L1.Tag_array[L1.index_bits.to_ulong()][j] << endl ;
                                break;
                              }   
                          }
                    }
                   else
                      {
                        cout<< "L2 read Miss" << endl;
                        L2AcceState = RM ; // If l1 and L2 both readf miss then start allocating
                        for(int j = 0; j < cacheconfig.L2setsize; j++)
                          {
                            if(L2.Valid_bit[L2.index_bits.to_ulong()][j] == 0)
                              {
                                L2.Tag_array[L2.index_bits.to_ulong()][j] = L2.tag_bits.to_ulong();
                                L2.Valid_bit[L2.index_bits.to_ulong()][j] = 1;

                                cout << "Writes in L2 way: " << j <<endl;
                                cout << "Printing L2 tag array " << L2.Tag_array[L2.index_bits.to_ulong()][j] << endl ;
                                break; 
                              }  
                          }
                        for(int j = 0; j < cacheconfig.L1setsize; j++)
                          {
                            if(L1.Valid_bit[L1.index_bits.to_ulong()][j] == 0)
                              {
                                L1.Tag_array[L1.index_bits.to_ulong()][j] = L1.tag_bits.to_ulong();
                                L1.Valid_bit[L1.index_bits.to_ulong()][j] = 1;
                                cout << "Writes in L1 way: " << j <<endl;
                                cout << "Printing L1 tag array " << L1.Tag_array[L1.index_bits.to_ulong()][j] << endl ;
                                break;
                              }   
                          }                  
                      }
                }
              }
             else 
              { 
                tag_match = array_cmp(L1.Tag_array, L1.tag_bits, L1.index_bits, cacheconfig.L1setsize);
                if(tag_match == 1)
                {
                  L1AcceState = WH;
                  L2AcceState = WH;
                  cout<< "L1 L2 write hit" << endl;
                }
                else if (!tag_match)
                {
                  L1AcceState = WM;
                  cout<< "L1 write miss" << endl;
                  tag_match = array_cmp(L2.Tag_array, L2.tag_bits, L2.index_bits, cacheconfig.L2setsize);
                  if(tag_match == 1)
                  {
                    L2AcceState = WH;
                    cout<< " L2 write hit" << endl;
                  }
                  else
                  {
                    L2AcceState = WM;
                   cout<< " L2 write miss" << endl;
                  }
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
