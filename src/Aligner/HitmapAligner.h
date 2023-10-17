#ifndef HITMAP_ALIGNER_H
#define HITMAP_ALIGNER_H

#include <iostream>
#include "../Libraries/Sequences/SHash/HitmapHash.h"
using namespace std;

/**
 * Sequence aligner based on Hitamp algorithm.
 *
 * @var _ab The alphabet of the sequences.
 * @var _dseq A reference to the data sequence.
 * @var _qlen The length of the query sequence.
 * @var _qthr The threshold of the query sequence.
 * @var _flen The length of the query fragments.
 * @var _fnum The number of query fragments.
 * @var _part_len The length of each part of the partition.
 * @var _part_shift The number of symbols for which the i-th part of the j-th partition is shifted in comparison to the i-th part of the (j-1)-th partition.
 * @var _block_size The size of the blocks for the sparse bitsets.
 * @var _buckets_num The number of buckets in the hash table.
 * @var _wins A vector containing all the windows that must be verified.
 * @var _results A vector containing all the approx. occurrences of the query in the data.
 * @var _hitmap A reference to the hash table of the hitmap.
 * 
 * @author Thanasis Vergoulis
 */
class HitmapAligner
{
	string _ab;
	string* _dseq;
	unsigned long _qlen;
	unsigned long _qthr;
	unsigned long _flen;
	unsigned long _fnum;
//	unsigned long _part_len;
//	unsigned long _part_shift;
	unsigned long _block_size;
	unsigned long _buckets_num;
	vector<int> _wins;
	vector<int> _results;
	int _errno;
	//
	hmp::HitmapHash* _hitmap;
public:
	/**
	 * The constructor of the class.
	 *
	 * @var qlen The length of the query sequences.
	 * @var qthr The cost threshold.
	 * @var fnum The number of fragments.
	 * @var part_len The length of each part of the partition.
	 * @var part_shift The number of symbols for which the i-th part of the j-th partition is shifted in comparison to the i-th part of the (j-1)-th partition.
	 * @var block_size The size of the blocks of the sparse bitsets. 
	 * @var buckets_num The number of buckets. 
	 * @var ab The alphabet of the sequences. 
	 *
	 * @author Thanasis Vergoulis
	 */
	HitmapAligner( unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string ab);
	
	HitmapAligner( void );
	
	~HitmapAligner();

	/**
	 * Creates the Hitmap index (using stored parameters).
	 *
	 * @param dseq Reference to the data sequence.
	 * @param mode Mode (silent/verbose).
	 * 
	 * @author Thanasis Vergoulis
	 */
	void createIdx( string& dseq, string mode); 
	
	void createIdx( string& dseq, unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string mode);

	/**
	 * Store the HITMAP idx in a file. 
	 *
	 * @param output_file The file where the idx is going to be stored. 
	 * @param ab The string alphabet. 
	 * @param constr_time The time required to construct the idx. 
	 *
	 * @author Thanasis Vergoulis
	 */
	void storeIdx( string output_file, string ab, double constr_time );
	
	/**
	 * Load the HITMAP idx from a file. 
	 *
	 * @param input_file The idx file. 
	 * @param dseq Pointer to the data sequence string.
	 * @param qlen The length of query sequences. 
	 *
	 * @author Thanasis Vergoulis
	 */
	void loadIdx( string input_file, string* dseq, unsigned long qlen);

	/**
	 * It aligns the query to the data based on Hitmap and using hamming distance threshold.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis 
	 */
	void alignHamDistance( string& qseq);

	/**
	 * It aligns the query to the data based on Hitmap and using edit distance threshold.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis 
	 */
	void alignEditDistance( string& qseq);

	void filtering(string& qseq);


	VerificationStatistics alignEditDistanceVerbose( string& qseq);

	/**
	 * It finds the windows of the data sequence that need to be verified (for both hamming and 
	 * edit distance queries.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis
	 */
	void findWindows( string& qseq);

	void keepUniqueWindows(void);

	/**
	 * It finds the approx. occurrences of the query sequence in the data sequence.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis
	 */
	void findResultsHamDistance( string& qseq);

	/**
	 * It finds the approx. occurrences of the query sequence in the data sequence.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis
	 */
	void findResultsEditDistance( string& qseq);

	/**
	 * It returns a vector containing the found verification windows. 
	 *
	 * @return The vector containing the found verification windows.
	 *
	 * @author Thanasis Vergoulis
	 */
	vector<int> getWindows();
	
	/**
	 * It returns a vector containing the found approx. occurrences of query in the data. 
	 *
	 * @return The vector containing the found approx. occurrences.
	 *
	 * @author Thanasis Vergoulis
	 */
	vector<int> getResults();
	
	/**
	 * Closes the Hitmap Aligner.
	 *
	 * @author Thanasis Vergoulis
	 */
	void close();
};


class HitmapLightAligner
{
	string _ab;
	string* _dseq;
	unsigned long _qlen;
	unsigned long _qthr;
	unsigned long _flen;
	unsigned long _fnum;
	unsigned long _block_size;
	unsigned long _buckets_num;
	vector<int> _wins;
	vector<int> _results;
	int _errno;
	//
	hmp::HitmapLightHash* _hitmap;
public:
	/**
	 * The constructor of the class.
	 *
	 * @var qlen The length of the query sequences.
	 * @var qthr The cost threshold.
	 * @var fnum The number of fragments.
	 * @var ab The alphabet of the sequences. 
	 *
	 * @author Thanasis Vergoulis
	 */
	HitmapLightAligner( unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string ab);
	
	HitmapLightAligner( void );
	
	~HitmapLightAligner();

	/**
	 * Creates the Hitmap index (using stored parameters).
	 *
	 * @param dseq Reference to the data sequence.
	 * @param mode Mode (silent/verbose).
	 * 
	 * @author Thanasis Vergoulis
	 */
	void createIdx( string& dseq, string mode); 
	
	void createIdx( string& dseq, unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string mode);

	/**
	 * Store the HITMAP idx in a file. 
	 *
	 * @param output_file The file where the idx is going to be stored. 
	 * @param ab The string alphabet. 
	 * @param constr_time The time required to construct the idx. 
	 *
	 * @author Thanasis Vergoulis
	 */
	void storeIdx( string output_file, string ab, double constr_time );
	
	/**
	 * Load the HITMAP idx from a file. 
	 *
	 * @param input_file The idx file. 
	 * @param dseq Pointer to the data sequence string.
	 * @param qlen The length of query sequences. 
	 *
	 * @author Thanasis Vergoulis
	 */
	void loadIdx( string input_file, string* dseq, unsigned long qlen);

	/**
	 * It aligns the query to the data based on Hitmap and using edit distance threshold.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis 
	 */
	void alignEditDistance( string& qseq);

	VerificationStatistics alignEditDistanceVerbose( string& qseq);

	/**
	 * It finds the windows of the data sequence that need to be verified (for both hamming and 
	 * edit distance queries.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis
	 */
	void findWindows( string& qseq);

	void keepUniqueWindows(void);

	/**
	 * It finds the approx. occurrences of the query sequence in the data sequence.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis
	 */
	void findResultsHamDistance( string& qseq);

	/**
	 * It finds the approx. occurrences of the query sequence in the data sequence.
	 *
	 * @param qseq A reference to the query sequence.
	 *
	 * @author Thanasis Vergoulis
	 */
	void findResultsEditDistance( string& qseq);
	
	/**
	 * It returns a vector containing the found verification windows. 
	 *
	 * @return The vector containing the found verification windows.
	 *
	 * @author Thanasis Vergoulis
	 */
	vector<int> getWindows();
	
	/**
	 * It returns a vector containing the found approx. occurrences of query in the data. 
	 *
	 * @return The vector containing the found approx. occurrences.
	 *
	 * @author Thanasis Vergoulis
	 */
	vector<int> getResults();
	
	/**
	 * Closes the Hitmap Aligner.
	 *
	 * @author Thansis Vergoulis
	 */
	void close();
};
#endif
