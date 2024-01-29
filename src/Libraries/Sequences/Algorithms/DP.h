#include <list>
#include <string>  //C++ strings
#include <cstring> //C strings
using namespace std;

/**
 * Return the position of the minimum value.
 *
 * This function returns the index of the element of an array that contains the
 * minimum value.
 *
 * @param value Array containing the values.
 * @param n The size of 'values' array.
 * @return The index of the element containing the minimum value.
 */
int min(int *values, int n);

/**
 * Returns the largest value in a given array.
 *
 * This function returns the largest value in a given array.
 *
 * @param values The given array of integers.
 * @param num_of_values The number of elements of the given array.
 *
 * @return The largest value in the array.
 */
int max(int *values, int num_of_values);

/**
 * Use dynamic programming to compute the minimum number of differences with
 * which the q_seq matches inside d_seq. This method is an auxiliary method used
 * by fredriksson's method during preprocessing.
 *
 * @param d_seq The data sequence.
 * @param q_seq The query sequence.
 * @param min_difs A initial value for the minimum number of differences.
 *
 * @return The updated minimum number of differences.
 */
int getMinDifferencesNaiveAlt(string &d_seq, char *q_seq, int min_difs);

/*
list <Match> ASMdpUkk(		string* d_seq,
			long area_start,
			long area_end,
			string* q_seq,
			int k,
			double& cells_comp); //TODO Testare oti ayto epitaxynei pragmati ta pragmata...*/