/************************************************************************************************//*
 * @file CString.h
 *
 * This file implements functions related to the generation of random sequences. 
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/
#include <iostream>
#include <string>
#include <cmath>
#include <bitset>
#include <map>
using namespace std;



/**
 * @brief Compressed string.
 *
 * @details This class reduces the space requirements for strings built on alphabets having small 
 * size. In particular, if each symbol for this alphabet can be stored in less than 1 byte, then we 
 * do so. E.g., if our alphabet is of size 4, then only 2 bits are needed. 
 *
 * @author Thanasis Vergoulis
 */
class CString
{
	bitset<8>* _compressed_str; 	//Each byte is represented as a bitset of 8 bits. The whole string is an array of bitsets.
	int _sym_size; //The number of bits required for each symbol.
	int _length; //The length of the string (in number of symbols).
	
public:
	/**
	 * @brief The constructor.
	 *
	 * @param or_str A reference to the original string.
	 * @param alphabet A reference to the alphabet.
	 *
	 * @author Thanasis Vergoulis
	 */
	CString( string& or_str, string& alphabet);
	
	/**
	 * @brief Converts the instance into a human-readable string.
	 *
	 * @param alphabet A reference to the alphabet.
	 *
	 * @return The corresponding string.
	 *
	 * @author Thanasis Vergoulis
	 */
	string to_string( string& alphabet);
	
	/**
	 * @brief Returns the specified substring of the compressed string.
	 *
	 * @param pos The start position of the substring.
	 * @param n The length of the substring (in number of symbols).
	 *
	 * @return The compressed substring.
	 *
	 * @author Thanasis Vergoulis
	 */
	CString substr( size_t pos, size_t n);
	
	/**
	 * @brief Returns the length of the compressed string (in number of symbols).
	 *
	 * @return The length. 
	 *
	 * @author Thanasis Vergoulis
	 */
	int length( void);

	/**
	 * @brief The access symbol operator.
	 *
	 * @param i The pointer to the symbol to be accessed.
	 *
	 * @author Thanasis Vergoulis
	 */
	bitset<8> operator[](int i);
};


/**
 * @brief Human-readable compressed string.
 *
 * @author Thanasis Vergoulis
 */
class HCString: public CString
{
};





