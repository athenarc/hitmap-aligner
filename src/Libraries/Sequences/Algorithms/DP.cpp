#include "DP.h"

int min(int *values,
		int n)
{
	int min = 0;
	int i;
	for (i = 1; i < n; i++)
	{
		if (values[min] > values[i])
			min = i;
	}
	return min;
}

int max(int *values, int num_of_values)
{
	int max_value;		   // The largest value of array 'values'.
	max_value = values[0]; // Initialize the largest value.

	int i;
	for (i = 1; i < num_of_values; i++)
	{
		if (values[i] > max_value)
		{
			max_value = values[i];
		}
	}

	return max_value;
}

/*
list <Match> ASMdpUkk(	string* d_seq,
			long area_start,
			long area_end,
			string* q_seq,
			int k,
			double& cells_comp)
{
	list <Match> matches;	//A list containing the matches

	int* cur_column; //The current column of the DP table
	int* new_column; //The column to be computed

	long last_act_cell; //The last active cell of the current row (i.e. the last cell in the column with value at most k).

	long q_seq_length = q_seq->length(); //We will use it too much, so it is wise (and efficient) to store it in a local variable.

	#ifdef DP_DEBUG
	cout<<"ASMdp started..."<<endl;
	#endif

	cur_column = new int[q_seq_length];	//Allocate the essential space for cur_column
	new_column = new int[q_seq_length];	//Allocate the essential space for new_column

	long area_length = area_end-area_start+1; //The length of the area of the data sequence that must be checked for matches.

	long i;	//Pointer to the row of the current cell
	long j;	//Pointer to the column of the current cell

	//1. Initialization of j-column (initially j is 0)
	for( i=0; i<q_seq_length; i++)
	{
		cur_column[i] = i;
	}
	last_act_cell = k; //The last active cell of column-0 is k (the k-th row of this column is the last row with value k).

	//Initialize the new column.
	int g;
	for( g=0; g<q_seq_length; g++)
	{
		new_column[g]=k+1; //Each cell takes 'infinite' value (k+1).
	}

	char data_seq_cur_char;

	//2. Compute all the remaining columns.
	for( j=1; j<=area_length; j++)
	{

		new_column[0] = 0;	//The zero-row of DP table is full of zeros

		data_seq_cur_char = d_seq->at(area_start+j-1); //Get the current character of the data sequence.

		//b. Compute the remaining cells in the new (i) column. Stop when you are at the last_active_cell+1.
		for( i=1; i<=last_act_cell+1 && i<q_seq_length; i++)
		{
			cells_comp=cells_comp+0.000001;	//A new cell to be computed (measure millions of cells)

			if( data_seq_cur_char == q_seq->at(i)) //If the symbols are equal.
			{
				new_column[i] = cur_column[i-1];	//Get the value of D[i-1,j-1] cell (where D is the DP table)
			}
			else	//If the symbols are different
			{
				int temp[3];	//Array containing D[i-1,j]+1, D[i,j-1]+1 and D[i-1,j-1] values
				temp[0] = new_column[i-1] + 1;	//Get D[i-1,j]+1 value
				temp[1] = cur_column[i-1] + 1;	//Get D[i-1,j-1]+1 value
				temp[2] = cur_column[i] + 1;	//Get D[i,j-1]+1 value
				int minimum = min(temp, 3);	//Get the pointer to the minimum of the values

				//NOTE: To 3 os orisma mallon den xreiazetai...

				new_column[i] = temp[minimum];	//Get the value of D[i,j]
			}
		}

		//Update the value of the last active cell.
		cells_comp=cells_comp+0.000001;
		if( last_act_cell != q_seq_length-1)
		{
			if( new_column[last_act_cell+1] > k)
			{
				int g;
				for( g=last_act_cell; g>0; g--)
				{
					if(new_column[g]==k)
					{
						last_act_cell = g;
						break;
					}
				}
			}
			else
			{
				last_act_cell = last_act_cell+1;
			}
		}
		else
		{
			//If you are at the last cell of the table, then the
			//next active cell may be higher. Thus, you must check
			//this.
			int g;
			for( g=last_act_cell; g>0; g--)
			{
				if(new_column[g]<=k)
				{
					last_act_cell = g;
					break;
				}
			}
		}



		//c. If any match, get it in your list of matches!

		//NOTE: If the last cell of the new j-column has value <= than
		//k, then j is the end of a k-approximate occurrence of
		//query seq in the data seq. The matching occurrence begins at
		//the corresponding _position and ends at j.

		//NOTE: We also check whereas the last active cell is the last
		//cell in the column. If not then the value of the last cell is
		//not considered because it is a remaining from previous columns
		//(we do a trick to avoid copying columns, and this trick is
		//responsible for this...).

		if( (last_act_cell==q_seq_length-1) && (new_column[q_seq_length-1] <= k))
		{
			Match new_match; //A new match.
			new_match._position = area_start+j-1; //Get the end position of the match.
			new_match._errors = new_column[q_seq_length-1]; //Get the number of errors of the match.

			matches.push_back(new_match);	//Get the new match in the list.

		}

		//d. "Copy" the new column to the current column
		int* swap_column; //An auxiliary pointer used to swap the two array pointers.
		swap_column = cur_column;
		cur_column = new_column;
		new_column = swap_column;

		//note: Instead of swaping the pointers to the arrays we could do the following:
		//
		//for( i=0; i<q_seq_length; i++)
		//{
		//	cur_column[i] = new_column[i];
		//}
		//
		//with the same result, but consuming much more time.


	}


	#ifdef DP_DEBUG
	cout<<"ASMdp ended..."<<endl;
	#endif

	//Free memory
	delete [] cur_column;
	delete [] new_column;

	//3. Return all the resulting matches
	return matches;
}
*/

int getMinDifferencesNaiveAlt(string &d_seq,
							  char *q_seq,
							  int min_difs)
{
	int *cur_column; // The current column of the DP table
	int *new_column; // The column to be computed

	long q_seq_length = strlen(q_seq); // q_seq->length(); //We will use it too much, so it is wise (and efficient) to store it in a local variable.
	long d_seq_length = d_seq.length();

	//	cout<<"'"<<q_seq<<"'"<<endl;

	cur_column = new int[q_seq_length]; // Allocate the essential space for cur_column
	new_column = new int[q_seq_length]; // Allocate the essential space for new_column

	long i; // Pointer to the row of the current cell
	long j; // Pointer to the column of the current cell

	// 1. Initialization of j-column (initially j is 0)
	for (i = 0; i < q_seq_length; i++)
	{
		cur_column[i] = i;
	}

	// 2. Compute all the remaining columns
	for (j = 1; j < d_seq_length; j++)
	{
		// a. Initialize the zero-row-cell of the new (i) column
		new_column[0] = 0; // The zero-row of DP table is full of zeros

		// b. Compute the remaining cells in the new (i) column
		for (i = 1; i < q_seq_length; i++)
		{
			if (d_seq.at(j) == q_seq[i]) // If the symbols are equal
			{
				new_column[i] = cur_column[i - 1]; // Get the value of D[i-1,j-1] cell (where D is the DP table)
			}
			else // If the symbols are different
			{
				int temp[3];					 // Array containing D[i-1,j]+1, D[i,j-1]+1 and D[i-1,j-1] values
				temp[0] = new_column[i - 1] + 1; // Get D[i-1,j]+1 value
				temp[1] = cur_column[i - 1] + 1; // Get D[i-1,j-1]+1 value
				temp[2] = cur_column[i] + 1;	 // Get D[i,j-1]+1 value
				int minimum = min(temp, 3);		 // Get the pointer to the minimum of the values

				// NOTE: To 3 os orisma mallon den xreiazetai...

				new_column[i] = temp[minimum]; // Get the value of D[i,j]
			}
		}

		// c. If any match, get it in your list of matches!

		// NOTE: If the last cell of the new j-column has value <= than
		// k, then j is the end of a k-approximate occurrence of
		// query seq in the data seq. The matching occurrence begins at
		// the corresponding _position and ends at j.

		if (new_column[q_seq_length - 1] <= min_difs)
		{
			min_difs = new_column[q_seq_length - 1]; // Now we must find something even smaller.
		}

		// d. "Copy" the new column to the current column
		int *swap_column; // An auxiliary pointer used to swap the two array pointers.
		swap_column = cur_column;
		cur_column = new_column;
		new_column = swap_column;

		// note: Instead of swaping the pointers to the arrays we could do the following:
		//
		// for( i=0; i<q_seq_length; i++)
		//{
		//	cur_column[i] = new_column[i];
		// }
		//
		// with the same result, but consuming much more time.
	}

	// Free memory
	delete[] cur_column;
	delete[] new_column;

	return min_difs;
}