#include "MemObserver.h"

MemObserver::MemObserver()
{
	//reset counters
	this->_func_calls_num=0;
	this->_idx_reads=0;
	this->_idx_writes=0;
	this->_oth_reads=0;
	this->_oth_writes=0;
	this->_idx_probes=0;
	this->_comparisons=0;
	this->_read_bytes=0;
	this->_write_bytes=0;
	this->_accessed_items=0;
	this->_items_in_heap1=0;
	this->_items_in_heap2=0;
	this->_items_in_heap3=0;
	this->_heap_bytes=0;
	this->_win_comps=0;
	this->_cache_misses=0;
	this->_method_bytes=0;
	this->_method_bytes_1=0;
	this->_method_bytes_2=0;
	this->_method_bytes_3=0;
	this->_method_bytes_4=0;
	
	this->_overflow_flag=false;
}

MemObserver* MemObserver::_instance = NULL; //Initialize MemObserver::_instance

MemObserver* MemObserver::getInstance()
{
	if( MemObserver::_instance==NULL)
	{
		MemObserver::_instance = new MemObserver();
	}
	return MemObserver::_instance;
}

void MemObserver::funcCalls( unsigned int quantity)
{
	unsigned int new_func_calls_num;
	new_func_calls_num = this->_func_calls_num + quantity;
	if( new_func_calls_num < this->_func_calls_num )
	{
		this->_overflow_flag = true;
	}
	this->_func_calls_num = new_func_calls_num;	
}

void MemObserver::accessedItems()
{
	unsigned int new_accessed_items;
	new_accessed_items = this->_accessed_items+1;
	if( new_accessed_items < this->_accessed_items )
	{
		this->_overflow_flag = true;
	}
	this->_accessed_items = new_accessed_items;	
}

void MemObserver::winComps()
{
	unsigned int new_win_comps;
	new_win_comps = this->_win_comps+1;
	if( new_win_comps < this->_win_comps )
	{
		this->_overflow_flag = true;
	}
	this->_win_comps = new_win_comps;	
}

void MemObserver::itemsInHeap1()
{
	unsigned int new_items_in_heap;
	new_items_in_heap = this->_items_in_heap1+1;
	if( new_items_in_heap < this->_items_in_heap1 )
	{
		this->_overflow_flag = true;
	}
	this->_items_in_heap1 = new_items_in_heap;	
}

void MemObserver::itemsInHeap2()
{
	unsigned int new_items_in_heap;
	new_items_in_heap = this->_items_in_heap2+1;
	if( new_items_in_heap < this->_items_in_heap2 )
	{
		this->_overflow_flag = true;
	}
	this->_items_in_heap2 = new_items_in_heap;	
}

void MemObserver::itemsInHeap3()
{
	unsigned int new_items_in_heap;
	new_items_in_heap = this->_items_in_heap3+1;
	if( new_items_in_heap < this->_items_in_heap3 )
	{
		this->_overflow_flag = true;
	}
	this->_items_in_heap3 = new_items_in_heap;	
}

void MemObserver::heapBytes( unsigned int quantity)
{
	unsigned int new_heap_bytes;
	new_heap_bytes = this->_heap_bytes + quantity;
	if( new_heap_bytes < this->_heap_bytes )
	{
		this->_overflow_flag = true;
	}
	this->_heap_bytes = new_heap_bytes;	
}


void MemObserver::idxReads( unsigned int quantity)
{
	unsigned int new_idx_reads;
	new_idx_reads = this->_idx_reads + quantity;
	if( new_idx_reads < this->_idx_reads )
	{
		this->_overflow_flag = true;
	}
	this->_idx_reads = new_idx_reads;	
}

void MemObserver::idxWrites( unsigned int quantity)
{
	unsigned int new_idx_writes;
	new_idx_writes = this->_idx_writes + quantity;
	if( new_idx_writes < this->_idx_writes )
	{
		this->_overflow_flag = true;
	}
	this->_idx_writes = new_idx_writes;	
}

void MemObserver::othReads( unsigned int quantity)
{
	unsigned int new_oth_reads;
	new_oth_reads = this->_oth_reads + quantity;
	if( new_oth_reads < this->_oth_reads )
	{
		this->_overflow_flag = true;
	}
	this->_oth_reads = new_oth_reads;	
}

void MemObserver::othWrites( unsigned int quantity)
{
	unsigned int new_oth_writes;
	new_oth_writes = this->_oth_writes + quantity;
	if( new_oth_writes < this->_oth_writes )
	{
		this->_overflow_flag = true;
	}
	this->_oth_writes = new_oth_writes;	
}

void MemObserver::idxProbes( unsigned int quantity)
{
	unsigned int new_idx_probes;
	new_idx_probes = this->_idx_probes + quantity;
	if( new_idx_probes < this->_idx_probes )
	{
		this->_overflow_flag = true;
	}
	this->_idx_probes = new_idx_probes;	
}

void MemObserver::comparisons(unsigned int quantity)
{
	unsigned int new_comparisons;
	new_comparisons = this->_comparisons + quantity;
	if( new_comparisons < this->_comparisons )
	{
		this->_overflow_flag = true;
	}
	this->_comparisons = new_comparisons;	
}

void MemObserver::methodBytes(unsigned int size)
{
	unsigned int new_method_bytes;
	new_method_bytes = this->_method_bytes + size;
	if( new_method_bytes < this->_method_bytes )
	{
		this->_overflow_flag = true;
	}
	this->_method_bytes = new_method_bytes;
}

void MemObserver::methodBytes(unsigned int size, int counter)
{
	if( counter == 1 )
	{
		unsigned int new_method_bytes_1;
		new_method_bytes_1 = this->_method_bytes_1 + size;
		if( new_method_bytes_1 < this->_method_bytes_1 )
		{
			this->_overflow_flag = true;
		}
		this->_method_bytes_1 = new_method_bytes_1;
	}
	else if( counter == 2 )
	{
		unsigned int new_method_bytes_2;
		new_method_bytes_2 = this->_method_bytes_2 + size;
		if( new_method_bytes_2 < this->_method_bytes_2 )
		{
			this->_overflow_flag = true;
		}
		this->_method_bytes_2 = new_method_bytes_2;
	}
	else if( counter == 3 )
	{
		unsigned int new_method_bytes_3;
		new_method_bytes_3 = this->_method_bytes_3 + size;
		if( new_method_bytes_3 < this->_method_bytes_3 )
		{
			this->_overflow_flag = true;
		}
		this->_method_bytes_3 = new_method_bytes_3;
	}
	else if( counter == 4 )
	{
		unsigned int new_method_bytes_4;
		new_method_bytes_4 = this->_method_bytes_4 + size;
		if( new_method_bytes_4 < this->_method_bytes_4 )
		{
			this->_overflow_flag = true;
		}
		this->_method_bytes_4 = new_method_bytes_4;
	}
}

void MemObserver::readBytes(unsigned int size)
{
	unsigned int new_read_bytes;
	new_read_bytes = this->_read_bytes + size;
	if( new_read_bytes < this->_read_bytes )
	{
		this->_overflow_flag = true;
	}
	this->_read_bytes = new_read_bytes;
}

void MemObserver::cacheMisses(unsigned int size)
{
	unsigned int new_cache_misses;
	new_cache_misses = this->_cache_misses + size;
	if( new_cache_misses < this->_cache_misses )
	{
		this->_overflow_flag = true;
	}
	this->_cache_misses = new_cache_misses;
}

void MemObserver::writeBytes(unsigned int size)
{
	unsigned int new_write_bytes;
	new_write_bytes = this->_write_bytes + size;
	if( new_write_bytes < this->_write_bytes )
	{
		this->_overflow_flag = true;
	}
	this->_write_bytes = new_write_bytes;
}

void MemObserver::printCounters()
{
	if( this->_overflow_flag == true )
	{
		cout<<"***\tATTENTION! At least one of the counters has overflowed."<<endl;
	}
	cout<<"Function calls: "<<this->_func_calls_num<<endl;
//	cout<<"Index reads: "<<this->_idx_reads<<endl;
//	cout<<"Index writes: "<<this->_idx_writes<<endl;
	cout<<"Index probes: "<<this->_idx_probes<<endl;
	cout<<"Other reads: "<<this->_oth_reads<<endl;
	cout<<"Other writes: "<<this->_oth_writes<<endl;
	cout<<"Read bytes: "<<this->_read_bytes<<endl;
	cout<<"Written bytes: "<<this->_write_bytes<<endl;
	cout<<"Comparisons: "<<this->_comparisons<<endl;
	cout<<"Items in heap: "<<this->_accessed_items<<" "<<this->_items_in_heap1<<" "<<this->_items_in_heap2<<" "<<this->_items_in_heap3<<" ("<<(this->_accessed_items+this->_items_in_heap1+this->_items_in_heap2+this->_items_in_heap3)<<")"<<endl;
	cout<<"Heap bytes: "<<this->_heap_bytes<<" ("<<(this->_read_bytes+this->_write_bytes-this->_heap_bytes)<<")"<<endl;
	cout<<"Window computations: "<<this->_win_comps<<endl;
	cout<<"Cache misses: "<<this->_cache_misses<<endl;
	cout<<"METHOD BYTES: "<<this->_method_bytes<<"("<<this->_method_bytes_1<<","<<this->_method_bytes_2<<","<<this->_method_bytes_3<<","<<this->_method_bytes_4<<")"<<endl;
}