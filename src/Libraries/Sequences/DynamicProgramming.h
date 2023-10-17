/************************************************************************************************/ /*
																									* @file SeqGen.h
																									*
																									* This file implements functions related to the generation of random sequences.
																									*
																									* @author Thanasis Vergoulis
																									* @version 2011.0303
																									**************************************************************************************************/
#include <iostream>
#include <string>
#include <bitset>
#include <list>
using namespace std;

#define WINDOW_SIZE 30 //=k_max_all+m_max_chain-1 //10?

/** A struct that represents a dynamic programming cell during the DP table
 *  computation and a candidate match returned by the getCandidateMatches.
 *  Analoga me to ti einai ayto poy anaparista xrisimopoioyntai kai diaforetika
 *  pedia.
 *
 * @note <greek>Edo xreiazetai prosoxi. o logos einai oti to megethos autis tis
 * struct paratirisa oti epireazei kata poly tin apodotikotita. sigkekrimena
 * mou eixan kseminei kati periexomena obsolete, ta opoia me to pou ta afairesa
 * ypirkse simantiki diafora stous xronous. Vasika prepei na kratisoyme oso pio
 * mikro ginetai to megethos aytis tis struct...</greek>
 *
 * @var _row The row of the corresponding cell.
 * @var _column The column of the corresponding cell.
 * @var _value The value of the corresponding cell.
 * @var _region The region that matches at the corresponding position.
 * @var _pred_bitmap The bitmap containing the columns of the predecessors.
 */
struct Candidate
{
	long _column;
	int _value;
	int _region;
	int _chain;
	bitset<WINDOW_SIZE> _pred_bitmap;

	// Define compare operators

	/** \brief The 'less than' compare operator.*/
	bool operator<(Candidate o)
	{
		return _column < o._column;
	}

	/** \brief The 'greater than' compare operator.*/
	bool operator>(Candidate o)
	{
		return _column > o._column;
	}

	/** \brief The 'equal to' compare operator.*/
	bool operator==(Candidate o)
	{
		return _column == o._column;
	}
};

/** This is a full version of dynamic programming, which returns also all the starting positions for
 * matches that end at the same position and have the same number of errors. Starting of matches
 * that end at the same position but have more errors are NOT returned.
 */
void dynamicProgramming(string *d_seq,
						long area_start,
						long area_end,
						string *q_seq,
						int thr,
						list<Candidate> &matches)
{
	//---Declarations---
	Candidate *cur_column;	// The current column of the DP table.
	Candidate *new_column;	// The column to be computed.
	long d_subseq_length;	// The length of data subsequence.
	long q_subseq_length;	// The length of the largest of the query's subsequences.
	long cur_pos;			// The current position in the data sequence.
	long data_seq_last_pos; // The last position of the data sequence.
	long last_act_cell;		// The last active cell of the current row (i.e. the last cell in the column with value at most k).
	long i;					// Pointer to the row of the current cell
	long j;					// Pointer to the column of the current cell
	int z, gg, g;
	char data_seq_cur_char; // The current character of the data sequence.
	int temp[3];			// Array containing D[i-1,j]+1, D[i,j-1]+1 and D[i-1,j-1] values
	long last_cell;			// The last cell of the dynamic programming table

	//---Initializations---
	d_subseq_length = area_end - area_start + 2; //(+1 cause we will add an " " character at the begining)
	q_subseq_length = q_seq->length();
	data_seq_last_pos = d_seq->length() - 1; // Get the total length of the data sequence.
	bitset<WINDOW_SIZE> my_one(1ul);		 // Mia simantiki timi bit einai i 00...01.
	bitset<WINDOW_SIZE> my_two(2ul);		 // kai mia alli simantiki timi einai i 00...10.
	last_cell = q_subseq_length - 1;

	// We compute values of the dynamic programming table column-by-column.
	// Each time we keep in main memory 2 columns. The current column
	//(cur_column) and the column to be computed (new_column).

	cur_column = new Candidate[q_subseq_length]; // Allocate the essential space for cur_column (j-1).
	new_column = new Candidate[q_subseq_length]; // Allocate the essential space for new_column (j).

	// Initialization of the current column (initially it is column 0).
	for (i = 0; i < q_subseq_length; i++)
	{
		cur_column[i]._value = i;
		cur_column[i]._pred_bitmap = my_one;
	}
	last_act_cell = thr; // The last active cell of the column-0 (see cut-off heuristic).

	// Loop that computes the cells of each new column j (j>0).
	for (j = 1; j < d_subseq_length; j++)
	{
		// Initialize the new column.
		for (g = 0; g < q_subseq_length; g++)
		{
			new_column[g]._value = thr + 1; // Each cell gets 'infinite' value (k+1).
		}
		new_column[0]._value = 0; // The zero-row of DP table is full of zeros
		new_column[0]._pred_bitmap = my_one;

		data_seq_cur_char = d_seq->at(area_start + j - 1); // Get the current character of the data sequence.

		// b. Compute the remaining cells in the new (i) column. Stop when you are at the last_active_cell+1.
		for (i = 1; i <= last_act_cell + 1 && i < q_subseq_length; i++)
		{
			//			if( data_seq_cur_char == q_seq->at(largest_start+i-1)) //If the symbols are equal
			if (data_seq_cur_char == q_seq->at(i)) // If the symbols are equal.
			{
				new_column[i]._value = cur_column[i - 1]._value; // Get the value of D[i-1,j-1] cell (where D is the DP table)
				// apla klironomei ta predecessors apo to pano
				// aristera.

				// Diaforetika tha klironomoyme apo ton apano aristera.
				new_column[i]._pred_bitmap = cur_column[i - 1]._pred_bitmap;
			}
			else // If the symbols are different
			{
				temp[0] = new_column[i - 1]._value + 1; // Get D[i-1,j]+1 value
				temp[1] = cur_column[i - 1]._value + 1; // Get D[i-1,j-1]+1 value
				temp[2] = cur_column[i]._value + 1;		// Get D[i,j-1]+1 value

				// Initially assume that D[i-1,j]+1 is smaller.
				new_column[i]._value = temp[0];
				new_column[i]._pred_bitmap = new_column[i - 1]._pred_bitmap;

				// Check if D[i-1,j-1]+1 is smaller.
				if (temp[1] < new_column[i]._value)
				{
					new_column[i]._value = temp[1];
					new_column[i]._pred_bitmap = cur_column[i - 1]._pred_bitmap;
				}
				else if (temp[1] == new_column[i]._value)
				{
					// then add the elements
					new_column[i]._pred_bitmap |= cur_column[i - 1]._pred_bitmap;
				}
				if (temp[2] < new_column[i]._value)
				{
					new_column[i]._value = temp[2];
					new_column[i]._pred_bitmap = cur_column[i]._pred_bitmap;
				}
				else if (temp[2] == new_column[i]._value)
				{
					// then add the elements
					new_column[i]._pred_bitmap |= cur_column[i]._pred_bitmap;
				}

				// TODO yparxei mia akraia periptosi, i periptosi opoy o predecessor peftei stin
				// column 0. Se ayti tin periptosi den tha prepei na kratisoyme to 0 alla to 1.
			}
		}

		// Update the value of the last active cell.
		if (last_act_cell != last_cell)
		{
			if (new_column[last_act_cell + 1]._value > thr)
			{
				//				int g;
				for (gg = last_act_cell; gg > 0; gg--)
				{
					if (new_column[gg]._value == thr)
					{
						last_act_cell = gg;
						break;
					}
				}
			}
			else
			{
				last_act_cell = last_act_cell + 1;
			}
		}
		else
		{
			//			int g;
			for (gg = last_act_cell; gg > 0; gg--)
			{
				if (new_column[gg]._value <= thr)
				{
					last_act_cell = gg;
					break;
				}
			}
		}

		// c. If any match, get it in your list of matches!
		// The last row of each column, contains possible matches.

		// TODO ola osa einai kato apo to kmax olon gia ti grammi, tha prepei na
		// epistrafoyn os candidate matches.

		// epi tis oysias einai to k poy einai sti thesi +(num_of_chains) metato kanoniko k

		if (last_act_cell == last_cell)
		{
			if (new_column[last_cell]._value <= thr)
			{
				new_column[last_cell]._column = area_start + j - 1;
				matches.push_back(new_column[last_cell]);
			}
		}

		// Shift all the bitmaps. ayto giati meta tha prepei na ypologiso gia tin epomeni stili...
		for (z = 0; z <= last_act_cell; z++)
		{
			new_column[z]._pred_bitmap = new_column[z]._pred_bitmap << 1;
		}

		// d. "Copy" the new column to the current column
		Candidate *swap_column; // An auxiliary pointer used to swap the two array pointers.

		swap_column = cur_column;
		cur_column = new_column;
		new_column = swap_column;

		// note: Instead of swaping the pointers to the arrays we could do the following:
		//
		// for( i=0; i<q_seq->length(); i++)
		//{
		//	cur_column[i] = new_column[i];
		// }
		//
		// with the same result, but consuming much more time.
	}

	// Free memory
	delete[] cur_column;
	delete[] new_column;
}