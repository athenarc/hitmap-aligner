#include <iostream>
#include "../MyLibraries/Sequences/Algorithms.h"
#include "../MyLibraries/Sequences/SeqGen/SeqGen.h"
#include "../MyLibraries/Sequences/SHash/StrHash.h"
#include "../MyLibraries/Bitsets/SparseBitset.h"
using namespace std;

/**
 * It represents a dummy bucket of the dummy string hash table.
 *
 * @author Thanasis Vergoulis
 */
class DummyBucket
{
public:
	/**
	 * The constructor.
	 *
	 * @author Thanasis Vergoulis 
	 */
	DummyBucket(){};

	/**
	 * The destructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	~DummyBucket(){};
};

/**
 * It implements a dummy string hash table. 
 */
class DummyHash : public StrHash<DummyBucket>
{
public:		
	/**
	 * The constructor of the class. 
	 *
	 * @param key_len The length of the key strings.
	 * @param buckets_num The number of buckets in the hash table (used by StrHash).
	 * @param buck_groups_num The number of buckets in each bucket group. 
	 *
	 * @author Thanasis Vergoulis
	 */
	DummyHash( string* dseq, unsigned int key_len, unsigned int buckets_num, unsigned int buck_groups_num);
	
	/**
	 * The destructor of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	~DummyHash(void);	

	/**
	 * Probe the hash table using key. 
	 */
	unsigned int probe(string key)
	{
		return this->find(key);
	};
	
	/**
	 * Do nothing.
	 */
	void extraStatistics(){};
};

DummyHash::DummyHash( string* dseq, unsigned int key_len, unsigned int buckets_num, unsigned int buck_groups_num) : StrHash<DummyBucket>(key_len,buckets_num,buck_groups_num)
{
	unsigned int cur_bucket;
	DummyBucket** tmp;
	
	for( int i=0; i<buck_groups_num; i++)
		this->_buckets[i] = new DummyBucket*[this->_buckets_per_group];

	for(int i=0; i<dseq->length(); i=i+key_len)
	{
		string cur_tok;
		cur_tok = dseq->substr(i,key_len);
		cur_bucket = (this->_hash_func(cur_tok)%this->_buckets_num); 
		tmp = this->getBucketPtrRef(cur_bucket);  
		
		if( (*tmp)==0 )
			(*tmp) = new DummyBucket();

	}
}

DummyHash::~DummyHash()
{
}

int main()
{
	double cost_dp; //The cost of the calculation of one dynamic programming cell.
	double cost_probe; //The cost of probing a string hash table for a given string. 
	double cost_shift; //The cost of shifting to LSB a bitset divided by the number of bits.
	double cost_bit; //The cost of performing bitwise operation on a bitset divided by the number of bits.
	
	clock_t clock_start, clock_stop;
	
	cost_dp = 0;
	cost_probe = 0;
	cost_shift = 0;
	cost_bit = 0;
	
	cout<<"Calculating the constants of your system..."<<endl<<endl;
	
	//=== Preprocessing... ===
	cout<<"\t"<<"- Preprocessing..."<<endl;
	SeqGen generator; 
	string ab; //the used alphabet
	string* dseq; //the data sequence
	string* qseq; //the query sequence
	int dseq_len; //the length of the data sequence
	int qseq_len; //the length of the query sequence
	int al_thr; //the alignment threshold
	vector<int> results; //results of dynamic programming are going to be stored here.
	ab="acgt"; dseq_len = 100000000; qseq_len=100; al_thr=5; //initialise temp variables.
	dseq = new string(dseq_len,'$');
	generator.createSeq( dseq_len, ab, dseq);
	qseq = new string(qseq_len,'$');
	generator.createSeq( qseq_len, ab, qseq);	
	DummyHash* hash = new DummyHash( dseq, qseq_len, 1000000000, 1);
	int iterations; int i;
	int bitset_len;
	bitset_len = 1000;
	srand( time(NULL)); 
	SparseBitset* bitset1 = new SparseBitset(bitset_len,bitset_len);
	bitset1->randomSet(bitset_len/5);
	SparseBitset* bitset2 = new SparseBitset(bitset_len,bitset_len);
	bitset2->randomSet(bitset_len/5);
	cout<<endl;
	
	//=== 1. Calculate cost_dp ===
	cout<<"\t"<<"- Calculating cost of a dynamic programming cell..."<<endl;
	clock_start = clock();	
	dynamicProgramming(dseq, 0, dseq_len-1, qseq, al_thr, &results);
	clock_stop = clock(); 
	cost_dp = (double)(clock_stop-clock_start)/(double)(dseq_len*qseq_len)/(double)CLOCKS_PER_SEC;	
	cout<<"\t"<<"- cost_dp="<<cost_dp<<endl<<endl;
	
	//=== 2. Calculate cost_probe ===
	cout<<"\t"<<"- Calculating cost of probing a string hash..."<<endl;
	iterations = 100000000;
	clock_start = clock();
	for(i=0; i<iterations; i++)
	{
		hash->probe(*qseq);
	}
	clock_stop = clock(); 
	cost_probe = (double)(clock_stop-clock_start)/(double)iterations/(double)CLOCKS_PER_SEC;
	cout<<"\t"<<"- cost_probe="<<cost_probe<<endl<<endl;
	
	//=== 3. Calculate cost_bit ===
	cout<<"\t"<<"- Calculating cost per bit for bitwise operations on bitset..."<<endl;
	iterations = 10000000;
	clock_start = clock();
	for(i=0; i<iterations; i++)
	{
		(*bitset1) ^= (*bitset2);
		(*bitset1) &= (*bitset2);
		(*bitset1) |= (*bitset2);
	}
	clock_stop = clock(); 
	cost_bit = (double)(clock_stop-clock_start)/(double)iterations/(double)(3*bitset_len)/(double)CLOCKS_PER_SEC;
	cout<<"\t"<<"- cost_bit="<<cost_bit<<endl<<endl;	
	
	return 0;
}