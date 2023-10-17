#ifndef MISC_H
#define MISC_H

#include <algorithm>

#define MAX_UINT 4294967295





struct QFragment
{
	string _seq;
	unsigned int _type;
	unsigned int _int_pos;
	unsigned int _row;
};


struct ListItem
{
	unsigned int _position;
	unsigned int _list_id;
	
	bool operator<(ListItem o) const
	{
		return _position < o._position;
	}
	
	bool operator>(ListItem o) const
	{
		return _position > o._position;
	}
	
	bool operator==(ListItem o) const
	{
		return _position == o._position;
	}

};


/**
 * @brief A subsequence of a sequence.
 *
 * @var _begin The beginning position in the sequence.
 * @var _end The ending position in the sequence.
 *
 * @author Thanasis Vergoulis
 */
struct Subsequence
{
	unsigned int _begin;
	unsigned int _end;
};

/**
 * @brief A window found after the filtering of the first step.
 *
 * @var _subseq The subsequence which corresponds to the window.
 * @var _counter How many fragments of the window found during filtering.
 * @var _matching_frag The id of the matching fragment (used if _counter=1).
 * @var _new_fragments Array containing the fragments of window leftovers (new size) to be probed. 
 */
struct FoundWindowStepA
{
	Subsequence _subseq;
	unsigned int _counter;
	unsigned int _matching_frag;
	string* _new_fragments;
	unsigned int* _counters;
};

#endif
