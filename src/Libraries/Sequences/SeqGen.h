/************************************************************************************************//*
 * @file SeqGen.h
 *
 * This file implements a class responsible to produce several kinds of sequences (e.g., random 
 * sequences. 
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class SeqGen
{
public:
	/**
	 * @brief The constructor.
	 *
	 * @author Thanasis Vergoulis
	 */
//	SeqGen(void);
	
	/**
	 * @brief It constructs a pseudo-random sequence of the desired length. 
	 *
	 * @details It uses the uniform Bernoulli model (all symbols have probability 1/alphabet.size
	 * to occur).
	 *
	 * @param length The length of the sequence to be generated.
	 * @param alphabet The alphabet of the sequence.
	 * @param rand_str Reference to the random sequence to be generated. 
	 *
	 * @author Thanasis Vergoulis
	 */
	void run( long long length, string alphabet, string& rand_str);
	
	/**
	 * @brief It constructs a pseudo-random sequence of the desired length (version for large seqs).
	 *
	 * @details It uses the uniform Bernoulli model (all symbols have probability 1/alphabet.size
	 * to occur). This version is for large sequences. The function writes the sequence directly on
	 * disk. 
	 *
	 * @param length The length of the sequence to be generated.
	 * @param alphabet The alphabet of the sequence. 
	 * @param o_file The name of the file where the new sequence is going to be stored. 
	 *
	 * @author Thanasis Vergoulis
	 */
	void run_4large( unsigned long long length, string alphabet, string o_file);
};
