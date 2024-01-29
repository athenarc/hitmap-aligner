#include <string>
#include <iostream>
#include <cmath>
#include "../../Math/MyMaths.h"
using namespace std;

/**
 * An instance of combinatorics toolbox for sequences.
 *
 * @var _alphabet Pointer to a string containing the alphabet used for the sequences.
 * @var _alphabet_size The number of characters inside the alhpabet. 
 *
 * @author Thanasis Vergoulis
 */
class CombToolbox
{
	string* _alphabet; 
	int _alphabet_size;
	
public:
	/**
	 * The constructor.
	 *
	 * @param alphabet Pointer to a string containing the alphabet to be used.
	 *
	 * @author Thanasis Vergoulis
	 */
	CombToolbox(string* alphabet);

	/**
	 * It returns a pointer to the alphabet.
	 *
	 * @return A pointer to the alphabet.
	 *
	 * @author Thanasis Vergoulis
	 */
	string* getAlphabet(void);
	
	/**
	 * It returns the size of the alphabet.
	 *
	 * @return The size of the alphabet.
	 *
	 * @author Thanasis Vergoulis
	 */
	int getAlphabetSize(void);

	/**
	 * It returns the number of different error distributions (i.e., distributions of errors in 
	 * different positions) for a given sequence length, error threshold and error model.
	 *
	 * @param seq_len The length of the sequence. 
	 * @param errors The threshold of errors. 
	 * @param model The error model ('ham' = hamming, 'lev' = edit distance)
	 *
	 * @return The number of different distributions.
	 *
	 * @author Thanasis Vergoulis
	 */
	int calcErrorDistributions(int seq_len, int errors, string model);
	
	/**
	 * It returns the number of different error distributions (i.e., distributions of errors in 
	 * different positions) for a given sequence, error threshold and error model.
	 *
	 * @param sequence A pointer to the sequence.
	 * @param errors The threshold of errors. 
	 * @param model The error model ('ham' = hamming, 'lev' = edit distance)
	 *
	 * @return The number of different distributions.
	 *
	 * @author Thanasis Vergoulis
	 */
	int calcErrorDistributions(string* sequence, int errors, string model);

	/**
	 * It returns the number of variations for a given sequence length, error threshold and error 
	 * model.
	 *
	 * @attention Some variations may be identical to eachother...
	 *
	 * @param seq_len The length of the sequence. 
	 * @param errors The threshold of errors. 
	 * @param model The error model ('ham' = hamming, 'lev' = edit distance)
	 *
	 * @return The number of variations.
	 *
	 * @author Thanasis Vergoulis
	 */
	int calcVariations(int seq_len, int errors, string model);

	/**
	 * It returns the number of variations for a given sequence, error threshold and error model.
	 *
	 * @attention Some variations may be identical to eachother...
	 *
	 * @param sequence A pointer to the sequence.
	 * @param errors The threshold of errors. 
	 * @param model The error model ('ham' = hamming, 'lev' = edit distance)
	 *
	 * @return The number of variations.
	 *
	 * @author Thanasis Vergoulis
	 */
	int calcVariations(string* sequence, int errors, string model);
	
	/**
	 * It returns to "variations" array all the variations of "sequence" within the error 
	 * threshold determined by "errors" based on error model "model".
	 *
	 * @param sequence A pointer to the sequence.
	 * @param errors The threshold of errors.
	 * @param model The error model ('ham' = hamming, 'lev' = edit distance)
	 * @param variation Array containing all the variations of sequence.
	 *
	 * @attention Memory allocation for "variations" must be done outside of the method.
	 *
	 * @author Thanasis Vergoulis
	 */
	void produceVariations(string* sequence, int errors, string model, string* variations);
	
	/**
	 * It returns true if all the symbols in the given sequence belongs to the alphabet.
	 *
	 * @param sequence A pointer to the sequence.
	 *
	 * @return True if all symbols of the sequence belong to the alphabet, false otherwise.
	 *
	 * @author Thanasis Vergoulis
	 */
	bool verifyAlphabet(string* sequence);
	
	/**
	 * It returns the total number of sequences of length seq_len that can be producesd based on
	 * the alphabet.
	 *
	 * @param seq_len The length of the sequence.
	 *
	 * @return The number of different sequences
	 *
	 * @author Thanasis Vergoulis
	 */
	double calcAllSequences(int seq_len);
};