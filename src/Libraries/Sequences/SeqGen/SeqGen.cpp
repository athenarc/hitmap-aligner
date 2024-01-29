#include "SeqGen.h"

SeqGen::SeqGen(void)
{
	srand( time(NULL));	//Initialize random seed
}

void SeqGen::createSeq( unsigned int length, string alphabet, string* res_seq)
{
	int random_num;	//This variable contains a pseudo-random number. 
	int sigma; //The length of the alphabet.
	int i;
	 
	sigma = alphabet.length(); //Get the length of the alphabet.
	
	for(i=0; i<length; i++)
	{		
		random_num = rand() % sigma;

		res_seq->at(i) = alphabet[random_num];
	}
}
