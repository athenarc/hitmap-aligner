/************************************************************************************************//*
 * @file SHash.h
 *
 * This file contain declarations about a hash table having lists for substrings of a data string. 
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/
#ifndef SHASH_H
#define SHASH_H

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <cmath>
#include <tr1/unordered_map>
#include <vector>
#include <map>
#include <cstring>
#include <sstream>
#include <bitset>
#include <unistd.h>
#include <limits.h>
#include <cstdlib> //to use atoi etc.
#include "../../Math/MyMaths.h"
#include "../../Misc/MemObserver.h"
using namespace std;

#define HITMAP_SIZE 10000

/**
 * @brief Inverted list.
 *
 * @details A list containing positions on the string. These are the start positions of occurrences
 * of a specific substring. 
 *
 * @var _position The start position of the occurrence. 
 *
 * @author Thanasis Vergoulis
 */
struct IList
{
	list<int> _positions;
};

/**
 * @brief The string hash.
 *
 * @var _h_table The hash table containing the inverted lists for each substring.
 *
 * @author Thanasis Vergoulis
 */
class SHash
{
	tr1::unordered_map<string,IList> _h_table;
public:
	/**
	 * @brief The constructor for substrings of ONE length.
	 *
	 * @param data_str The data string.
	 * @param gram_length The length of the keys.
	 *
	 * @author Thanasis Vergoulis
	 */
	SHash( string& data_str, int gram_length);
	
	/**
	 * @brief The constructor for substrings having length in a range.
	 *
	 * @param data_str The data string.
	 * @param min_gram_length The mininum length for the keys.
	 * @param max_gram_length The maximum length for the keys.
	 *
	 * @author Thanasis Vergoulis
	 */
	SHash( string& data_str, int min_gram_length, int max_gram_length);
	
	/**
	 * @brief The destructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	~SHash(void);
	
	/**
	 * @brief It returns the number of distinct keys in the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);
	
	/**
	 * @brief It returns the number bytes needed to store the hash in memory (or on the disk). 
	 *
	 * @author Thanasis Vergoulis
	 */
	int byteSize(void);
	
	/**
	 * @brief It returns an iterator to the entry which matches the query. It returns end otherwise.
	 *
	 * @param query_str The search string.
	 *
	 * @author Thanasis Vergoulis
	 */
	tr1::unordered_map<string,IList>::iterator find(string query_str);
	
	/**
	 * @brief Checks if the current iterator is end. 
	 *
	 * @details If it=end it returns false, otherwise it returns true.
	 *
	 * @param it The iterator. 
	 *
	 * @author Thanasis Vergoulis
	 */
	bool exists(tr1::unordered_map<string,IList>::iterator it);
	
	/**
	 * @brief It prints the contents of the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);
	
	/**
	 * @brief It prints statistics of the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void showStats(void);
};

/**
 */
struct Position
{
	unsigned long _position;
	unsigned short int _type; //2B -- 0 to 65,535
//	unsigned short int _order;
};

/**
 * @brief The String Hash used by Dirichlet method.
 *
 * @details The String Hash is implemented as follows: The hash table is implemented as an array of
 * pointers. Each pointer corresponds to one bucket. The number of buckets is determined during the
 * construction of the object. Each bucket is implemented as a vector of items. 
 *
 * @var _h_table The hash table containing the inverted lists for each fragment.
 * @var _hash_func The function used for hashing.
 * @var _buckets_num The number of buckets of the hash table. 
 *
 * @author Thanasis Vergoulis
 */
class SHashDir
{
protected:
	vector<unsigned long>** _h_table;
	tr1::unordered_map<string,int>::hasher _hash_func;
	unsigned long _buckets_num;
public:
	/**
	 * @brief The constructor for substrings of ONE length.
	 *
	 * @param data_str The data string.
	 * @param gram_length The length of the keys.
	 *
	 * @author Thanasis Vergoulis
	 */
	SHashDir( string& data_str, unsigned long gram_length);

	/** SILENT CONSTRUCTOR TODO FTIAKSE TO
	 */
	SHashDir( string& data_str, unsigned long gram_length, string mode);
	
	/**
	 * @brief The destructor.
	 *
	 * @note At this time it does nothing. 
	 *
	 * @author Thanasis Vergoulis
	 */
	~SHashDir(void);

	/**
	 * @brief Free the memory used by the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void clear();
	
	/**
	 * @brief It returns the number of distinct keys in the hash table.
	 *
	 * @attention NOT IMPLEMENTED
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);
	
	/**
	 * @brief It returns the number bytes needed to store the hash in memory (or on the disk). 
	 *
	 * @attention NOT IMPLEMENTED
	 *
	 * @author Thanasis Vergoulis
	 */
	int byteSize(void);
	
	/**
	 * @brief It returns an iterator to the entry which matches the query. It returns end otherwise.
	 *
	 * @param query_str The search string.
	 *
	 * @author Thanasis Vergoulis
	 */
	vector<unsigned long>* find(string query_str);

	vector<unsigned long>* get(unsigned long i);
	
	unsigned long getBucketsNum();
	
	/**
	 * @brief Checks if the current iterator is end. 
	 *
	 * @details If it=end it returns false, otherwise it returns true.
	 *
	 * @attention NOT IMPLEMENTED
	 *
	 * @param it The iterator. 
	 *
	 * @author Thanasis Vergoulis
	 */
	bool exists(tr1::unordered_map<string,IList>::iterator it);
	
	/**
	 * @brief It prints the contents of the hash table.
	 *
	 * @attention NOT IMPLEMENTED
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);
	
	/**
	 * @brief It prints statistics of the hash table.
	 *
	 * @attention NOT IMPLEMENTED
	 *
	 * @author Thanasis Vergoulis
	 */
	void showStats(void);
};

/**
 * @brief The HitmapTable needed by the List Reduction method.
 *
 * @var _hits ...
 * @var _hitmaps_num The number of hitmaps (i.e., rows) of the table.
 * @var _hitmap_size The size of each hitmap (i.e., the number of columns in the table).
 *
 * @author Thanasis Vergoulis
 */
class HitmapTable
{
protected:
//	vector<bool>** _hits;
	bitset<HITMAP_SIZE>** _hits;
	int _hitmap_size;
	tr1::unordered_map<string,int>::hasher _hash_func;
	unsigned long _buckets_num;
public:
	/**
	 * @brief The constructor.
	 *
	 * @param hitmaps_num The number of hitmaps.
	 * @param hitmap_size The size of each hitmap.
	 *
	 * @author Thanasis Vergoulis
	 */
	HitmapTable( string& data_str, unsigned long gram_length, unsigned long q_len, unsigned long q_thr, int hitmap_size);
	
	/**
	 * @brief Set to 1 the value of a particular hit in a particular hitmap. 
	 *
	 * @param hitmap_id The row number of the corresponding hitmap.
	 * @param hit_id The column number of the corresponding hit.
	 *
	 * @author Thanasis Vergoulis
	 */
	void setHit( int hitmap_id, int hit_id);

	/**
	 * @brief Initialize a hitmap to a set of zeros.
	 *
	 * @param hitmap_id The id of the hitmap to be initialized.
	 *
	 * @author Thanasis Vergoulis
	 */
	void initHitmap( int hitmap_id );

	/**
	 * @brief Get the hid value. 
	 *
	 * @param hitmap_id The row number of the corresponding hitmap.
	 * @param hit_id The column number of the corresponding hit.
	 *
	 * @author Thanasis Vergoulis
	 */	
	int getHit( int hitmap_id, int hit_id);
	
	/**
	 * @brief Get the "merge plan", i.e., which list you are going to merge.
	 *
	 * @param hitmaps_list Array containing the the row number of each hitmap to be merged.
	 * @param hl_size The size of the hitmaps_list. 
	 * @param merge_plan Array containing for each column which rows must be merged.
	 *
	 * @author Thanasis Vergoulis
	 */
	void getMergePlan( unsigned long* hitmaps_list, int hl_size, int* merge_plan);

	/**
	 * @brief Get the regions of data sequence which are candidate for matches (based on a list of gram hitmaps).
	 *
	 * @param hitmaps_list Array containing the row number of each hitmap to be used.
	 * @param hl_size The number of hitmaps to be used (the size of hitmap_list).
	 * @param regions Hitmap containing (at the end) the list of candidate regions (1 where a candidate is).
	 * @param occ_thr The number of occurrences a region must have to become a candidate region.
	 *
	 * @authro Thanasis Vergoulis
	 */
	void getCandRegions( unsigned long* hitmaps_list, int hl_size, bitset<HITMAP_SIZE>* regions, int occ_thr);
	
	int getHitmapSize();

	/**
	 * @brief It returns the hash key of the given search string.
	 *
	 * @param query_str The search string.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long getHashKey(string query_str);
	
	
	/**
	 * @brief It prints statistics of the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void showStats(void);
};

/**
 * @brief The HitmapTableOld needed by the List Reduction method.
 *
 * @var _hits The 2-dimensional table where the hits are stored (each row is a hitmap).
 * @var _hitmaps_num The number of hitmaps (i.e., rows) of the table.
 * @var _hitmap_size The size of each hitmap (i.e., the number of columns in the table).
 *
 * @author Thanasis Vergoulis
 */
class HitmapTableOld
{
protected:
	vector<bool>* _hits;
	int _hitmaps_num;
	int _hitmap_size;
public:
	/**
	 * @brief The constructor.
	 *
	 * @param hitmaps_num The number of hitmaps.
	 * @param hitmap_size The size of each hitmap.
	 *
	 * @author Thanasis Vergoulis
	 */
	HitmapTableOld( int hitmaps_num, int hitmap_size);
	
	/**
	 * @brief Set to 1 the value of a particular hit in a particular hitmap. 
	 *
	 * @param hitmap_id The row number of the corresponding hitmap.
	 * @param hit_id The column number of the corresponding hit.
	 *
	 * @author Thanasis Vergoulis
	 */
	void setHit( int hitmap_id, int hit_id);

	/**
	 * @brief Initialize a hitmap to a set of zeros.
	 *
	 * @param hitmap_id The id of the hitmap to be initialized.
	 *
	 * @author Thanasis Vergoulis
	 */
	void initHitmap( int hitmap_id );

	/**
	 * @brief Get the hid value. 
	 *
	 * @param hitmap_id The row number of the corresponding hitmap.
	 * @param hit_id The column number of the corresponding hit.
	 *
	 * @author Thanasis Vergoulis
	 */	
	int getHit( int hitmap_id, int hit_id);
	
	/**
	 * @brief Get the "merge plan", i.e., which list you are going to merge.
	 *
	 * @param hitmaps_list Array containing the the row number of each hitmap to be merged.
	 * @param hl_size The size of the hitmaps_list. 
	 * @param merge_plan Array containing for each column which rows must be merged.
	 *
	 * @author Thanasis Vergoulis
	 */
	void getMergePlan( unsigned long* hitmaps_list, int hl_size, int* merge_plan);
	
	int getHitmapSize();
};

/**
 * @brief The String Hash with List Reduction (LR) used by Dirichlet method.
 *
 * @details The String Hash is implemented as follows: The hash table is implemented as an array of
 * pointers. Each pointer corresponds to one bucket. The number of buckets is determined during the
 * construction of the object. Each bucket is implemented as a vector of items. 
 *
 * @var _hitmap_table A pointer to the hitmap table used by LR.
 *
 * @author Thanasis Vergoulis
 */
class StrHashDirLR : public SHashDir
{
protected:
	HitmapTableOld* _hitmap_table;
public:
	/**
	 * @brief The constructor for substrings of ONE length.
	 *
	 * @param data_str The data string (derived from parent class).
	 * @param gram_length The length of the keys (derived from parent class).
	 * @param q_len The (max) query string length.
	 * @param q_thr The (max) cost threshold.
	 * @param r_num The number of (overlapping) regions in which the data_str is divided (for LR).
	 *
	 * @author Thanasis Vergoulis
	 */
	StrHashDirLR( string& data_str, unsigned long gram_length, unsigned long q_len, unsigned long q_thr, unsigned long r_num);
	
	/**
	 * @brief The destructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	~StrHashDirLR(void);
	
	/**
	 * @brief It returns an iterator to the entry which matches the query. It returns end otherwise.
	 *
	 * @param query_str The search string.
	 *
	 * @author Thanasis Vergoulis
	 */
	vector<unsigned long>* find(string query_str);
	
	/**
	 * @brief It returns the hash key of the given search string.
	 *
	 * @param query_str The search string.
	 *
	 * @author Thanasis Vergoulis
	 */
	unsigned long getHashKey(string query_str);
	
	/**
	 * @brief It executes getMergePlan for _hitmap_table.
	 *
	 * @param hitmaps_list Array containing the the row number of each hitmap to be merged.
	 * @param hl_size The size of the hitmaps_list. 
	 * @param merge_plan Array containing for each column which rows must be merged.
	 *
	 * @author Thanasis Vergoulis
	 */
	void getMergePlan( unsigned long* hitmaps_list, int hl_size, int* merge_plan);
	
	/**
	 * @brief Free the memory used by the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void clear();

	void showStats(void);
	
	int getHitmapSize();

};

/**
 * @brief Inverted list.
 *
 * @details A list containing positions on the string. These are the start positions of occurrences
 * of a specific substring. 
 *
 * @var _position The start position of the occurrence. 
 *
 * @author Thanasis Vergoulis
 */
struct IList2
{
	list<Position> _positions;
};

struct InvList
{
	vector<Position> _positions;
};

/**
 * @brief The string hash.
 *
 * @var _h_table The hash table containing the inverted lists for each substring.
 *
 * @author Thanasis Vergoulis
 */
class SHash2
{ 
//	tr1::unordered_map<string,IList2> _h_table;
//	tr1::unordered_map<string,vector<Position> > _h_table;
	vector<Position>** _h_table;
	tr1::unordered_map<string,int>::hasher _hash_func;
	unsigned long _buckets_num;
public:
	/**
	 * @brief The constructor for substrings of ONE length.
	 *
	 * @param data_str The data string.
	 * @param gram_length The length of the keys.
	 *
	 * @author Thanasis Vergoulis
	 */	
	SHash2( string& data_str, unsigned long gram_length, unsigned long window_length);
	
	SHash2( string& data_str, unsigned long gram_length, unsigned long window_length, unsigned long sl_num);
	
	/**
	 * @brief The destructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	~SHash2(void);
	
	void clear();
	
	/**
	 * @brief It returns the number of distinct keys in the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);
	
	/**
	 * @brief It returns the number bytes needed to store the hash in memory (or on the disk). 
	 *
	 * @author Thanasis Vergoulis
	 */
	int byteSize(void);
	
	/**
	 * @brief It returns ...
	 *
	 * @param query_str The search string.
	 *
	 * @author Thanasis Vergoulis
	 */
	vector<Position>* find(string query_str);
	
	/**
	 * @brief Checks if the current iterator is end. 
	 *
	 * @details If it=end it returns false, otherwise it returns true.
	 *
	 * @param it The iterator. 
	 *
	 * @author Thanasis Vergoulis
	 */
	bool exists(tr1::unordered_map<string,IList>::iterator it);
	
	/**
	 * @brief It prints the contents of the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);
	
	/**
	 * @brief It prints statistics of the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void showStats(void);
};

/**
 * @brief The string hash.
 *
 * @var _h_table The hash table containing the inverted lists for each substring.
 *
 * @author Thanasis Vergoulis
 */
class SHash3
{
	tr1::unordered_map<string,IList2> _h_table;
public:
	/**
	 * @brief The constructor for substrings of ONE length.
	 *
	 * @param data_str The data string.
	 * @param gram_length The length of the keys.
	 *
	 * @author Thanasis Vergoulis
	 */	
	SHash3( string& data_str, unsigned long gram_length, unsigned long window_length);
	
	/**
	 * @brief The destructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	~SHash3(void);
	
	/**
	 * @brief It returns the number of distinct keys in the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	int size(void);
	
	/**
	 * @brief It returns the number bytes needed to store the hash in memory (or on the disk). 
	 *
	 * @author Thanasis Vergoulis
	 */
	int byteSize(void);
	
	/**
	 * @brief It returns an iterator to the entry which matches the query. It returns end otherwise.
	 *
	 * @param query_str The search string.
	 *
	 * @author Thanasis Vergoulis
	 */
	tr1::unordered_map<string,IList2>::iterator find(string query_str);
	
	/**
	 * @brief Checks if the current iterator is end. 
	 *
	 * @details If it=end it returns false, otherwise it returns true.
	 *
	 * @param it The iterator. 
	 *
	 * @author Thanasis Vergoulis
	 */
	bool exists(tr1::unordered_map<string,IList>::iterator it);
	
	/**
	 * @brief It prints the contents of the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(void);
	
	/**
	 * @brief It prints statistics of the hash table.
	 *
	 * @author Thanasis Vergoulis
	 */
	void showStats(void);
};

/**
 * It represents a query fragment concatenation, it stores just the start positions of the fragments.
 *
 * @var _starts The start positions. 
 *
 * @author Thanasis Vergoulis
 */
class Concatenation
{
	unsigned long* _starts;
};




#endif

