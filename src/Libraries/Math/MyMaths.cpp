/************************************************************************************************//*
 * @file MyMaths.h
 *
 * This file implements functions related to mathematics. 
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/


#include "MyMaths.h"


float factorial(float a)
{
	if (a > 1)
		return (a * factorial (a-1));
	else
		return (1);
}

double factorial(double a)
{
	if (a > 1)
		return (a * factorial (a-1));
	else
		return (1);
}

float binom(float n, float m)
{
	float num;
	float denom;
	
	num = factorial(n);
	denom = factorial(n-m) * factorial(m);
	
	return num/denom;
}

//calculate binomial coefficient for large numbers... 
unsigned int binom_large( float n, float m)
{
	if( n-m==1 ) //special case
	{
		return n;
	}
	else if(n-m==0)
	{
		return 1;
	}
	else
	{ 
		double num=1;
		double denom;
		
		if( (n-m)>m )
		{			
			for( int i=n-m+1; i<=n; i++)
			{
				num *= i;
			}
			denom = factorial(m); 
		}
		else
		{
			for( int i=m+1; i<=n; i++)
			{
				num *=i;
			}
			denom = factorial(n-m);
		}
			
		//cout<<"num="<<num<<", denom="<<denom<<", res= "<<(num/denom)<<endl;
		return (unsigned int) (num/denom);
	}
}

float getPermutationsRepNum( float n, float k)
{
	return pow(n,k);
}

void comb( char* obj_list, int num_obj, int num_sel, char* comb_list)
{
	int* pointers; //An array containing 'num_sel' pointers + 1 dummy pointer.
	int cur_pointer;
	
	pointers = new int[num_sel+1]; //Allocate space from 'pointers'.
	//Initialize pointers.
	for(int i=0; i<num_sel; i++)
	{
		pointers[i] = i;
	}
	pointers[num_sel] = num_obj; //Initialize dummy pointer.
	
	cout<<"Comb: ";
	for(int i=0; i<num_sel; i++)
	{
		cout<<pointers[i];
		//		cout<<obj_list[pointers[i]];
	}
	cout<<endl;
	
	while( pointers[num_sel-1]<pointers[num_sel])
	{
		pointers[num_sel-1]++;
		if(pointers[num_sel-1]==pointers[num_sel])
		{
			for( cur_pointer = num_sel-2; cur_pointer>=0; cur_pointer--)
			{
				pointers[cur_pointer]++;
				if( pointers[cur_pointer]!=(pointers[cur_pointer+1]-1))
				{
					for(int j=(cur_pointer+1); j<num_sel; j++)
					{
						pointers[j] = pointers[j-1]+1;
					}
					break;
				}
			}
			
		}
		if(pointers[num_sel-1]!=pointers[num_sel])
		{
			cout<<"Comb: ";
			for(int i=0; i<num_sel; i++)
			{
				cout<<pointers[i];
				//		cout<<obj_list[pointers[i]];
			}
			cout<<endl;
		}
		
	}
	
	delete [] pointers;
	
	
}

void combineIntegers( int* collection, int coll_size, int sel_size, int** combs)
{
	//DEBUG
//	cout<<"My integers:"<<endl;
//	for(int i=0; i<coll_size; i++)
//	{
//		cout<<collection[i]<<" ";
//	}
//	cout<<endl;
	
	//Get the concatenations.
	int num_sel = sel_size;
	int num_obj = coll_size;
	int cur_comb_id; //The id of the current combination.
	cur_comb_id = 0;

	
	int* pointers; //An array containing 'num_sel' pointers + 1 dummy pointer.
	int cur_pointer;
	
	pointers = new int[num_sel+1]; //Allocate space from 'pointers'.
	//Initialize pointers.
	for(int i=0; i<num_sel; i++)
	{
		pointers[i] = i;
	}
	pointers[num_sel] = num_obj; //Initialize dummy pointer.

	for(int i=0; i<num_sel; i++)
	{ 
		combs[cur_comb_id][i] = collection[pointers[i]];
//		cout<<combs[cur_comb_id][i]; //DEBUG
	}
//	cout<<endl; //DEBUG
	cur_comb_id++;

	
	while( pointers[num_sel-1]<pointers[num_sel])
	{
		pointers[num_sel-1]++;
		if(pointers[num_sel-1]==pointers[num_sel])
		{
			for( cur_pointer = num_sel-2; cur_pointer>=0; cur_pointer--)
			{
				pointers[cur_pointer]++;
				if( pointers[cur_pointer]!=(pointers[cur_pointer+1]-1))
				{
					for(int j=(cur_pointer+1); j<num_sel; j++)
					{
						pointers[j] = pointers[j-1]+1;
					}
					break;
				}
			}
			
		}
		if(pointers[num_sel-1]!=pointers[num_sel])
		{
			for(int i=0; i<num_sel; i++)
			{
				combs[cur_comb_id][i] = collection[pointers[i]];
//				cout<<combs[cur_comb_id][i]; //DEBUG
			}
//			cout<<endl; //DEBUG
			cur_comb_id++;
		}
		
	}
	
	delete [] pointers;	
}

void printCombinedIntegers( int combs_num, int sel_size, int** combs)
{
	int i,j;
	for( i=0; i<combs_num; i++)
	{
		cout<<"comb["<<i<<"]: ";
		for( j=0; j<sel_size; j++)
		{
			cout<<combs[i][j]<<" ";
		}
		cout<<endl;
	}
}

