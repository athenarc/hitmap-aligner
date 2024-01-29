#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "../Libraries/Sequences/Algorithms.h"

using namespace std;

/**
 * It is a validator that reveals if the results of another alignment method are correct. 
 *
 * @var _dseq A reference to the data sequence.
 * @var _dlen The length of the data sequence.
 * @var _qseq A reference to the query sequence.
 * @var _qlen The length of the query sequence.
 * @var _qthr The cost threshold.
 * @var _results A vector containing the starting positions of the query occurrences in the data.
 */
class Validator
{
	string* _dseq;
	int _dlen;
	string* _qseq;
	int _qlen;
	unsigned int _qthr;
	vector<int> _results;
public:
	/**
	 * The constructor of the class.
	 *
	 * @param dseq A reference to the data sequence.
	 * @param qseq A reference to the query sequence.
	 * @param qthr The cost threshold.
	 *
	 * @author Thanasis Vergoulis
	 */
	Validator(string* dseq, string* qseq, unsigned int qthr);
	
	/**
	 * The destructor of the class.
	 *
	 * @author Thanasis Vergoulis
	 */
	~Validator(void);
	
	/**
	 * It reveals if results of another alignment method are correct.
	 * 
	 * @param distance The distance function (e.g., edit, hamming).
	 * @param test_results A reference to a vector containing the results of the other al. method.
	 *
	 * @return True if test_results are correct, false otherwise.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool validate(string distance, vector<int>& test_results); 
	
	/**
	 * It displays the starting positions and the type of occurrences.
	 *
	 * @param offset The real start of the data sequence (because we may have added dummy symbols in the beginning).
	 * @param tabs The number of tabs for the display.
	 *
	 * @author Thanasis Vergoulis
	 */
	void printResults(int offset, int tabs);
};

#endif