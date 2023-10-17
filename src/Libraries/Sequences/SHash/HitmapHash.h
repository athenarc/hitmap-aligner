/******************************************************************************************************************************//*
 * @file HitmapHash.h
 *
 * Declaration of HitmapHash class. This class implements the Hitmap index.  
 *
 * @author Thanasis Vergoulis
 ********************************************************************************************************************************/
#ifndef HITMAPHASH_H
#define HITMAPHASH_H

//#define USE_POOL_AND_VECTORS
//#define USE_POOL_ONLY
#define COLUMN_STORED_BUCKETS

#include <cmath>
#include <boost/dynamic_bitset.hpp>
#include <boost/integer/common_factor_rt.hpp>
#include <fstream>
#include "../Algorithms.h"
#include "../../Bitsets/SparseBitset.h"
#include "../../Math/MyMaths.h"
#include "../../Misc/Utilities.h" //to use itoa
#include "StrHash.h"
using namespace boost;

namespace hmp //Hitmap namespace
{
	/**
	 * It contains information related to the memory consumption of the Hitmap index.
	 *
	 * @var _total_bytes The total number of bytes required by the index.
	 * @var _hitmap_bytes The number of bytes required by the buckets of the hash table.
	 * @var _pool_bytes The number of bytes required by the pool.
	 * @var _total_bytes_without_opts The number of bytes required if we would not use the pool.
	 *
	 * @author Thanasis Vergoulis
	 */
	struct StorageSize
	{
		double _total_bytes;
		double _hitmap_bytes;
		double _pool_bytes;
		double _total_bytes_without_opts;
		
		/**
		 * The costructor of the struct.
		 *
		 * @author Thanasis Vergoulis
		 */
		StorageSize()
		{
			this->_total_bytes = 0;
			this->_hitmap_bytes = 0;
			this->_pool_bytes = 0;
			this->_total_bytes_without_opts = 0;
		}
	};

//========================================================================================
//

#ifdef USE_POOL_ONLY
	/**
	 * It represents a hitmap, i.e., a bucket of the Hitmap index. 
	 *
	 * @var _hitmap The hitmap (i.e., an array of bitsets).
	 * @var _bs_num The number of bitsets in each hitmap bucket (static).
	 * @var _bs_size The number of bits in each bitmap (static).
	 * @var _block_size The size of each block in the SparseBitsets (static).
	 * @var _zeros A bitset of size _bs_size full of zeros (static) used for many purposes.
	 * @var _zeros_exist It is true if there is space allocated for _zeros (static).
	 *
	 * @author Thanasis Vergoulis
	 */
	class HitmapBucket
	{
		SparseBitsetC** _hitmap;
		static unsigned long _bs_num;
		static unsigned long _bs_size;
		static unsigned long _block_size;
		static SparseBitsetC* _zeros;
		static SparseBitset* _zeros2;
	public:
		/**
		 * The constructor.
		 *
		 * @param bs_num The number of bitsets in the hitmap.
		 * @param bs_size The number of bits in each bitset.
		 * @param block_size The size of each block in the SparseBitsets.
		 *
		 * @author Thanasis Vergoulis 
		 */
		HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size);

		/**
		 * The destructor.
		 *
		 * @author Thanasis Vergoulis
		 */
		~HitmapBucket();
			
		/**
		 * It sets the b_id-th bit of the bs_id-th hitmap.
		 *
		 * @param bs_id The id of the bitset which contains the bit to be set.
		 * @param b_id The id of the bit to be set.
		 *
		 * @author Thanasis Vergoulis
		 */
		void setHit(unsigned long bs_id, unsigned long b_id);

		/**
		 * It returns the bs_id-th sparse bitset of the bucket.
		 *
		 * @param bs_id The id of the bitset in the bucket.
		 *
		 * @author Thanasis Vergoulis
		 */
		SparseBitsetC getBitset(unsigned long bs_id);
		
		/**
		 * It sets the result bitset to contain a shifted (to the LSB) copy of the determined hitmap's bitset.
		 *
		 * @param result Reference to the result bitset.
		 * @param bs_id The id of the hitmap's bitset to be shifted.
		 * @param shift_by The number of positions to be shifted (to the LSB).
		 *
		 * @author Thanasis Vergoulis
		 */
		void lsbShiftBitset(SparseBitset* result, unsigned long bs_id, unsigned long shift_by);
				
		/**
		 * It prints on the standard output the bs_id-th bitset.
		 *
		 * @param bs_id The id of the bitset to be printed.
		 *
		 * @author Thanasis Vergoulis
		 */
		void printBitset(unsigned long bs_id);
		
		/**
		 * It prints on the standard output the current instance of the class.
		 *
		 * @author Thanasis Vergoulis
		 */
		void print(void);
		
		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @attention This function must be executed only before replacing zero bitsets with zero
		 * pointers. Otherwise, use countEmptyBitsets().
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		unsigned long countZeros(void);

		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		unsigned long countEmptyBitsets(void);

		double avgSetBits(void);

		double countEmptyBlocks();
		double countBlocks();
		
		/**
		 * It returns the number of set bits in all bitsets.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		int countSetBits(void);
		
		/**
		 * It returns the number of set bits in a particular bitset.
		 *
		 * @param bs_id The id of the desired bitset.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		int countSetBits(unsigned long bs_id);
		
		/**
		 * It removes all the bitsets that are full of zeros and replaces them with a null pointer.
		 *
		 * @author Thanasis Vergoulis
		 */
		void removeZeros(void);
		
		/** 
		 * It deletes the space allocated to _zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		static void freeZeros(void)
		{ 
//			delete HitmapBucket::_zeros_old;
			delete HitmapBucket::_zeros;
		}
		
/*		static SparseBitsetU* getZeros(void)
		{ 
//			return HitmapBucket::_zeros_old;
			return HitmapBucket::_zeros;
		}*/



		/**
		 * It returns the size of the hitmap bucket in bytes with or without considering 
		 * the size of unit vector blocks (stored in UVB pool). The actual size is if 
		 * uvb_blocks is false.
		 * 
		 * @param uvb_blocks If it is true the size of the UVB blocks is considered, otherwise no.
		 *
		 * @return The size of the hitmap bucket in bytes.
		 *
		 * @author Thanasis Vergoulis
		 */		
		double byteSize(bool uvb_blocks)
		{
			double size;
			
			//Consider the size of the pointers.
			size = HitmapBucket::_bs_num*sizeof(SparseBitsetC*);
			
			//Consider the size of the bitsets.
			for( int i=0; i<HitmapBucket::_bs_num; i++)
			{ 
				if( this->_hitmap[i]!=0 ) //No bitset if the pointer is 0
				{ 
					size += this->_hitmap[i]->byteSize(uvb_blocks); 
				}
			}
			return size;
		}
	};

	/**
	 * It represents a set of counter bitsets.
	 *
	 * @var _cnt_bitsets Array containing references to the counter bitsets.
	 * @var _rem_bitsets Array containing references to the remainder bitsets. 
	 * @var _cnt_bitsets_num The number of counter bitsets (and the number of remainder bitsets as well).
	 * @var _cnt_num The number of counters encoded in the counter bitsets.
	 *
	 * @author Thanasis Vergoulis
	 */
	class CounterBitsets
	{
		SparseBitset** _cnt_bitsets_new;
		SparseBitset** _rem_bitsets_new;
		unsigned long _cnt_bitsets_num;
		unsigned long _cnt_num;
	public:
		/**
		 * The constructor.
		 *
		 * @param cnt_num The number of counters encoded in the counter bitsets. 
		 * @param t_max The maximum number the counter can count. 
		 * @param block_size The size of the blocks of each sparse bitset. 
		 *
		 * @author Thanasis Vergoulis
		 */
		CounterBitsets(unsigned long cnt_num, unsigned long t_max, unsigned long block_size);

		~CounterBitsets();
		
		/**
		 * It calculates the t-set flags of a given set of bitsets and returns the result in the result bitset.
		 *
		 * @param bitsets Array containing the given set of bitsets.
		 * @param bitsets_num The number of bitsets.
		 * @param t The appearance threshold.
		 * @param result Reference to the result bitset.
		 *
		 * @author Thanasis Vergoulis
		 */
		void tSetFlagsCalc(dynamic_bitset<>** bitsets, unsigned long bitsets_num, unsigned long t, dynamic_bitset<>* result);		

		void tSetFlagsCalc(SparseBitset** bitsets, unsigned long bitsets_num, unsigned long t, SparseBitset* result);

		/**
		 * It resets the counter bitsets.
		 *
		 * @author Thanasis Vergoulis
		 */
		void reset(void);
		
		/**
		 * It prints on standard output the contents of the counter & remainder bitsets.
		 *
		 * @author Thanasis Vergoulis
		 */
		void print(void);
	};

	/**
	 * It represents a Hitmap index. It is an StrHash having as bucket a HitmapBucket.  
	 *
	 * @var _dseq_ptr A pointer to the data sequence. 
	 * @var _dseq_len The length of the data sequence. 
	 * @var _part_len The length of each part of each partition.
	 * @var _part_shift The number of symbols for which the i-th part of the j-th partition is shifted in comparison to the i-th part of the (j-1)-th partition. 
	 * @var _partition_num The number of partitions in the partition set.
	 * @var _partition_size The number of parts in each partition. 
	 * @var _fnum The number of query fragments.
	 * @var _flen The length of query fragments.
	 * @var _qthr The cost threshold.
	 * @var _counters A pointer to a buffer of counter bitsets (used to computed t-set flags).
	 * @var _wins Hash table containing the windows that need to be verified.
	 * @var _results Vector containing the approx. occurrences of the query in the data sequence. 
	 * @var _stats Verification statistics.
	 *
	 * @author Thanasis Vergoulis
	 */
	class HitmapHash : public StrHash<HitmapBucket>
	{
		string* _dseq_ptr;
		unsigned long _dseq_len;
		unsigned long _part_len;
		unsigned long _part_shift;
		unsigned long _partition_num;
		unsigned long _partition_size; 
		unsigned long _fnum;
		unsigned long _flen;
		unsigned long _qthr;
		CounterBitsets* _counters;
		vector<int> _wins;
		vector<int> _results;
		VerificationStatistics _stats;
		SparseBitset* _tsetflags_new;
		unsigned long _block_size;
		unsigned long _blocks_num;
	public:		
		/**
		 * The constructor of the class. 
		 *
		 * @param dseq_ptr A pointer to the data sequence.
		 * @param qthr The cost threshold.
		 * @param part_shift The number of symbols for which the i-th part of a partset is shifted in conmparison to its (i-1)-th part.
		 * @param flen The length of the query fragments (used also by StrHash as the length of key-strings.
		 * @param fnum The number of fragments. 
		 * @param block_size The size of the blocks of each sparse bitset.
		 * @param buckets_num The number of buckets in the hash table (used by StrHash).
		 *
		 * @author Thanasis Vergoulis
		 */
		HitmapHash( string* dseq_ptr, unsigned long qthr, unsigned long part_shift, unsigned long flen, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, unsigned long buck_groups_num);
		
		~HitmapHash(void);
		
/*		void deleteGroupBucket(unsigned long group_id,unsigned long bucket_id)
		{
			delete [] this->_buckets[group_id][bucket_id];
		}*/
	
		void initBuckets(void);
	
		/**
		 * It returns the size of the Hitmap-hash table in bits.
		 *
		 * @return The size of the hash table in bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		StorageSize byteSize();
	
		/**
		 * It prints on the standard output statistics which are special for the Hitmap index. 
		 *
		 * @author Thanasis Vergoulis
		 */
		void extraStatistics();
		
		/**
		 * It prints in the standard output the current instance of the class. 
		 *
		 * @author Thanasis Vergoulis
		 */
		void print();

		/**
		 * It initializes the counter bitsets.
		 *
		 * @param t_max The maximum number the counter can count.
		 *
		 * @author Thanasis Vergoulis
		 */
		void initCounterBitsets(unsigned long t_max);
		
		/**
		 * It searches for a given string.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void findWindows(string& qseq);
		
		/**
		 * It sorts all duplicate windows and removes all duplicates.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueWindows(void);
		
		/**
		 * @depricated
		 *
		 * @author Thanasis Vergoulis
		 */
		void verifyWindows(string& qseq, unsigned long epsilon, unsigned long fnum);
		
		void verifyWindows(string& qseq);
		
		/**
		 * It returns the data sequence windows that need to be verified.
		 *
		 * @return A vector containing the start points of the windows.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getWindows(void);
		
		/**
		 * It calculates the verification statistics.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void setVerificationStatistics(string& qseq);
		
		/**
		 * It returns the verification statistics.
		 *
		 * @return The verification statistics.
		 *
		 * @author Thanasis Vergoulis
		 */
		VerificationStatistics getVerificationStatistics(void);
		
		
		/**
		 * It returns the approx. occurrences of the query in the data.
		 *
		 * @return A vector containing the end poins of the approx. occurrences.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getResults(void);

		/**
		 * It guarantees that only unique results are in the results.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueResults(void);
		
		void freeZeros(void);
	};
#endif

//========================================================================================
//

#ifdef USE_POOL_AND_VECTORS
	/**
	 * It represents a hitmap, i.e., a bucket of the Hitmap index. 
	 *
	 * @var _hitmap The hitmap (i.e., an array of bitsets).
	 * @var _bs_num The number of bitsets in each hitmap bucket (static).
	 * @var _bs_size The number of bits in each bitmap (static).
	 * @var _block_size The size of each block in the SparseBitsets (static).
	 * @var _zeros A bitset of size _bs_size full of zeros (static) used for many purposes.
	 * @var _zeros_exist It is true if there is space allocated for _zeros (static).
	 *
	 * @author Thanasis Vergoulis
	 */
	class HitmapBucket
	{
		SparseBitsetUltraC** _hitmap;
		static unsigned long _bs_num;
		static unsigned long _bs_size;
		static unsigned long _block_size;
		static SparseBitsetUltraC* _zeros;
		static SparseBitset* _zeros2;
	public:
		/**
		 * The constructor.
		 *
		 * @param bs_num The number of bitsets in the hitmap.
		 * @param bs_size The number of bits in each bitset.
		 * @param block_size The size of each block in the SparseBitsets.
		 *
		 * @author Thanasis Vergoulis 
		 */
		HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size);

		/**
		 * The destructor.
		 *
		 * @author Thanasis Vergoulis
		 */
		~HitmapBucket();
			
		/**
		 * It sets the b_id-th bit of the bs_id-th hitmap.
		 *
		 * @param bs_id The id of the bitset which contains the bit to be set.
		 * @param b_id The id of the bit to be set.
		 *
		 * @author Thanasis Vergoulis
		 */
		void setHit(unsigned long bs_id, unsigned long b_id);

		/**
		 * It returns the bs_id-th sparse bitset of the bucket.
		 *
		 * @param bs_id The id of the bitset in the bucket.
		 *
		 * @author Thanasis Vergoulis
		 */
		SparseBitsetUltraC getBitset(unsigned long bs_id);
		
		/**
		 * It sets the result bitset to contain a shifted (to the LSB) copy of the determined hitmap's bitset.
		 *
		 * @param result Reference to the result bitset.
		 * @param bs_id The id of the hitmap's bitset to be shifted.
		 * @param shift_by The number of positions to be shifted (to the LSB).
		 *
		 * @author Thanasis Vergoulis
		 */
		void lsbShiftBitset(SparseBitset* result, unsigned long bs_id, unsigned long shift_by);
				
		/**
		 * It prints on the standard output the bs_id-th bitset.
		 *
		 * @param bs_id The id of the bitset to be printed.
		 *
		 * @author Thanasis Vergoulis
		 */
		void printBitset(unsigned long bs_id);
		
		/**
		 * It prints on the standard output the current instance of the class.
		 *
		 * @author Thanasis Vergoulis
		 */
		void print(void);
		
		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @attention This function must be executed only before replacing zero bitsets with zero
		 * pointers. Otherwise, use countEmptyBitsets().
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		unsigned long countZeros(void);

		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		unsigned long countEmptyBitsets(void);

		double avgSetBits(void);

		double countEmptyBlocks();
		double countBlocks();
		
		/**
		 * It returns the number of set bits in all bitsets.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		int countSetBits(void);
		
		/**
		 * It returns the number of set bits in a particular bitset.
		 *
		 * @param bs_id The id of the desired bitset.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		int countSetBits(unsigned long bs_id);
		
		/**
		 * It removes all the bitsets that are full of zeros and replaces them with a null pointer.
		 *
		 * @author Thanasis Vergoulis
		 */
		void removeZeros(void);
		
		/** 
		 * It deletes the space allocated to _zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		static void freeZeros(void)
		{ 
//			delete HitmapBucket::_zeros_old;
			delete HitmapBucket::_zeros;
		}
		
/*		static SparseBitsetU* getZeros(void)
		{ 
//			return HitmapBucket::_zeros_old;
			return HitmapBucket::_zeros;
		}*/



		/**
		 * It returns the size of the hitmap bucket in bytes with or without considering 
		 * the size of unit vector blocks (stored in UVB pool). The actual size is if 
		 * uvb_blocks is false.
		 * 
		 * @param uvb_blocks If it is true the size of the UVB blocks is considered, otherwise no.
		 *
		 * @return The size of the hitmap bucket in bytes.
		 *
		 * @author Thanasis Vergoulis
		 */		
		double byteSize(bool uvb_blocks)
		{
			double size;
			
			//Consider the size of the pointers.
			size = HitmapBucket::_bs_num*sizeof(SparseBitsetUltraC*);
			
			//Consider the size of the bitsets.
			for( int i=0; i<HitmapBucket::_bs_num; i++)
			{ 
				if( this->_hitmap[i]!=0 ) //No bitset if the pointer is 0
				{ 
					size += this->_hitmap[i]->byteSize(uvb_blocks); 
				}
			}
			return size;
		}
	};

	/**
	 * It represents a set of counter bitsets.
	 *
	 * @var _cnt_bitsets Array containing references to the counter bitsets.
	 * @var _rem_bitsets Array containing references to the remainder bitsets. 
	 * @var _cnt_bitsets_num The number of counter bitsets (and the number of remainder bitsets as well).
	 * @var _cnt_num The number of counters encoded in the counter bitsets.
	 *
	 * @author Thanasis Vergoulis
	 */
	class CounterBitsets
	{
		SparseBitset** _cnt_bitsets_new;
		SparseBitset** _rem_bitsets_new;
		unsigned long _cnt_bitsets_num;
		unsigned long _cnt_num;
	public:
		/**
		 * The constructor.
		 *
		 * @param cnt_num The number of counters encoded in the counter bitsets. 
		 * @param t_max The maximum number the counter can count. 
		 * @param block_size The size of the blocks of each sparse bitset. 
		 *
		 * @author Thanasis Vergoulis
		 */
		CounterBitsets(unsigned long cnt_num, unsigned long t_max, unsigned long block_size);

		~CounterBitsets();
		
		/**
		 * It calculates the t-set flags of a given set of bitsets and returns the result in the result bitset.
		 *
		 * @param bitsets Array containing the given set of bitsets.
		 * @param bitsets_num The number of bitsets.
		 * @param t The appearance threshold.
		 * @param result Reference to the result bitset.
		 *
		 * @author Thanasis Vergoulis
		 */
		void tSetFlagsCalc(dynamic_bitset<>** bitsets, unsigned long bitsets_num, unsigned long t, dynamic_bitset<>* result);		

		void tSetFlagsCalc(SparseBitset** bitsets, unsigned long bitsets_num, unsigned long t, SparseBitset* result);

		/**
		 * It resets the counter bitsets.
		 *
		 * @author Thanasis Vergoulis
		 */
		void reset(void);
		
		/**
		 * It prints on standard output the contents of the counter & remainder bitsets.
		 *
		 * @author Thanasis Vergoulis
		 */
		void print(void);
	};

	/**
	 * It represents a Hitmap index. It is an StrHash having as bucket a HitmapBucket.  
	 *
	 * @var _dseq_ptr A pointer to the data sequence. 
	 * @var _dseq_len The length of the data sequence. 
	 * @var _part_len The length of each part of each partition.
	 * @var _part_shift The number of symbols for which the i-th part of the j-th partition is shifted in comparison to the i-th part of the (j-1)-th partition. 
	 * @var _partition_num The number of partitions in the partition set.
	 * @var _partition_size The number of parts in each partition. 
	 * @var _fnum The number of query fragments.
	 * @var _flen The length of query fragments.
	 * @var _qthr The cost threshold.
	 * @var _counters A pointer to a buffer of counter bitsets (used to computed t-set flags).
	 * @var _wins Hash table containing the windows that need to be verified.
	 * @var _results Vector containing the approx. occurrences of the query in the data sequence. 
	 * @var _stats Verification statistics.
	 *
	 * @author Thanasis Vergoulis
	 */
	class HitmapHash : public StrHash<HitmapBucket>
	{
		string* _dseq_ptr;
		unsigned long _dseq_len;
		unsigned long _part_len;
		unsigned long _part_shift;
		unsigned long _partition_num;
		unsigned long _partition_size; 
		unsigned long _fnum;
		unsigned long _flen;
		unsigned long _qthr;
		CounterBitsets* _counters;
		vector<int> _wins;
		vector<int> _results;
		VerificationStatistics _stats;
		SparseBitset* _tsetflags_new;
		unsigned long _block_size;
		unsigned long _blocks_num;
	public:		
		/**
		 * The constructor of the class. 
		 *
		 * @param dseq_ptr A pointer to the data sequence.
		 * @param qthr The cost threshold.
		 * @param part_shift The number of symbols for which the i-th part of a partset is shifted in conmparison to its (i-1)-th part.
		 * @param flen The length of the query fragments (used also by StrHash as the length of key-strings.
		 * @param fnum The number of fragments. 
		 * @param block_size The size of the blocks of each sparse bitset.
		 * @param buckets_num The number of buckets in the hash table (used by StrHash).
		 *
		 * @author Thanasis Vergoulis
		 */
		HitmapHash( string* dseq_ptr, unsigned long qthr, unsigned long part_shift, unsigned long flen, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, unsigned long buck_groups_num);
		
		~HitmapHash(void);
		
/*		void deleteGroupBucket(unsigned long group_id,unsigned long bucket_id)
		{
			delete [] this->_buckets[group_id][bucket_id];
		}*/
	
		void initBuckets(void);
	
		/**
		 * It returns the size of the Hitmap-hash table in bits.
		 *
		 * @return The size of the hash table in bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		StorageSize byteSize();
	
		/**
		 * It prints on the standard output statistics which are special for the Hitmap index. 
		 *
		 * @author Thanasis Vergoulis
		 */
		void extraStatistics();
		
		/**
		 * It prints in the standard output the current instance of the class. 
		 *
		 * @author Thanasis Vergoulis
		 */
		void print();

		/**
		 * It initializes the counter bitsets.
		 *
		 * @param t_max The maximum number the counter can count.
		 *
		 * @author Thanasis Vergoulis
		 */
		void initCounterBitsets(unsigned long t_max);
		
		/**
		 * It searches for a given string.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void findWindows(string& qseq);
		
		/**
		 * It sorts all duplicate windows and removes all duplicates.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueWindows(void);
		
		/**
		 * @depricated
		 *
		 * @author Thanasis Vergoulis
		 */
		void verifyWindows(string& qseq, unsigned long epsilon, unsigned long fnum);
		
		void verifyWindows(string& qseq);
		
		/**
		 * It returns the data sequence windows that need to be verified.
		 *
		 * @return A vector containing the start points of the windows.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getWindows(void);
		
		/**
		 * It calculates the verification statistics.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void setVerificationStatistics(string& qseq);
		
		/**
		 * It returns the verification statistics.
		 *
		 * @return The verification statistics.
		 *
		 * @author Thanasis Vergoulis
		 */
		VerificationStatistics getVerificationStatistics(void);
		
		
		/**
		 * It returns the approx. occurrences of the query in the data.
		 *
		 * @return A vector containing the end poins of the approx. occurrences.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getResults(void);

		/**
		 * It guarantees that only unique results are in the results.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueResults(void);
		
		void freeZeros(void);
	};
#endif

//========================================================================================
//

#ifdef NO_MEM_OPTS
	class HitmapBucket
	{
		SparseBitset** _hitmap;
		static unsigned long _bs_num;
		static unsigned long _bs_size;
		static unsigned long _block_size;
		static SparseBitset* _zeros;
	public:
		/**
		 * The constructor.
		 *
		 * @param bs_num The number of bitsets in the hitmap.
		 * @param bs_size The number of bits in each bitset.
		 * @param block_size The size of each block in the SparseBitsets.
		 *
		 * @author Thanasis Vergoulis 
		 */
		HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size);

		~HitmapBucket();
				
		/**
		 * It sets the b_id-th bit of the bs_id-th hitmap.
		 *
		 * @param bs_id The id of the bitset which contains the bit to be set.
		 * @param b_id The id of the bit to be set.
		 *
		 * @author Thanasis Vergoulis
		 */
		void setHit(unsigned long bs_id, unsigned long b_id);

		SparseBitset getBitset(unsigned long bs_id);
		
		/**
		 * It sets the result bitset to contain a shifted (to the LSB) copy of the determined hitmap's bitset.
		 *
		 * @param result Reference to the result bitset.
		 * @param bs_id The id of the hitmap's bitset to be shifted.
		 * @param shift_by The number of positions to be shifted (to the LSB).
		 *
		 * @author Thanasis Vergoulis
		 */
		void lsbShiftBitset(SparseBitset* result, unsigned long bs_id, unsigned long shift_by);
		
		/**
		 * It sets the result bitset to contain a shifted (to the MSB) copy of the determined hitmap's bitset.
		 *
		 * @param result Reference to the result bitset.
		 * @param bs_id The id of the hitmap's bitset to be shifted.
		 * @param shift_by The number of positions to be shifted (to the MSB).
		 *
		 * @author Thanasis Vergoulis
		 */
		void msbShiftBitset(SparseBitsetC* result, unsigned long bs_id, unsigned long shift_by);
				
		/**
		 * It prints on the standard output the bs_id-th bitset.
		 *
		 * @param bs_id The id of the bitset to be printed.
		 *
		 * @author Thanasis Vergoulis
		 */
		void printBitset(unsigned long bs_id);
		
		/**
		 * It prints on the standard output the current instance of the class.
		 *
		 * @author Thanasis Vergoulis
		 */
		void print(void);
		
		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @attention This function must be executed only before replacing zero bitsets with zero
		 * pointers. Otherwise, use countEmptyBitsets().
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		unsigned long countZeros(void);

		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		unsigned long countEmptyBitsets(void);

		double avgSetBits(void);

		double countEmptyBlocks();
		double countBlocks();
		double countUnaryBlocks();
		
		/**
		 * It returns the number of set bits in all bitsets.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		int countSetBits(void);
		
		/**
		 * It returns the number of set bits in a particular bitset.
		 *
		 * @param bs_id The id of the desired bitset.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
		int countSetBits(unsigned long bs_id);
		
		/**
		 * It removes all the bitsets that are full of zeros and replaces them with a null pointer.
		 *
		 * @author Thanasis Vergoulis
		 */
		void removeZeros(void);
		
		/** 
		 * It deletes the space allocated to _zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
		static void freeZeros(void)
		{ 
			delete HitmapBucket::_zeros;
		}
		
		static SparseBitset* getZeros(void)
		{ 
			return HitmapBucket::_zeros;
		}
	};
#endif 

//========================================================================================
//

#ifdef COLUMN_STORED_BUCKETS

	/**
	 * It represents a bucket column.
	 *
	 * @var _col_cnt The content of the column.
	 * @var _col_id The id of the column.
	 *
	 * @author Thanasis Vergoulis
	 */
	struct BucketCol
	{
		dynamic_bitset<> _col_cnt;
		int _col_id;
	};

	/**
	 * It represents a hash bucket of the Hitmap hash table.
	 *
	 * @var _bucket_columns A vector containing all the non-empty columns of the bucket's bitsets.
	 * @var _bs_num The number of bitsets in the bucket (static).
	 * @var _bs_size The size of the bitsets (static).
	 * @var _block_size The size of the blocks for the sparse bitsets (static).
	 *
	 * @author Thanasis Vergoulis
	 */
	class HitmapBucket
	{
		vector<BucketCol> _bucket_columns;		
		static unsigned long _bs_num;
		static unsigned long _bs_size;
		static SparseBitset* _zero_bitset;
		static unsigned long _block_size;
	public:
		/**
		 * The constructor.
		 *
		 * @param bs_num The number of bitsets in the hitmap.
		 * @param bs_size The number of bits in each bitset.
		 *
		 * @author Thanasis Vergoulis 
		 */
		HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size);

		/**
		 * The destructor.
		 *
		 * @author Thanasis Vergoulis
		 */
		~HitmapBucket();
			
		/**
		 * It sets the b_id-th bit of the bitsets_to_set_num consecutive bitsets having as
		 * first the first_bitset.  
		 *
		 * @param bit_id The id of the bit to be set.
		 *
		 * @author Thanasis Vergoulis
		 */
//		void setHit(unsigned long bit_id, unsigned long first_bitset, unsigned long bitsets_to_set_num);

		void setHit(unsigned long pos, unsigned long part_len, unsigned long flen);
		void setHit_quick(unsigned long pos, unsigned long part_len, unsigned long flen);


		/**
		 * It sets the result bitset to contain a shifted (to the LSB) copy of the determined hitmap's bitset.
		 *
		 * @param result Reference to the result bitset.
		 * @param bs_id The id of the hitmap's bitset to be shifted.
		 * @param shift_by The number of positions to be shifted (to the LSB).
		 *
		 * @author Thanasis Vergoulis
		 */
		void lsbShiftBitset(SparseBitset* result, unsigned long bs_id, unsigned long shift_by);

		/**
		 * It releases the space kept for zero bitset.
		 *
		 * @author Thanasis Vergoulis
		 */
		static void freeZeroBitset(void)
		{
			delete HitmapBucket::_zero_bitset;
			HitmapBucket::_zero_bitset = NULL;
		}
		
		/**
		 * It returns a string that represents the bucket. 
		 *
		 * DO NOT SERIALIZE EMPTY BUCKET!!
		 */
		string serialize(void);
		
		string serializeCompact(void);

		/**
		 * DO NOT ADD COLUMN TO EMPTY BUCKET!!!
		 */
		void addColumn(string column);

		/**
		 * It returns the bs_id-th sparse bitset of the bucket.
		 *
		 * @param bs_id The id of the bitset in the bucket.
		 *
		 * @author Thanasis Vergoulis
		 */
//		SparseBitsetC getBitset(unsigned long bs_id);
					
		/**
		 * It prints on the standard output the bs_id-th bitset.
		 *
		 * @param bs_id The id of the bitset to be printed.
		 *
		 * @author Thanasis Vergoulis
		 */
//		void printBitset(unsigned long bs_id);
		
		/**
		 * It prints on the standard output the current instance of the class.
		 *
		 * @author Thanasis Vergoulis
		 */
//		void print(void);
		
		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @attention This function must be executed only before replacing zero bitsets with zero
		 * pointers. Otherwise, use countEmptyBitsets().
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
//		unsigned long countZeros(void);

		/**
		 * It returns the number of bitsets that are full of zeros.
		 *
		 * @return The number of bitsets that are full of zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
//		unsigned long countEmptyBitsets(void);

//		double avgSetBits(void);

//		double countEmptyBlocks();
//		double countBlocks();
		
		/**
		 * It returns the number of set bits in all bitsets.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
//		int countSetBits(void);
		
		/**
		 * It returns the number of set bits in a particular bitset.
		 *
		 * @param bs_id The id of the desired bitset.
		 *
		 * @return The number of set bits.
		 *
		 * @author Thanasis Vergoulis
		 */
//		int countSetBits(unsigned long bs_id);
		
		/**
		 * It removes all the bitsets that are full of zeros and replaces them with a null pointer.
		 *
		 * @author Thanasis Vergoulis
		 */
//		void removeZeros(void);
		
		/** 
		 * It deletes the space allocated to _zeros.
		 *
		 * @author Thanasis Vergoulis
		 */
/*		static void freeZeros(void)
		{ 
//			delete HitmapBucket::_zeros_old;
			delete HitmapBucket::_zeros;
		}*/
		
/*		static SparseBitsetU* getZeros(void)
		{ 
//			return HitmapBucket::_zeros_old;
			return HitmapBucket::_zeros;
		}*/



		/**
		 * It returns the size of the hitmap bucket in bytes 
		 * @return The size of the hitmap bucket in bytes.
		 *
		 * @author Thanasis Vergoulis
		 */		
		double byteSize()
		{
			double size;
			double column_size;
			
			column_size = sizeof(unsigned long) + (this->_bs_num/8); //column_id + bitset size

			size = sizeof(vector<BucketCol>);
			size += this->_bucket_columns.size() * column_size;
			
			return size;
		}
	};

	/**
	 * It represents a set of counter bitsets.
	 *
	 * @var _cnt_bitsets Array containing references to the counter bitsets.
	 * @var _rem_bitsets Array containing references to the remainder bitsets. 
	 * @var _cnt_bitsets_num The number of counter bitsets (and the number of remainder bitsets as well).
	 * @var _cnt_num The number of counters encoded in the counter bitsets.
	 *
	 * @author Thanasis Vergoulis
	 */
	class CounterBitsets
	{
		SparseBitset** _cnt_bitsets_new;
		SparseBitset** _rem_bitsets_new;
		unsigned long _cnt_bitsets_num;
		unsigned long _cnt_num;
	public:
		/**
		 * The constructor.
		 *
		 * @param cnt_num The number of counters encoded in the counter bitsets. 
		 * @param t_max The maximum number the counter can count. 
		 * @param block_size The size of the blocks of each sparse bitset. 
		 *
		 * @author Thanasis Vergoulis
		 */
		CounterBitsets(unsigned long cnt_num, unsigned long t_max, unsigned long block_size);

		/**
		 * The destructor of the class.
		 *
		 * @author Thanasis Vergoulis
		 */
		~CounterBitsets();
		
		/**
		 * It calculates the t-set flags of a given set of bitsets and returns the result in the result bitset.
		 *
		 * @param bitsets Array containing the given set of bitsets.
		 * @param bitsets_num The number of bitsets.
		 * @param t The appearance threshold.
		 * @param result Reference to the result bitset.
		 *
		 * @author Thanasis Vergoulis
		 */
		void tSetFlagsCalc(SparseBitset** bitsets, unsigned long bitsets_num, unsigned long t, SparseBitset* result);

		/**
		 * It resets the counter bitsets.
		 *
		 * @author Thanasis Vergoulis
		 */
		void reset(void);
		
		/**
		 * It prints on standard output the contents of the counter & remainder bitsets.
		 *
		 * @author Thanasis Vergoulis
		 */
		void print(void);
	};

	/**
	 * It represents a Hitmap index. It is an StrHash having as bucket a HitmapBucket.  
	 *
	 * @var _dseq_ptr A pointer to the data sequence. 
	 * @var _dseq_len The length of the data sequence. 
	 * @var _part_len The length of each part of each partition.
	 * @var _part_shift The number of symbols for which the i-th part of the j-th partition is shifted in comparison to the i-th part of the (j-1)-th partition. 
	 * @var _partition_num The number of partitions in the partition set.
	 * @var _partition_size The number of parts in each partition. 
	 * @var _fnum The number of query fragments.
	 * @var _flen The length of query fragments.
	 * @var _qthr The cost threshold.
	 * @var _counters A pointer to a buffer of counter bitsets (used to computed t-set flags).
	 * @var _wins Hash table containing the windows that need to be verified.
	 * @var _results Vector containing the approx. occurrences of the query in the data sequence. 
	 * @var _stats Verification statistics.
	 *
	 * @author Thanasis Vergoulis
	 */
	class HitmapHash : public StrHash<HitmapBucket>
	{
		string* _dseq_ptr;
		unsigned long _dseq_len;
		unsigned long _part_len;
		unsigned long _part_shift;
		unsigned long _partition_num;
		unsigned long _partition_size; 
		unsigned long _fnum;
		unsigned long _flen;
		unsigned long _qthr;
		CounterBitsets* _counters;
		vector<int> _wins;
		vector<int> _results;
		VerificationStatistics _stats;
		SparseBitset* _tsetflags_new;
		unsigned long _block_size;
		unsigned long _blocks_num;
		string _ab;
	public:		
		/**
		 * The constructor of the class. 
		 *
		 * @param dseq_ptr A pointer to the data sequence.
		 * @param qthr The cost threshold.
		 * @param part_shift The number of symbols for which the i-th part of a partset is shifted in conmparison to its (i-1)-th part.
		 * @param flen The length of the query fragments (used also by StrHash as the length of key-strings.
		 * @param fnum The number of fragments. 
		 * @param block_size The size of the blocks of the sparse bitsets.
		 * @param buckets_num The number of buckets in the hash table (used by StrHash).
		 * @param buck_groups_num The number of buckets in each bucket group. 
		 *
		 * @author Thanasis Vergoulis
		 */
		HitmapHash( string* dseq_ptr, unsigned long qthr, unsigned long part_shift, unsigned long flen, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, unsigned long buck_groups_num);
		HitmapHash(void);
		
		/**
		 * The destructor of the class.
		 *
		 * @author Thanasis Vergoulis
		 */
		~HitmapHash(void);


		unsigned long getBucketsNum(void);
		unsigned long getFNum(void);
		unsigned long getQthr(void);
		unsigned long getFLen(void);
		string getAb(void);

		/**
		 * Estimates the average query cost based on the parameters given.
		 *
		 * @param dlen
		 * @param qlen
		 * @param qthr
		 * @param phi
		 * @param ab_size
		 * @param random_queries If TRUE, we assume query seqs are selected randomly. Otherwise, they are selected from a part of the data (thus it is sure that there is at least one alignment).
		 */
		static double estimateVerCost( unsigned long dlen, unsigned long qlen, unsigned long qthr, unsigned long phi, unsigned long ab_size, double cost_dp, bool random_queries);

		static double estimateFiltCost( unsigned long dlen, unsigned long qlen, unsigned long qthr, unsigned long phi, double cost_prob, double cost_bit);

		static double estimateCandidates(int dlen, int fnum, int flen, int qthr, int ab_size);
		
		static double estimateVerAreas(int dlen, int fnum, int flen, int qthr, int ab_size);

		void store(string output_file, string ab, double constr_time);

		void load(string input_file, string* dseq_ptr);
	
		/**
		 * It calculates the contents of the Hitmap's buckets.
		 *
		 * @author Thanasis Vergoulis
		 */
		void calcBuckets(void);

		/**
		 * It prints on the standard output statistics which are special for the Hitmap index. 
		 *
		 * @author Thanasis Vergoulis
		 */
		void extraStatistics();
	
		/**
		 * It returns the data sequence windows that need to be verified.
		 *
		 * @return A vector containing the start points of the windows.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getWindows(void);

		/**
		 * It returns the approx. occurrences of the query in the data.
		 *
		 * @return A vector containing the end poins of the approx. occurrences.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getResults(void);
	
		/**
		 * It guarantees that only unique results are in the results.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueResults(void);	
	
		/**
		 * It initializes the counter bitsets.
		 *
		 * @param t_max The maximum number the counter can count.
		 *
		 * @author Thanasis Vergoulis
		 */
		void initCounterBitsets(unsigned long t_max);	

		/**
		 * It calculates the verification statistics.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void setVerificationStatistics(string& qseq, string ab);
		
		/**
		 * It returns the verification statistics.
		 *
		 * @return The verification statistics.
		 *
		 * @author Thanasis Vergoulis
		 */
		VerificationStatistics getVerificationStatistics(void);

		/**
		 * It searches for a given string.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void findWindows(string& qseq);
		
		/**
		 * It sorts all duplicate windows and removes all duplicates.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueWindows(void);

		/**
		 * It verifies matches in all the candidate windows.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void verifyWindows(string& qseq);

		/**
		 * It returns the size of the Hitmap-hash table in bytes
		 *
		 * @return The size of the hash table in bytes.
		 *
		 * @author Thanasis Vergoulis
		 */
		double byteSize()
		{
			double size;

			size = sizeof(unsigned long);

			for(int i=0; i<this->_buckets_num; i++)
			{ 
				if( this->_buckets[0][i]!=0 )
				{
					size += this->_buckets[0][i]->byteSize();
				}
				else
				{
					size += sizeof(unsigned long);
				}
			}
			
			return size;
		}
	
		/**
		 * It prints in the standard output the current instance of the class. 
		 *
		 * @author Thanasis Vergoulis
		 */
//		void print();

		/**
		 * @depricated
		 *
		 * @author Thanasis Vergoulis
		 */
//		void verifyWindows(string& qseq, unsigned long epsilon, unsigned long fnum);
		
	};
	
	/**
	 * The bucket of the light version of Hitmap index (this used for low errors).
	 */
	struct HitmapLightBucket
	{
		SparseBitset* _hitmap;
	};
	
	/**
	 * The light version of Hitmap index, used for low errors. 
	 */
	class HitmapLightHash
	{
		string* _dseq_ptr;
		unsigned long _dseq_len;
		unsigned long _part_len;
		unsigned long _part_shift;
		unsigned long _fnum;
		unsigned long _flen;
		unsigned long _qthr;
		unsigned long _qlen;
		unsigned long _block_size;
		unsigned long _buckets_num;
		vector<int> _wins;
		vector<int> _results;
		VerificationStatistics _stats;
		string _ab;
		tr1::unordered_map<string, HitmapLightBucket> _hash_table;
	public:		
		HitmapLightHash( string* dseq_ptr, unsigned long fnum, unsigned long flen, unsigned long qlen, unsigned long qthr, unsigned long block_size, unsigned long buckets_num);
		HitmapLightHash( void );
		~HitmapLightHash( void );
		void build( void );
		void store( string output_file, string ab, double constr_time );
		void load( string input_file, string* dseq_ptr, unsigned long qlen);
		unsigned long getBucketsNum(void);
		unsigned long getFNum(void);
		unsigned long getQthr(void);
		unsigned long getFLen(void);
		string getAb(void);

		/**
		 * It calculates the verification statistics.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void setVerificationStatistics(string& qseq, string ab);
		
		/**
		 * It returns the verification statistics.
		 *
		 * @return The verification statistics.
		 *
		 * @author Thanasis Vergoulis
		 */
		VerificationStatistics getVerificationStatistics(void);

		/**
		 * It searches for a given string.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void findWindows(string& qseq);
		
		/**
		 * It sorts all duplicate windows and removes all duplicates.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueWindows(void);

		/**
		 * It verifies matches in all the candidate windows.
		 *
		 * @param qseq Reference to the query sequence.
		 *
		 * @author Thanasis Vergoulis
		 */
		void verifyWindows(string& qseq);

		/**
		 * It returns the data sequence windows that need to be verified.
		 *
		 * @return A vector containing the start points of the windows.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getWindows(void);

		/**
		 * It returns the approx. occurrences of the query in the data.
		 *
		 * @return A vector containing the end poins of the approx. occurrences.
		 *
		 * @author Thanasis Vergoulis
		 */
		vector<int> getResults(void);

		/**
		 * It guarantees that only unique results are in the results.
		 *
		 * @author Thanasis Vergoulis
		 */
		void keepUniqueResults(void);	

	};
	
#endif

	
}

#endif

