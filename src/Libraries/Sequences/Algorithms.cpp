#include "Algorithms.h"

void dynamicProgramming(string *dseq, int area_start, int area_end, string *qseq, int qthr, vector<int> *results)
{
	int *curr_column; // The column of the table which is being calucated currently.
	int *prev_column; // The previous column;
	int *swap_column; // A dummy pointer column used only for swaps of columns
	int row_num;	  // the number of rows
	int col_num;	  // the number of columns

	row_num = qseq->length() + 1;		 // get the number of rows
	col_num = area_end - area_start + 2; // get the number of columns

	// memory allocation
	curr_column = new int[row_num];
	prev_column = new int[row_num];

	// initially, prev_columns has values 0, 1, 2, ...
	// this is the column-0
	for (int i = 0; i < row_num; i++)
	{
		prev_column[i] = i;
	}
	// cout<<"qthr = "<<qthr<<endl; cout<<"qseq="<<qseq->length()<<endl;
	// start computing the rest of the columns
	for (int j = 1; j < col_num; j++)
	{
		curr_column[0] = 0; // the first row of any column is zero (semi-local alignment)

		// compute the new column
		for (int i = 1; i < row_num; i++)
		{
			if (dseq->at(area_start + j - 1) == qseq->at(i - 1)) // if match
			{
				curr_column[i] = prev_column[i - 1]; // Get the value of D[i-1,j-1] cell
			}
			else // otherwise
			{
				curr_column[i] = prev_column[i - 1];
				if (curr_column[i] > prev_column[i])
				{
					curr_column[i] = prev_column[i];
				}
				if (curr_column[i] > curr_column[i - 1])
				{
					curr_column[i] = curr_column[i - 1];
				}
				curr_column[i]++;
			}
		}
		// get the position if there is a match
		if (curr_column[row_num - 1] <= qthr)
		{
			results->push_back(area_start + j - 1);
			//			cout<<"end:"<<(area_start+j-1)<<", errors:"<<curr_column[row_num-1]<<endl; //DEBUG
			//			cout<<"dseq *"<<dseq->substr(area_start+j-1-30,30)<<"*"<<endl;
		}

		// swap columns
		swap_column = curr_column;
		curr_column = prev_column;
		prev_column = swap_column;
	}

	delete[] curr_column;
	delete[] prev_column;
}

int smallestEditDistance(string *dseq, int area_start, int area_end, string *qseq)
{
	int *curr_column; // The column of the table which is being calucated currently.
	int *prev_column; // The previous column;
	int *swap_column; // A dummy pointer column used only for swaps of columns
	int row_num;	  // the number of rows
	int col_num;	  // the number of columns
	int distance;

	// cout<<"edit distance of:"<<endl;
	// cout<<">>"<<dseq->substr(area_start,area_end-area_start+1)<<endl;
	// cout<<">>"<<(*qseq)<<endl;

	row_num = qseq->length() + 1;		 // get the number of rows
	col_num = area_end - area_start + 2; // get the number of columns

	// memory allocation
	curr_column = new int[row_num];
	prev_column = new int[row_num];

	// initially, prev_columns has values 0, 1, 2, ...
	// this is the column-0
	for (int i = 0; i < row_num; i++)
	{
		prev_column[i] = i;
	}

	// start computing the rest of the columns
	for (int j = 1; j < col_num; j++)
	{
		curr_column[0] = 0; // the first row of any column is zero (semi-local alignment)

		// compute the new column
		for (int i = 1; i < row_num; i++)
		{
			if (dseq->at(area_start + j - 1) == qseq->at(i - 1)) // if match
			{
				curr_column[i] = prev_column[i - 1]; // Get the value of D[i-1,j-1] cell
			}
			else // otherwise
			{
				curr_column[i] = prev_column[i - 1];
				if (curr_column[i] > prev_column[i])
				{
					curr_column[i] = prev_column[i];
				}
				if (curr_column[i] > curr_column[i - 1])
				{
					curr_column[i] = curr_column[i - 1];
				}
				curr_column[i]++;
			}
		}

		/*
		// get the position if there is a match
		if( curr_column[row_num-1] <= qthr )
		{
			results->push_back(area_start+j-1);
			// cout<<"end:"<<(area_start+j-1)<<", errors:"<<curr_column[row_num-1]<<endl; //DEBUG
			// cout<<"dseq *"<<dseq->substr(area_start+j-1-30,30)<<"*"<<endl;
		}
		*/

		if (j == (col_num - 1))
		{
			distance = curr_column[row_num - 1];
			break;
		}

		// swap columns
		swap_column = curr_column;
		curr_column = prev_column;
		prev_column = swap_column;
	}

	delete[] curr_column;
	delete[] prev_column;

	return distance;
}