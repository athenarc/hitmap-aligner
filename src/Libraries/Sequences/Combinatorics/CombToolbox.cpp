#include "CombToolbox.h"

CombToolbox::CombToolbox(string* alphabet)
{
	this->_alphabet = alphabet; //get the alphabet to be used.
	this->_alphabet_size = this->_alphabet->length(); //get the size of the alphabet
}

string* CombToolbox::getAlphabet()
{
	return this->_alphabet;
}

int CombToolbox::getAlphabetSize()
{
	return this->_alphabet_size;
}

int CombToolbox::calcErrorDistributions(int seq_len, int errors, string model)
{
	int distr_num; //The number of error distributions.
	distr_num = 0;

	if( model == "ham") //if hamming distance
	{
		for(int i=0; i<=errors; i++)
		{
			//Compute the number of combinations for i positions selected from seq_len positions.
			distr_num += binom_large( seq_len, i);
		}
		return distr_num;
	}
	else if( model == "lev") //if edit (levenstein) distance
	{
		int ins_distr, rest_distr; //buffers for intermediate results.

		for(int i=0; i<=errors; i++)
		{
			//Determine the number of insertions
			for(int ins_num=0; ins_num<=i; ins_num++)
			{
				//First find the number of different ways to distribute the insertions in the 
				//sequence. Note that there are seq_len+1 positions where an insertion can be 
				//placed. Moreover, for each 
				ins_distr = binom_large( (seq_len+1), ins_num);

				//For each of the previous distributions we must find the number of different 
				//distributions for the rest errors as replacements and deletions. We consider them 
				//together, because a deletion can be assumed as a replacement with a special 
				//symbol (outside of the alphabet).
				rest_distr = binom_large( seq_len, (i-ins_num));
				
				distr_num += (ins_distr*rest_distr);
			}
		}
		return distr_num;
	}
	else //otherwise
	{
		cout<<"ERROR: Uknown error model!"<<endl;
		return -1;
	}
}

int CombToolbox::calcErrorDistributions(string* sequence, int errors, string model)
{
	int seq_len; //The length of the sequence.
	seq_len = sequence->length();
	
	//Print information about the calculation to be done.
	cout<<"Calculation details:"<<endl;
	cout<<" * Sequence: "<<(*sequence)<<" ("<<seq_len<<" symbols)"<<endl;
	cout<<" * Threshold: "<<errors<<endl;
	cout<<" * Model: "<<model<<endl;

	return this->calcErrorDistributions(seq_len,errors,model);
}

int CombToolbox::calcVariations(int seq_len, int errors, string model)
{
	int var_num; //the number of variations
	
	var_num = this->calcErrorDistributions(seq_len,errors,model); //get the number of error distributions
	
	if( model == "ham" )
	{
		//In hamming distance, each error distribution can take alphabet_size-1 different values
		//(all the other symbols in the alphabet). Thus, to find the variations we multiply number
		//of distributions to this value.
		var_num *= (this->_alphabet_size-1);
		
		//However, as we count in distributions number the distribution of 0 errors, we have to 
		//remove the alternatives for this special case.
		var_num -= (this->_alphabet_size-2);
		return  var_num;
	}
	else if( model == "lev")
	{
		//In edit distance, each error distribution can take alphabet_size different values. We
		//assume that deletions are replacements with an extra symbol (which is not in the alphabet)
		//thus we count together deletions and replacements. For insertions, each one can take 
		//alphabet_size different values. Therefore, in any case we multiply error distributions
		//with the alphabet size.
		var_num *= this->_alphabet_size;
		
		//However, as we count in distributions number the distribution of 0 errors, we have to 
		//remove the alternatives for this special case. 
		var_num -= (this->_alphabet_size-1);
		return var_num;
	}
	else
	{
		cout<<"ERROR: Uknown error model!"<<endl;
		return -1;
	}
}

int CombToolbox::calcVariations(string* sequence, int errors, string model)
{
	return this->calcVariations(sequence->length(),errors,model);
}

bool CombToolbox::verifyAlphabet(string* sequence)
{
	//get one-by-one the symbols of the given sequence
	for(int i=0; i<sequence->length(); i++)
	{
		if( this->_alphabet->find(sequence->at(i))==string::npos)
		{
			return false; //if the symbol was not in the alphabet
		}
	}
	return true;
}

double CombToolbox::calcAllSequences(int seq_len)
{ cout<<pow(this->_alphabet_size,seq_len)<<endl;
	return pow(this->_alphabet_size,seq_len);
}