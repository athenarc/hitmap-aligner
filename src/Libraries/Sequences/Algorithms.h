#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <iostream>
#include "../Misc/Utilities.h" //itoa
#include <vector>
using namespace std;

/**
 * A struct holding statistics about verification need to be performed after a filtering process.
 *
 * @var _ver_size The size of the data areas to be verified.
 * @var _data_size The size of the data.
 * @var _ver_size_percentage _ver_size/_data_size
 * @var _ver_areas_num The number of verification areas
 *
 * @author Thanasis Vergoulis
 */
struct VerificationStatistics
{
	double _ver_size;
	double _data_size;
	double _ver_size_percentage;
	double _ver_areas_num;
	double _non_merged_areas;
	double _estimated_candidates;

	/**
	 * It displays the statistics.
	 *
	 * @param tabs_num The number of tabs printed in the front of each line.
	 *
	 * @author Thanasis Vergoulis
	 */
	void print(int tabs_num)
	{
		for (int i = 0; i < tabs_num; i++)
		{
			cout << "\t";
		}
		cout << "Verification Statistics:" << endl;
		for (int i = 0; i < tabs_num; i++)
		{
			cout << "\t";
		}
		cout << " * areas num: " << this->_ver_areas_num << " (before merging=" << this->_non_merged_areas << ")" << endl;
		for (int i = 0; i < tabs_num; i++)
		{
			cout << "\t";
		}
		cout << " * est. candidates: " << this->_estimated_candidates << endl;
		for (int i = 0; i < tabs_num; i++)
		{
			cout << "\t";
		}
		cout << " * areas size: " << this->_ver_size << " (" << (this->_ver_size_percentage * 100) << "%)" << endl;
		for (int i = 0; i < tabs_num; i++)
		{
			cout << "\t";
		}
		cout << " * data size: " << this->_data_size << endl;
	}
};

/**
 * It returns all the occurrences of qseq in a determined area of dseq with at most qthr errors (no
 * optimizations implemented).
 *
 * @param dseq A reference to the data sequence.
 * @param area_start The start of the area of interest.
 * @param area_end The end of the area of interest.
 * @param qseq A reference to the query sequence.
 * @param qthr The cost threshold.
 * @param results A reference to the vector containing the ending positions of the occurrences.
 *
 * @author Thanasis Vergoulis
 */
void dynamicProgramming(string *dseq, int area_start, int area_end, string *qseq, int qthr, vector<int> *results);

/**
 * It returns the smallest edit distance of any suffix of dseq.
 */
int smallestEditDistance(string *dseq, int area_start, int area_end, string *qseq);

#endif