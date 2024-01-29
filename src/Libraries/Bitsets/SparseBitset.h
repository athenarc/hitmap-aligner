#ifndef SPARSE_BITSET_H
#define SPARSE_BITSET_H

#include <iostream>
#include <boost/dynamic_bitset.hpp>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include "../Misc/Utilities.h" //to use itoa

using namespace std;
using namespace boost;
/**
 * It implements a compressed form of SparseBitset (by using a blocks pool).
 *
 * @var _block_ptrs An array containing pointers to the blocks of the bitset.
 * @var _size The size of the bitset.
 * @var _block_size The size of each bitset block.
 * @var _block_num The number of blocks in each bitset.
 * @var _random_init It is true if the seed for random number generation is set (static).
 *
 * @author Thanasis Vergoulis
 */
class SparseBitsetUltraC
{
	//	dynamic_bitset<>** _block_ptrs;
	vector<dynamic_bitset<> *> _block_ptrs;
	dynamic_bitset<> _blocks_status;
	static int _size;
	static int _block_size;
	static int _blocks_num;
	static int _last_bit_id;
	static bool _random_init;
	static map<dynamic_bitset<>, short int> _blocks_pool;

public:
	/**
	 * The constructor of the class.
	 *
	 * @param size The size of the bitset.
	 * @param block_size The size of each bitset block.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetUltraC(int size, int block_size);

	/**
	 * The constructor of the class.
	 *
	 * @param o A reference to another object to be copied.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetUltraC(const SparseBitsetUltraC &o);

	/**
	 * The constructor of the class.
	 *
	 * @param o A pointer to another object to be copied.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetUltraC(const SparseBitsetUltraC *o);

	/**
	 * The destructor of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	~SparseBitsetUltraC();

	/**
	 * It deletes all contents of the block pool.
	 *
	 * @author Thanasis Vergoulis
	 */
	static void cleanBlockPool(void);

	/**
	 * It returns the number of blocks in the sparse bitset (static version).
	 *
	 * @return The number of blocks in the sparse bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	static int getBlocksNumSt(void)
	{
		return SparseBitsetUltraC::_blocks_num;
	}

	/**
	 * It returns the number of bytes used by the block pool.
	 *
	 * @return The number of bytes used by the block pool.
	 *
	 * @author Thanasis Vergoulis
	 */
	static double poolByteSize(void);

	/**
	 * It returns the number of blocks in the block pool.
	 *
	 * @author Thanasis Vergoulis
	 */
	static double poolSize(void)
	{
		return SparseBitsetUltraC::_blocks_pool.size();
	}

	/**
	 * It returns the number of empty blocks in the bitset.
	 *
	 * @return The number of empty blocks in the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	double countEmptyBlocks(void);

	/**
	 * It returns the size of the sparse bitset in bytes with or without considering the
	 * optimizations.
	 *
	 * @param no_opts If true, no optimizations are considered.
	 *
	 * @return The size of the bitset in bytes.
	 */
	double byteSize(bool no_opts);

	/**
	 * It resets the bitset.
	 *
	 * @attention PREPEI NA KANEI APLOS MIDEN !!! NA DO AN THELEI ALLAGI
	 *
	 * @author Thanasis Vergoulis
	 */
	void clear(void);

	/**
	 * It resets the bitset.
	 */
	void reset(void);

	/**
	 * It sets the bit_id-th bit of the bitset.
	 *
	 * @param bit_id The bit id.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(int bit_id);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset The given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> bitset);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> *bitset_ptr);

	/**
	 * The same as set, just stop copying if the rest of the bits are zeros.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void setEfficient(dynamic_bitset<> *bitset_ptr);

	/**
	 * It sets all the bits of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(void);

	/**
	 * It returns true if the bitset is empty.
	 *
	 * @return True if the bitset is empty, false otherwise.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool empty(void);

	/**
	 * It returns the size of the bitset.
	 *
	 * @return The size of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);

	/**
	 * It displays the contents of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);

	void flipNonEmptyBlock(int bit_id);

	void flipNonEmptyBlock(int block_id, int block_bit_id);

	bool runNoneInBlock(int block_id);

	/**
	 * It returns true if the bit_id-th bit of the bitset is set.
	 *
	 * @param bit_id The id of the bit.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool test(int bit_id);

	/**
	 * The same as test, but needs to run only on non-empty blocks (i.e., is faster).
	 *
	 * @param bit_id
	 *
	 * @author Thanasis Vergoulis
	 */
	bool testInNonEmptyBlock(int bit_id);

	bool testInNonEmptyBlock(int block_id, int block_bit_id);

	bool testBlock(int block_id);
	dynamic_bitset<> getBlock(int block_id);

	/**
	 * It returns the bitset.
	 *
	 * @return The bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	dynamic_bitset<> getBitset(void);

	/**
	 * It sets randomly (at most) bit_num bits of the bitset.
	 *
	 * @param bit_num The number of bitsets to be set.
	 *
	 * @author Thanasis Vergoulis
	 */
	void randomSet(int bit_num);

	/**
	 * It returns the number of set bits.
	 *
	 * @return The nubmer of set bits.
	 *
	 * @author Thanasis Vergoulis
	 */
	int count(void);

	int getBlocksNum(void);

	int getBlockSize(void);

	/**
	 * It sets the seed for the random number generation.
	 *
	 * @author Thanasis Vergoulis
	 */
	static void randomInit(void)
	{
		if (!SparseBitsetUltraC::_random_init)
		{
			srand(time(NULL));
			SparseBitsetUltraC::_random_init = true;
		}
	}

	/**
	 * The equality operator of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool operator==(const SparseBitsetUltraC &o) const
	{
		for (int i = 0; i < SparseBitsetUltraC::_blocks_num; i++)
		{
			if (this->_block_ptrs[i] == NULL)
			{
				if (o._block_ptrs[i] != NULL)
				{
					return false;
				}
			}
			else
			{
				if (o._block_ptrs[i] == NULL)
				{
					return false;
				}
				else if ((*o._block_ptrs[i]) != (*this->_block_ptrs[i]))
				{
					return false;
				}
			}
		}
		return true;
	}

	/**
	 * The inequality operator of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool operator!=(const SparseBitsetUltraC &o) const
	{
		return !(*this == o);
	}
};

/**
 * It implements a compressed form of SparseBitset (by using a blocks pool).
 *
 * @var _block_ptrs An array containing pointers to the blocks of the bitset.
 * @var _size The size of the bitset.
 * @var _block_size The size of each bitset block.
 * @var _block_num The number of blocks in each bitset.
 * @var _random_init It is true if the seed for random number generation is set (static).
 *
 * @author Thanasis Vergoulis
 */
class SparseBitsetC
{
	dynamic_bitset<> **_block_ptrs;
	static int _size;
	static int _block_size;
	static int _blocks_num;
	static int _last_bit_id;
	static bool _random_init;
	static map<dynamic_bitset<>, short int> _blocks_pool;

public:
	/**
	 * The constructor of the class.
	 *
	 * @param size The size of the bitset.
	 * @param block_size The size of each bitset block.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetC(int size, int block_size);

	/**
	 * The constructor of the class.
	 *
	 * @param o A reference to another object to be copied.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetC(const SparseBitsetC &o);

	/**
	 * The constructor of the class.
	 *
	 * @param o A pointer to another object to be copied.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetC(const SparseBitsetC *o);

	/**
	 * The destructor of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	~SparseBitsetC();

	/**
	 * It deletes all contents of the block pool.
	 *
	 * @author Thanasis Vergoulis
	 */
	static void cleanBlockPool(void);

	/**
	 * It returns the number of blocks in the sparse bitset (static version).
	 *
	 * @return The number of blocks in the sparse bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	static int getBlocksNumSt(void)
	{
		return SparseBitsetC::_blocks_num;
	}

	/**
	 * It returns the number of bytes used by the block pool.
	 *
	 * @return The number of bytes used by the block pool.
	 *
	 * @author Thanasis Vergoulis
	 */
	static double poolByteSize(void);

	/**
	 * It returns the number of blocks in the block pool.
	 *
	 * @author Thanasis Vergoulis
	 */
	static double poolSize(void)
	{
		return SparseBitsetC::_blocks_pool.size();
	}

	/**
	 * It returns the number of empty blocks in the bitset.
	 *
	 * @return The number of empty blocks in the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	double countEmptyBlocks(void);

	/**
	 * It returns the size of the sparse bitset in bytes with or without considering the
	 * optimizations.
	 *
	 * @param no_opts If true, no optimizations are considered.
	 *
	 * @return The size of the bitset in bytes.
	 */
	double byteSize(bool no_opts);

	/**
	 * It resets the bitset.
	 *
	 * @attention PREPEI NA KANEI APLOS MIDEN !!! NA DO AN THELEI ALLAGI
	 *
	 * @author Thanasis Vergoulis
	 */
	void clear(void);

	/**
	 * It resets the bitset.
	 */
	void reset(void);

	/**
	 * It sets the bit_id-th bit of the bitset.
	 *
	 * @param bit_id The bit id.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(int bit_id);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset The given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> bitset);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> *bitset_ptr);

	/**
	 * The same as set, just stop copying if the rest of the bits are zeros.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void setEfficient(dynamic_bitset<> *bitset_ptr);

	/**
	 * It sets all the bits of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(void);

	/**
	 * It returns true if the bitset is empty.
	 *
	 * @return True if the bitset is empty, false otherwise.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool empty(void);

	/**
	 * It returns the size of the bitset.
	 *
	 * @return The size of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);

	/**
	 * It displays the contents of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);

	void flipNonEmptyBlock(int bit_id);

	void flipNonEmptyBlock(int block_id, int block_bit_id);

	bool runNoneInBlock(int block_id);

	/**
	 * It returns true if the bit_id-th bit of the bitset is set.
	 *
	 * @param bit_id The id of the bit.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool test(int bit_id);

	/**
	 * The same as test, but needs to run only on non-empty blocks (i.e., is faster).
	 *
	 * @param bit_id
	 *
	 * @author Thanasis Vergoulis
	 */
	bool testInNonEmptyBlock(int bit_id);

	bool testInNonEmptyBlock(int block_id, int block_bit_id);

	bool testBlock(int block_id);
	dynamic_bitset<> getBlock(int block_id);

	/**
	 * It returns the bitset.
	 *
	 * @return The bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	dynamic_bitset<> getBitset(void);

	/**
	 * It sets randomly (at most) bit_num bits of the bitset.
	 *
	 * @param bit_num The number of bitsets to be set.
	 *
	 * @author Thanasis Vergoulis
	 */
	void randomSet(int bit_num);

	/**
	 * It returns the number of set bits.
	 *
	 * @return The nubmer of set bits.
	 *
	 * @author Thanasis Vergoulis
	 */
	int count(void);

	int getBlocksNum(void);

	int getBlockSize(void);

	/**
	 * It sets the seed for the random number generation.
	 *
	 * @author Thanasis Vergoulis
	 */
	static void randomInit(void)
	{
		if (!SparseBitsetC::_random_init)
		{
			srand(time(NULL));
			SparseBitsetC::_random_init = true;
		}
	}

	/**
	 * The equality operator of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool operator==(const SparseBitsetC &o) const
	{
		for (int i = 0; i < SparseBitsetC::_blocks_num; i++)
		{
			if (this->_block_ptrs[i] == NULL)
			{
				if (o._block_ptrs[i] != NULL)
				{
					return false;
				}
			}
			else
			{
				if (o._block_ptrs[i] == NULL)
				{
					return false;
				}
				else if ((*o._block_ptrs[i]) != (*this->_block_ptrs[i]))
				{
					return false;
				}
			}
		}
		return true;
	}

	/**
	 * The inequality operator of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool operator!=(const SparseBitsetC &o) const
	{
		return !(*this == o);
	}
};

/**
 * It implements bit operations on sparse bitsets with efficient way. Moreover, it uses
 * the unit vector blocks pool to reduce space.
 *
 * @var _block_ptrs An array containing pointers to the blocks of the bitset.
 * @var _size The size of the bitset.
 * @var _block_size The size of each bitset block.
 * @var _block_num The number of blocks in each bitset.
 * @var _random_init It is true if the seed for random number generation is set (static).
 * @var _unit_vector_blocks_pool The pool that contains all the possible unit vectors (static).
 *
 * @author Thanasis Vergoulis
 */
class SparseBitsetU
{
	dynamic_bitset<> **_block_ptrs;
	static int _size;
	static int _block_size;
	static int _blocks_num;
	static int _last_bit_id;
	static bool _random_init;
	static dynamic_bitset<> *_unit_vector_blocks_pool;
	static map<dynamic_bitset<>, int> _blocks_pool;

public:
	/**
	 * The constructor of the class.
	 *
	 * @param size The size of the bitset.
	 * @param block_size The size of each bitset block.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetU(int size, int block_size);

	/**
	 * The constructor of the class.
	 *
	 * @param o A reference to another object to be copied.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetU(const SparseBitsetU &o);

	/**
	 * The constructor of the class.
	 *
	 * @param o A pointer to another object to be copied.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitsetU(const SparseBitsetU *o);

	/**
	 * The destructor of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	~SparseBitsetU();

	/**
	 * It returns the number of blocks in the sparse bitset (static version).
	 *
	 * @return The number of blocks in the sparse bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	static int getBlocksNumSt(void)
	{
		return SparseBitsetU::_blocks_num;
	}

	/**
	 */
	void updateBlocksPool(void)
	{
		map<dynamic_bitset<>, int>::iterator it;
		for (int i = 0; i < SparseBitsetU::_blocks_num; i++)
		{
			if (this->_block_ptrs[i] != NULL)
			{
				it = SparseBitsetU::_blocks_pool.find(*this->_block_ptrs[i]);
				if (it == SparseBitsetU::_blocks_pool.end()) // if the first occurrence
				{
					SparseBitsetU::_blocks_pool.insert(std::pair<dynamic_bitset<>, int>(*this->_block_ptrs[i], 1));
				}
				else
				{
					SparseBitsetU::_blocks_pool[(*this->_block_ptrs[i])]++;
				}
			}
		}
	}

	static void blocksPoolStats(void)
	{
		cout << "Number of distinct blocks: " << SparseBitsetU::_blocks_pool.size() << endl;
		map<dynamic_bitset<>, int>::iterator it;
		unsigned int size;
		size = 0;
		it = SparseBitsetU::_blocks_pool.begin();
		while (it != SparseBitsetU::_blocks_pool.end())
		{
			size += it->second;
			it++;
		}
		cout << "Total number of non-empty blocks: " << size << endl;
	}

	void shiftLSB(int shift_by);
	double getEmptyBlocksNum();
	double getUnaryBlocksNum();

	/**
	 * It returns the size of the sparse bitset in bytes with or without considering the
	 * unit vector blocks (stored in UVB pool). The actual size is if uvb_blocks is false.
	 *
	 * @param uvb_blocks If it is true the size of the unit vector blocks is considered, otherwise it is not.
	 *
	 * @return The size of the bitset in bytes.
	 */
	double byteSize(bool uvb_blocks)
	{
		double size;

		// Consider the size of the block pointers.
		size = SparseBitsetU::_blocks_num * sizeof(dynamic_bitset<> *);

		// Consider the size of the blocks.
		for (int i = 0; i < SparseBitsetU::_blocks_num; i++)
		{
			if (this->_block_ptrs[i] != NULL) // if null pointer then no block!
			{
				if (uvb_blocks)
				{
					size += this->_block_ptrs[i]->num_blocks() * sizeof(unsigned long);
					//@note boost::dynamic_bitset<> stores the bitset as a set of blocks,
					// where each block is an integer (default: unsigned long)
				}
				else
				{
					if (this->_block_ptrs[i]->count() != 1)
					{
						size += this->_block_ptrs[i]->num_blocks() * sizeof(unsigned long);
						//@note boost::dynamic_bitset<> stores the bitset as a set of blocks,
						// where each block is an integer (default: unsigned long)
					}
				}
			}
		}
		return size;
	}

	static double UVBpoolByteSize(void)
	{
		return SparseBitsetU::_block_size * SparseBitsetU::_unit_vector_blocks_pool[0].num_blocks() * sizeof(unsigned long);
	}

	/**
	 * It compares the current SparseBitsetU with another one.
	 *
	 * @param o The other SparseBitsetU instance.
	 *
	 * @return True if the two instances are equal, false otherwise.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool isEqual(SparseBitsetU *o)
	{
		for (int i = 0; i < SparseBitsetU::_blocks_num; i++)
		{ // cout<<"~i="<<i<<endl;
			if (this->_block_ptrs[i] == NULL && o->_block_ptrs[i] == NULL)
			{ // cout<<"case 1"<<endl;
			  // do nothing
			}
			else if (this->_block_ptrs[i] == NULL || o->_block_ptrs[i] == NULL)
			{ // cout<<"case 2"<<endl;
				return false;
			}
			else
			{ // cout<<"case 3"<<endl;
				if (this->_block_ptrs[i] != o->_block_ptrs[i])
					return false;
			}
		}
		return true;
	}

	/**
	 * It resets the bitset.
	 *
	 * @attention PREPEI NA KANEI APLOS MIDEN !!! NA DO AN THELEI ALLAGI
	 *
	 * @author Thanasis Vergoulis
	 */
	void clear(void);

	/**
	 * It resets the bitset.
	 */
	void reset(void);

	/**
	 * It sets the bit_id-th bit of the bitset.
	 *
	 * @param bit_id The bit id.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(int bit_id);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset The given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> bitset);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> *bitset_ptr);

	/**
	 * The same as set, just stop copying if the rest of the bits are zeros.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void setEfficient(dynamic_bitset<> *bitset_ptr);

	/**
	 * It sets all the bits of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(void);

	/**
	 * It returns true if the bitset is empty.
	 *
	 * @return True if the bitset is empty, false otherwise.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool empty(void);

	/**
	 * It returns the size of the bitset.
	 *
	 * @return The size of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);

	/**
	 * It displays the contents of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);

	void flipNonEmptyBlock(int bit_id);

	void flipNonEmptyBlock(int block_id, int block_bit_id);

	bool runNoneInBlock(int block_id);

	/**
	 * It returns true if the bit_id-th bit of the bitset is set.
	 *
	 * @param bit_id The id of the bit.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool test(int bit_id);

	/**
	 * The same as test, but needs to run only on non-empty blocks (i.e., is faster).
	 *
	 * @param bit_id
	 *
	 * @author Thanasis Vergoulis
	 */
	bool testInNonEmptyBlock(int bit_id);

	bool testInNonEmptyBlock(int block_id, int block_bit_id);

	bool testBlock(int block_id);
	dynamic_bitset<> getBlock(int block_id);

	/**
	 * It returns the bitset.
	 *
	 * @return The bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	dynamic_bitset<> getBitset(void);

	/**
	 * It sets randomly (at most) bit_num bits of the bitset.
	 *
	 * @param bit_num The number of bitsets to be set.
	 *
	 * @author Thanasis Vergoulis
	 */
	void randomSet(int bit_num);

	/**
	 * It returns the number of set bits.
	 *
	 * @return The nubmer of set bits.
	 *
	 * @author Thanasis Vergoulis
	 */
	int count(void);

	int getBlocksNum(void);

	int getBlockSize(void);

	/**
	 * It sets the seed for the random number generation.
	 *
	 * @author Thanasis Vergoulis
	 */
	static void randomInit(void)
	{
		if (!SparseBitsetU::_random_init)
		{
			srand(time(NULL));
			SparseBitsetU::_random_init = true;
		}
	}
};

/**
 * It implements bit operations on sparse bitsets with efficient way.
 *
 * @var _block_ptrs An array containing pointers to the blocks of the bitset.
 * @var _size The size of the bitset.
 * @var _block_size The size of each bitset block.
 * @var _block_num The number of blocks in each bitset.
 * @var _random_init It is true if the seed for random number generation is set (static).
 *
 * @author Thanasis Vergoulis
 */
class SparseBitset
{
	dynamic_bitset<> **_block_ptrs;
	static int _size;
	static int _block_size;
	static int _blocks_num;
	static int _last_bit_id;
	static bool _random_init;

public:
	/**
	 * The constructor of the class.
	 *
	 * @param size The size of the bitset.
	 * @param block_size The size of each bitset block.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitset(int size, int block_size);

	/**
	 * The constructor of the class.
	 *
	 * @param o A reference to another object to be copied.
	 *
	 * @author Thanasis Vergoulis
	 */
	SparseBitset(const SparseBitset &o);

	SparseBitset(const SparseBitset *o);
	~SparseBitset();

	static int getBlocksNumSt() { return SparseBitset::_blocks_num; }

	void shiftLSB(int shift_by);

	/**
	 * It decompresses the bitset o (compressed by unit-vector-block-pool method) and
	 * shifts it to the least significant bit by shift_by positions.
	 *
	 * @param shift_by The number of positions to be shifted.
	 * @param o The compressed bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void shiftLSB(int shift_by, SparseBitsetU *o);

	void compare(SparseBitset &bitset)
	{
		for (int i = 0; i < SparseBitset::_size; i++)
		{
			if (this->test(i) != bitset.test(i))
			{
				int x;
				cout << "problem!!! in " << i << endl;
				cout << "cur: " << endl;
				this->print();
				cout << "oth: " << endl;
				bitset.print();
				cin >> x;
			}
		}
	};

	/**
	 * It decompresses a SparseBitsetU into a SparseBitset.
	 *
	 * @param o The instance of SparseBitsetU to be decompressed.
	 *
	 * @author Thanasis Vergoulis
	 */
	void decompress(SparseBitsetU *o)
	{
		for (int i = 0; i < SparseBitset::_blocks_num; i++)
		{
			if (o->testBlock(i))
			{
				if (this->_block_ptrs[i] != NULL)
					delete this->_block_ptrs[i];
				this->_block_ptrs[i] = new dynamic_bitset<>(o->getBlock(i));
			}
			else
			{
				delete this->_block_ptrs[i];
				this->_block_ptrs[i] = NULL;
			}
		}
	}

	/**
	 * It decompresses a SparseBitsetU into a SparseBitset.
	 *
	 * @param o The instance of SparseBitsetU to be decompressed.
	 *
	 * @author Thanasis Vergoulis
	 */
	void decompress(SparseBitsetC *o)
	{
		for (int i = 0; i < SparseBitset::_blocks_num; i++)
		{ // cout<<"block "<<i<<endl;
			if (o->testBlock(i))
			{
				if (this->_block_ptrs[i] != NULL)
					delete this->_block_ptrs[i];
				this->_block_ptrs[i] = new dynamic_bitset<>(o->getBlock(i));
			}
			else
			{
				delete this->_block_ptrs[i];
				this->_block_ptrs[i] = NULL;
			}
		}
		//		cout<<"in_decompress:"<<endl;
		//		this->print();
	}

	void decompress(SparseBitsetUltraC *o)
	{
		for (int i = 0; i < SparseBitset::_blocks_num; i++)
		{
			if (o->testBlock(i))
			{
				if (this->_block_ptrs[i] != NULL)
					delete this->_block_ptrs[i];
				this->_block_ptrs[i] = new dynamic_bitset<>(o->getBlock(i));
			}
			else
			{
				delete this->_block_ptrs[i];
				this->_block_ptrs[i] = NULL;
			}
		}
	}

	void shiftMSB(int shift_by);

	double getEmptyBlocksNum();
	double getUnaryBlocksNum();

	/**
	 * It compares the sparse bitset with a regular one. If there is a difference, it
	 * presents it.
	 *
	 * @param bitset The regular bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void compareWithRegBitset(dynamic_bitset<> *bitset)
	{
		for (int i = 0; i < SparseBitset::_size; i++)
		{
			if (this->test(i) != bitset->test(i))
			{
				int x;
				cout << "PROBLEM!! Bit " << i << " is different!!" << endl;
				cout << "Sparse:" << endl;
				this->print();
				cout << "Regular:" << endl;
				cout << (*bitset) << endl;
				cin >> x;
			}
		}
		cout << "OK" << endl;
	};

	/**
	 * It resets the bitset.
	 *
	 * @attention PREPEI NA KANEI APLOS MIDEN !!! NA DO AN THELEI ALLAGI
	 *
	 * @author Thanasis Vergoulis
	 */
	void clear(void);

	/**
	 * It resets the bitset.
	 */
	void reset(void);

	/**
	 * It sets the bit_id-th bit of the bitset.
	 *
	 * @param bit_id The bit id.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(int bit_id);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset The given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> bitset);

	/**
	 * It sets all the bits of the bitset according to a given bitset.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(dynamic_bitset<> *bitset_ptr);

	/**
	 * The same as set, just stop copying if the rest of the bits are zeros.
	 *
	 * @param bitset_ptr A pointer to the given bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void setEfficient(dynamic_bitset<> *bitset_ptr);

	/**
	 * It sets all the bits of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void set(void);

	/**
	 * It returns true if the bitset is empty.
	 *
	 * @return True if the bitset is empty, false otherwise.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool empty(void);

	/**
	 * It returns the size of the bitset.
	 *
	 * @return The size of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);

	/**
	 * It displays the contents of the bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);

	string serialize(void);

	string getOnesListDelim(void);

	void flipNonEmptyBlock(int bit_id);

	void flipNonEmptyBlock(int block_id, int block_bit_id);

	bool runNoneInBlock(int block_id);

	/**
	 * It returns true if the bit_id-th bit of the bitset is set.
	 *
	 * @param bit_id The id of the bit.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool test(int bit_id);

	/**
	 * The same as test, but needs to run only on non-empty blocks (i.e., is faster).
	 *
	 * @param bit_id
	 *
	 * @author Thanasis Vergoulis
	 */
	bool testInNonEmptyBlock(int bit_id);

	bool testInNonEmptyBlock(int block_id, int block_bit_id);

	bool testBlock(int block_id);

	/**
	 * It returns the bitset.
	 *
	 * @return The bitset.
	 *
	 * @author Thanasis Vergoulis
	 */
	dynamic_bitset<> getBitset(void);

	/**
	 * It sets randomly (at most) bit_num bits of the bitset.
	 *
	 * @param bit_num The number of bitsets to be set.
	 *
	 * @author Thanasis Vergoulis
	 */
	void randomSet(int bit_num);

	/**
	 * It returns the number of set bits.
	 *
	 * @return The nubmer of set bits.
	 *
	 * @author Thanasis Vergoulis
	 */
	int count(void);

	int getBlocksNum(void);

	int getBlockSize(void);

	/**
	 * It sets the seed for the random number generation.
	 *
	 * @author Thanasis Vergoulis
	 */
	static void randomInit(void)
	{
		if (!SparseBitset::_random_init)
		{
			srand(time(NULL));
			SparseBitset::_random_init = true;
		}
	}

	SparseBitset &operator&=(const SparseBitset &o)
	{
		for (int i = 0; i < this->_blocks_num; i++)
		{
			if (this->_block_ptrs[i] == NULL) // 0 AND x = 0, thus don't do anything
			{
				continue;
			}
			else if (o._block_ptrs[i] == NULL) // x AND 0 = 0, thus transform the block to zero
			{
				delete this->_block_ptrs[i];
				this->_block_ptrs[i] = NULL;
			}
			else
			{
				(*this->_block_ptrs[i]) &= (*o._block_ptrs[i]);
				// Check if the result becomes zero
				if (this->_block_ptrs[i]->none())
				{
					delete this->_block_ptrs[i];
					this->_block_ptrs[i] = NULL;
				}
			}
		}
		return *this;
	}

	void selective_and(SparseBitset *o, vector<int> &v)
	{
		vector<int>::iterator it;
		it = v.begin();

		while (it != v.end())
		//		for(int i=0; i<this->_blocks_num; i++)
		{
			if (this->_block_ptrs[(*it)] == NULL) // 0 AND x = 0, thus don't do anything
			{
				it++;
				continue;
			}
			else if (o->_block_ptrs[(*it)] == NULL) // x AND 0 = 0, thus transform the block to zero
			{
				delete this->_block_ptrs[(*it)];
				this->_block_ptrs[(*it)] = NULL;
			}
			else
			{
				(*this->_block_ptrs[(*it)]) &= (*o->_block_ptrs[(*it)]);

				// Check if the result becomes zero
				if (this->_block_ptrs[(*it)]->none())
				{
					delete this->_block_ptrs[(*it)];
					this->_block_ptrs[(*it)] = NULL;
				}
			}
			it++;
		}
	}

	SparseBitset &operator|=(const SparseBitset &o)
	{
		for (int i = 0; i < this->_blocks_num; i++)
		{
			if (o._block_ptrs[i] != NULL)
			{
				if (this->_block_ptrs[i] == NULL)
				{
					this->_block_ptrs[i] = new dynamic_bitset<>((*o._block_ptrs[i]));
				}
				else
				{
					(*this->_block_ptrs[i]) |= (*o._block_ptrs[i]);
					// We don't need to check if the result becomes zero, because OR with
					// something that is not zero, can never become zero.
				}
			}
		}
		return *this;
	}

	void selective_or(SparseBitset *o, vector<int> &v)
	{
		vector<int>::iterator it;
		it = v.begin();

		//		for(int i=0;i<this->_blocks_num; i++)
		while (it != v.end())
		{
			if (o->_block_ptrs[(*it)] != NULL)
			{
				if (this->_block_ptrs[(*it)] == NULL)
				{
					this->_block_ptrs[(*it)] = new dynamic_bitset<>((*o->_block_ptrs[(*it)]));
				}
				else
				{
					(*this->_block_ptrs[(*it)]) |= (*o->_block_ptrs[(*it)]);

					// We don't need to check if the result becomes zero, because OR with
					// something that is not zero, can never become zero.
				}
			}
			it++;
		}
	}

	SparseBitset &operator^=(const SparseBitset &o)
	{
		for (int i = 0; i < this->_blocks_num; i++)
		{
			if (o._block_ptrs[i] != NULL)
			{
				if (this->_block_ptrs[i] == NULL)
				{
					this->_block_ptrs[i] = new dynamic_bitset<>((*o._block_ptrs[i]));
				}
				else
				{
					(*this->_block_ptrs[i]) ^= (*o._block_ptrs[i]);

					// Check if the result becomes zero
					if (this->_block_ptrs[i]->none())
					{
						delete this->_block_ptrs[i];
						this->_block_ptrs[i] = NULL;
					}
				}
			}
		}
		return *this;
	}

	void selective_xor(SparseBitset *o, vector<int> &v)
	{
		vector<int>::iterator it;
		it = v.begin();

		while (it != v.end())
		{
			if (o->_block_ptrs[(*it)] != NULL)
			{
				if (this->_block_ptrs[(*it)] == NULL)
				{
					this->_block_ptrs[(*it)] = new dynamic_bitset<>((*o->_block_ptrs[(*it)]));
				}
				else
				{
					(*this->_block_ptrs[(*it)]) ^= (*o->_block_ptrs[(*it)]);

					// Check if the result becomes zero
					if (this->_block_ptrs[(*it)]->none())
					{
						delete this->_block_ptrs[(*it)];
						this->_block_ptrs[(*it)] = NULL;
					}
				}
			}
			it++;
		}
	}

	SparseBitset &operator=(const SparseBitset &o)
	{
		if (this == &o)
			return *this;

		for (int i = 0; i < this->_blocks_num; i++)
		{
			if (this->_block_ptrs[i] != NULL)
				delete this->_block_ptrs[i];

			if (o._block_ptrs[i] != NULL)
				this->_block_ptrs[i] = new dynamic_bitset<>((*o._block_ptrs[i]));
			else
				this->_block_ptrs[i] = NULL;
		}

		return *this;
	}

	SparseBitset operator&(const SparseBitset &o)
	{
		SparseBitset tmp(o);
		tmp &= (*this);
		return tmp;
	}

	void selective_and(SparseBitset *o1, SparseBitset *o2, vector<int> &v)
	{
		(*this) = (*o1);
		this->selective_and(o2, v);
	}
};

#endif