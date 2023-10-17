#ifndef MEMOBSERVER_H
#define MEMOBSERVER_H


#include <iostream>
using namespace std;

/**
 * @brief It maintains a list of counters to measure many interesting memory events. 
 * 
 * @details It is implemented as a Singleton class (see also Singleton design pattern).
 * @author Thanasis Vergoulis
 */
class MemObserver
{
	static MemObserver* _instance; 

	unsigned int _func_calls_num;
	unsigned int _idx_reads;
	unsigned int _idx_writes;
	unsigned int _oth_reads;
	unsigned int _oth_writes;
	unsigned int _idx_probes;
	unsigned int _comparisons;
	unsigned int _read_bytes;
	unsigned int _write_bytes;
	unsigned int _avg_list_size;
	unsigned int _accessed_items;
	unsigned int _items_in_heap1;
	unsigned int _items_in_heap2;
	unsigned int _items_in_heap3;
	unsigned int _heap_bytes;
	unsigned int _win_comps;
	unsigned int _cache_misses;
	
	unsigned int _method_bytes;
	unsigned int _method_bytes_1;
	unsigned int _method_bytes_2;
	unsigned int _method_bytes_3;
	unsigned int _method_bytes_4;
	
	bool _overflow_flag;

	/**
	 * @brief The constructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	MemObserver();	

public:
	/**
	 * @brief It returns a reference to the instance of this class.
	 *
	 * @author Thanasis Vergoulis
	 */	
	static MemObserver* getInstance();

	/**
	 * @brief It increases the function calls counter by 'quantity'.
	 *
	 * @param quantity The new function calls to be encountered.
	 *
	 * @author Thanasis Vergoulis
	 */	
	void funcCalls( unsigned int quantity);

	void readBytes( unsigned int size);
	
	void writeBytes( unsigned int size);

	void avgListSize(unsigned int size);

	void cacheMisses(unsigned int size);
	
	void methodBytes(unsigned int size);
	void methodBytes(unsigned int size, int counter);

	void winComps();
	void accessedItems();
	void itemsInHeap1();	
	void itemsInHeap2();
	void itemsInHeap3();
	void heapBytes(unsigned int size);
	/**
	 * @brief It increases the index reads counter by 'quantity'.
	 *
	 * @param quantity The new index reads to be encountered.
	 *
	 * @author Thanasis Vergoulis
	 */
	void idxReads( unsigned int quantity);
	
	/**
	 * @brief It increases the index writes counter by 'quantity'.
	 *
	 * @param quantity The new index writes to be encountered.
	 *
	 * @author Thanasis Vergoulis
	 */
	void idxWrites( unsigned int quantity);
	
	/**
	 * @brief It increases the other reads counter by 'quantity'.
	 *
	 * @param quantity The new other reads to be encountered.
	 *
	 * @author Thanasis Vergoulis
	 */
	void othReads( unsigned int quantity);
	
	/**
	 * @brief It increases the other writes counter by 'quantity'.
	 *
	 * @param quantity The new other writes to be encountered.
	 *
	 * @author Thanasis Vergoulis
	 */
	void othWrites( unsigned int quantity);
	
	/**
	 * @brief It increases the index probes counter by 'quantity'.
	 *
	 * @param quantity The new index probes to be encountered.
	 *
	 * @author Thanasis Vergoulis
	 */
	void idxProbes( unsigned int quantity);
	
	void comparisons( unsigned int quantity);
	
	/**
	 * @brief It prints all the counters.
	 *
	 * @author Thanasis Vergoulis
	 */
	void printCounters();
};

#endif