/**************************************************************************************//*
 * @file SeqGen.h
 *
 * This file implements a class responsible to produce several kinds of sequences (e.g., 
 * random sequences. 
 *
 * @author Thanasis Vergoulis
 ****************************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

class SeqGen
{
public:
	/**
	 * @brief The constructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	SeqGen(void);

	/**
	 * @brief It creates a random sequence.
	 *
	 * @param length The length of the random sequence to be created.
	 * @param alphabet The alphabet on which the sequence is created.
	 * @param res_seq Reference to the result sequence.
	 *
	 * @author Thanasis Vergoulis
	 */	
	void createSeq( unsigned int length, string alphabet, string* res_seq);
};
