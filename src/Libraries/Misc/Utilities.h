#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

/**
 * It returns the string that corresponds to a given number.
 *
 * @param num The given number.
 * @return The corresponding string
 *
 * @author Thanasis Vergoulis
 */
string itoa( int num );

string dtoa( double num);

string ltoa( long int num);

/**
 * It splits a string based on a given delimiter.
 *
 * @param str The given string.
 * @param delimiter The given delimiter.
 * @return A vector containing the tokens of the string. 
 *
 * @author Thanasis Vergoulis
 */
vector<string> tokenize( string str, char delimiter);

vector<string> tokenize( string str, string delimiter);

#endif
