/************************************************************************************************//*
 * @file MyMaths.h
 *
 * This file implements functions related to mathematics. 
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/

#ifndef MYMATHS_H
#define MYMATHS_H

#include <iostream>
#include <string>
#include <cmath>
using namespace std;

/**
 * My impelementation of the factorial. 
 *
 * @param a The number of which the factorial we want to find.
 *
 * @return The factorial of the given number. 
 *
 * @author Thanasis Vergoulis
 */
float factorial(float a);

/**
 * This function returns the number of combinations of m objects chosen from n objects.
 *
 * @param n The number of objects in the initial set.
 * @param m the number of objects to be chosen.
 *
 * @return The number of combinations.
 *
 * @author Thanasis Vergoulis
 */
float binom(float n, float m);

/**
 * This function does the same as binom, but for large numbers.
 *
 * @param n The number of objects in the initial set.
 * @param m The number of objects to be chosen.
 *
 * @return The number of combinations.
 *
 * @author Thanasis Vergoulis
 */
unsigned int binom_large( float n, float m);

/**
 * The function that returns the number of permutations with repetitions allowed.
 *
 * @param n The number of items in the collection.
 * @param k The number of items to be selected from the collection.
 *
 * @return The number of permutations (with repetitions).
 *
 * @author Thanasis Vergoulis
 */
float getPermutationsRepNum( float n, float k);

/**
 * The function that produces the permutations with repetitions of k items selected by a 
 * collection of items.
 *
 * @param collection The collection of items.
 * @param n The number of items in the collection. 
 * @param k The number of items to be selected.
 * @param permutations An array containing all the permutations.
 * @param perm_num The number of permutations to be produced.
 *
 * @author Thanasis Vergoulis
 */
template <class Item>
void getPermutationsRep( Item* collection, int n, int k, Item** permutations, int perm_num)
{
	unsigned long long i,j;

	for( i=0; i<perm_num; i++)
	{
		for( j=0; j<k; j++)
		{
			permutations[i][j] = floor(i/pow(n,j)); 
			permutations[i][j] = collection[permutations[i][j]%n ];
		}
	}
}


template <class Item>
void getNextPermutationRep( Item* collection, int n, int k, unsigned long long i, Item* permutation )
{
	int j;
	
	for( j=0; j<k; j++)
	{
		permutation[j] = floor(i/pow(n,j));
		permutation[j] = collection[permutation[j]%n];
	}
}


/**
 * @brief Produce all the combinations of 'num_sel' objects selected from 'obj_list'.
 *
 * @param obj_list The list containing the set of objects.
 * @param num_obj The size of 'obj_list'.
 * @param num_sel The number of objects to be selected.
 * @param comb_list The list containing the combinations.
 *
 * @attention It prints pointers!!! (see source)
 *
 * @author Thanasis Vergoulis
 */
void comb( char* obj_list, int num_obj, int num_sel, char* comb_list);


/**
 * It produces all the combinations for 'sel_size' integers selected by a collection of 'coll_size' integers. 
 *
 * @param collection The collection of the integers.
 * @param coll_size The number of integers in the collection.
 * @param sel_size The number of integers in the selection.
 * @param combs Array containing the combinations.
 *
 * @author Thanasis Vergoulis
 */
void combineIntegers( int* collection, int coll_size, int sel_size, int** combs);

/**
 * It prints the combinations produced by 'combineIntegers'.
 *
 * @param combs_num The number of combinations.
 * @param sel_size The number of integers in the selection.
 * @param combs Array containing the combinations. 
 *
 * @author Thanasis Vergoulis
 */
void printCombinedIntegers( int combs_num, int sel_size, int** combs);




#endif