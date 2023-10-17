/***********************************************************************************************/ /**
* @file Fredr.h
*
**************************************************************************************************/
#ifndef FREDR_H
#define FREDR_H

#include <iostream>
#include <cmath>
#include <string>
#include <tr1/unordered_map> //C++ TR1 (Technical Report 1) implementation of hash tables.
#include "DP.h"
using namespace std;

/**
 * It represents a component capable to execute the Fredriksson's algorithm.
 *
 * @var _alphabet Reference to the alphabet of the sequences.
 * @var _alphabet_size The size of the used alphabet.
 * @var _gram_len The used length of grams.
 * @var _q_seqs Array containing the query sequences.
 * @var _q_seqs_num The number of query sequences.
 * @var _d_seq Reference to the data sequence.
 * @var _q_thrs Array containing the query thresholds.
 * @var _min_q_len The minimum length among the query sequences.
 * @var _max_q_thr The maximum query error threshold.
 * @var _dict The gram dictionary.
 * @var _dict_size The size of the gram dictionary.
 * @var _mode The alignment mode.
 *
 * @author Thanasis Vergoulis
 */
class FredrAligner
{
private:
	string *_alphabet;
	int _alphabet_size;
	int _gram_len;
	string *_q_seqs;
	string *_d_seq;
	int _q_seqs_num;
	int *_q_thrs;
	int _min_q_len;
	int _max_q_thr;
	tr1::unordered_map<long long, int> *_dict;
	int _dict_size;
	string _mode;

	/**
	 * It verifies that the verification area contains a match.
	 *
	 * @param ver_start The start of the d_seq area to be verified.
	 * @param ver_end The end of the d_seq area to be verified.
	 *
	 * @author Thanasis Vergoulis
	 */
	void verify(long ver_start, long ver_end);

public:
	/**
	 * It initializes the parameters of the query sets.
	 *
	 * @param alphabet Reference to the alphabet of the sequences.
	 * @param gram_len The used length of grams.
	 *
	 * @author Thanasis Vergoulis
	 */
	FredrAligner(string *alphabet, int gram_len);

	/**
	 * It prepares the query set after a change and before the execution (based on the newly stored
	 * parameters).
	 *
	 * @author Thanasis Vergoulis
	 */
	void queryPreparation(void);

	/**
	 * It executes the Fredriksson's algorithm.
	 *
	 * @param d_seq A reference to the data sequence.
	 * @param q_seqs Array containing the query sequences.
	 * @param q_seqs_num The number of query sequences.
	 * @param q_thrs Array containing the query thresholds.
	 *
	 * @author Thanasis Vergoulis
	 */
	void align(string *d_seq, string *q_seqs, int q_seqs_num, int *q_thrs);

	/**
	 * It executes Fredriksson's algorithm on the given d_seq based on the stored parameters.
	 *
	 * @param d_seq A reference to the data sequence.
	 *
	 * @author Thanasis Vergoulis
	 */
	void align(string *d_seq);

	/**
	 * It executes Fredriksson's algorithm on the stored d_seq based on the given parameters.
	 */
	void align(string *q_seqs, int q_seqs_num, int *q_thrs);

	/**
	 * It executes Fredriksson's algorithm based on the stored parameters.
	 *
	 * @author Thanasis Vergoulis
	 */
	void align(void);
};

#endif /* FREDR*/