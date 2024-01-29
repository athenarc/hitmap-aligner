#include "SparseBitset.h"

bool SparseBitset::_random_init = false; 
int SparseBitset::_size;
int SparseBitset::_block_size;
int SparseBitset::_blocks_num;
int SparseBitset::_last_bit_id;

SparseBitset::SparseBitset(int size, int block_size)
{  
	SparseBitset::_size = size;
	SparseBitset::_block_size = block_size;
	SparseBitset::_blocks_num = ceil((double)SparseBitset::_size/(double)SparseBitset::_block_size);
	SparseBitset::_last_bit_id = (SparseBitset::_size%SparseBitset::_block_size)-1;
	this->_block_ptrs = new dynamic_bitset<>*[this->_blocks_num];
	for( int i=0; i<this->_blocks_num; i++)
	{
		this->_block_ptrs[i] = NULL; //initially all blocks are assumed to contain zeros
	}
}

SparseBitset::SparseBitset(const SparseBitset& o)
{
/*	this->_size = o._size;
	this->_block_size = o._block_size;
	this->_blocks_num = o._blocks_num;*/
	this->_block_ptrs = new dynamic_bitset<>*[SparseBitset::_blocks_num];
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if(o._block_ptrs[i]!=NULL )
		{
			this->_block_ptrs[i] = new dynamic_bitset<>((*o._block_ptrs[i]));
		}
		else
		{
			this->_block_ptrs[i] = NULL;
		}
	}
}

SparseBitset::SparseBitset(const SparseBitset* o)
{
/*	this->_size = o->_size;
	this->_block_size = o->_block_size;
	this->_blocks_num = o->_blocks_num;*/
	this->_block_ptrs = new dynamic_bitset<>*[SparseBitset::_blocks_num];
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if(o->_block_ptrs[i]==NULL)
			this->_block_ptrs[i]=NULL;
		else
			this->_block_ptrs[i]= new dynamic_bitset<>((*o->_block_ptrs[i]));
	}
}

SparseBitset::~SparseBitset()
{ 
	for( int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i] != NULL )
		{
			delete this->_block_ptrs[i];
		}
	}
	delete [] this->_block_ptrs;
}

void SparseBitset::shiftLSB(int shift_by)
{
	bool car; //the 'carriage' to the next during shifting
	car = false; 
	int i,j;
	int rightmost_non_empty_block;
	rightmost_non_empty_block = 0;
	int new_rightmost = SparseBitset::_blocks_num-1;

	if( shift_by>0)
	{
		for(j=1; j<=shift_by; j++)
		{
			for( i=(SparseBitset::_blocks_num-1); i>=rightmost_non_empty_block; i--)
			{
				if( this->_block_ptrs[i]==NULL)
				{
					if( car )
					{
						car = false;
						this->_block_ptrs[i] = new dynamic_bitset<>(SparseBitset::_block_size);
						this->_block_ptrs[i]->set(SparseBitset::_block_size-1);	
						new_rightmost = i;					
					}
					else
					{ 
						//do nothing
						continue;
					}
				}
				else
				{
					if( this->_block_ptrs[i]->test(0) )
					{ 
						car = true;
					}
					else
					{
						car = false;
					}
					(*this->_block_ptrs[i])>>=1;	
					if(this->_block_ptrs[i]->none())
					{
						delete this->_block_ptrs[i];
						this->_block_ptrs[i]=NULL;
					}	
					else
					{
						new_rightmost = i;
					}
				}
			}
			rightmost_non_empty_block = new_rightmost;
		}
	}
}

void SparseBitset::shiftMSB(int shift_by)
{
	//TODO!!!!!!
}

double SparseBitset::getEmptyBlocksNum()
{
	double empty_blocks_num;
	empty_blocks_num = 0;
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			empty_blocks_num++;
		}
	}
	return empty_blocks_num;
}

double SparseBitset::getUnaryBlocksNum()
{
	double blocks_num;
	blocks_num = 0;
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL)
		{
			if(this->_block_ptrs[i]->count()==1)
			{
				blocks_num++;
			}
		}
	}
	return blocks_num;
			
}

void SparseBitset::set(int bit_id)
{
	int block_id; 
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitset::_block_size);
	block_bit_id = bit_id%SparseBitset::_block_size;
	
	if( this->_block_ptrs[block_id] == NULL )
		this->_block_ptrs[block_id] = new dynamic_bitset<>(SparseBitset::_block_size);
	
	this->_block_ptrs[block_id]->set(block_bit_id);
}

void SparseBitset::flipNonEmptyBlock(int bit_id)
{
	int block_id; 
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitset::_block_size);
	block_bit_id = bit_id%SparseBitset::_block_size;
	
	this->_block_ptrs[block_id]->flip(block_bit_id);
}

void SparseBitset::flipNonEmptyBlock(int block_id, int block_bit_id)
{
	this->_block_ptrs[block_id]->flip(block_bit_id);
}

bool SparseBitset::runNoneInBlock(int block_id)
{
	return this->_block_ptrs[block_id]->none();
}

bool SparseBitset::test(int bit_id)
{
	int block_id;
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitset::_block_size);
	block_bit_id = bit_id%SparseBitset::_block_size;
	
	if( this->_block_ptrs[block_id] == NULL )
		return false;
//	else if( this->_block_ptrs[block_id]->none())
//		return false;
	else
		return this->_block_ptrs[block_id]->test(block_bit_id);
}

bool SparseBitset::testInNonEmptyBlock(int bit_id)
{
	int block_id;
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)this->_block_size);
	block_bit_id = bit_id%this->_block_size;
	
	return this->_block_ptrs[block_id]->test(block_bit_id);
}

bool SparseBitset::testInNonEmptyBlock(int block_id, int block_bit_id)
{
	return this->_block_ptrs[block_id]->test(block_bit_id);
}

bool SparseBitset::testBlock(int block_id)
{
	if(this->_block_ptrs[block_id]==NULL )
		return false;
	else 
		return true;
}

void SparseBitset::print(void)
{ 
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			for(int j=0; j<SparseBitset::_block_size; j++)
			{
				cout<<"0";
			}
			cout<<" ";
		}
		else
		{
			cout<<(*this->_block_ptrs[i])<<" ";
		}
	}
	cout<<endl;
}

string SparseBitset::serialize(void)
{
	string bitset_str; string tmp_str;
	bitset_str = "";
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			for(int j=0; j<SparseBitset::_block_size; j++)
			{
				bitset_str += "0";
			}
		}
		else
		{
			to_string((*this->_block_ptrs[i]),tmp_str);
			bitset_str += tmp_str;
		}
	}
	return bitset_str;
}

string SparseBitset::getOnesListDelim(void)
{
	string bitset_str; string tmp_str;
	bitset_str = "";
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL)
		{
			for(int j=0; j<SparseBitset::_block_size; j++)
			{
				if( this->_block_ptrs[i]->test(j) )
					bitset_str += itoa(i*SparseBitset::_block_size+j)+",";
			}
		}
	}
	bitset_str = bitset_str.substr(0,bitset_str.length()-1);
	return bitset_str;
}

int SparseBitset::count(void)
{
	int set_bits;
	set_bits = 0;
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if(this->_block_ptrs[i]!=NULL)
		{
			set_bits += this->_block_ptrs[i]->count();
		}
	}
	return set_bits;
}

dynamic_bitset<> SparseBitset::getBitset(void)
{
	dynamic_bitset<> bitset(SparseBitset::_size);
	for(int i=0; i<SparseBitset::_size; i++)
	{
		if( this->test(i) )
		{
			bitset.set(i);
		}
	}
	return bitset;
}

void SparseBitset::clear(void)
{
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL )
		{
			delete this->_block_ptrs[i];
			this->_block_ptrs[i] = NULL;
		}
	}
}

void SparseBitset::reset(void)
{ 
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{ 
		if( this->_block_ptrs[i]!=NULL )
		{ 
			delete this->_block_ptrs[i];
			this->_block_ptrs[i] = NULL;
		}
	}
}

void SparseBitset::set(dynamic_bitset<> bitset)
{
	this->clear();
	for(int i=0; i<SparseBitset::_size; i++)
	{
		if( bitset.test(i) )
		{
			this->set(i);
		}
	}
}

void SparseBitset::set(dynamic_bitset<>* bitset_ptr)
{ 
	this->clear();
	for(int i=0; i<SparseBitset::_size; i++)
	{
		if( bitset_ptr->test(i) )
		{
			this->set(i);
		}
	}
}

void SparseBitset::setEfficient(dynamic_bitset<>* bitset_ptr)
{
	this->clear();
	for(int i=0; i<SparseBitset::_size; i++)
	{
		if( bitset_ptr->test(i) )
		{
			this->set(i);
			bitset_ptr->flip(i);
			if( bitset_ptr->none() )
				break;
		}
	}
}

void SparseBitset::set(void)
{
	for(int i=0; i<SparseBitset::_blocks_num; i++)
	{
		if( this->_block_ptrs[i] == NULL )
		{
			this->_block_ptrs[i] = new dynamic_bitset<>(SparseBitset::_block_size);
		}
		this->_block_ptrs[i]->set();
	}
}

bool SparseBitset::empty(void)
{
	for(int i=0; i<this->_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL )
		{
			return false;
		}
	}
	return true; 
}

int SparseBitset::size(void)
{
	return SparseBitset::_size;
}

void SparseBitset::randomSet(int bit_num)
{
	int bit_id; //The id of the bit to be set. 
	
	SparseBitset::randomInit();
	
	for(int i=0; i<bit_num; i++)
	{
		bit_id = rand() % SparseBitset::_size;
		this->set(bit_id);
	}
}

int SparseBitset::getBlocksNum(void)
{
	return SparseBitset::_blocks_num;
}

int SparseBitset::getBlockSize(void)
{
	return SparseBitset::_block_size;
} 

//=========================================================================================

bool SparseBitsetU::_random_init = false; 
int SparseBitsetU::_size;
int SparseBitsetU::_block_size;
int SparseBitsetU::_blocks_num;
int SparseBitsetU::_last_bit_id;
dynamic_bitset<>* SparseBitsetU::_unit_vector_blocks_pool = NULL;
map<dynamic_bitset<>,int> SparseBitsetU::_blocks_pool;

SparseBitsetU::SparseBitsetU(int size, int block_size)
{ 
	SparseBitsetU::_size = size;
	SparseBitsetU::_block_size = block_size;
	SparseBitsetU::_blocks_num = ceil((double)SparseBitsetU::_size/(double)SparseBitsetU::_block_size);
	SparseBitsetU::_last_bit_id = (SparseBitsetU::_size%SparseBitsetU::_block_size)-1;
	this->_block_ptrs = new dynamic_bitset<>*[this->_blocks_num];
	for( int i=0; i<this->_blocks_num; i++)
	{
		this->_block_ptrs[i] = NULL; //initially all blocks are assumed to contain zeros
	}
	
	if( SparseBitsetU::_unit_vector_blocks_pool == NULL )
	{ 
		SparseBitsetU::_unit_vector_blocks_pool = new dynamic_bitset<>[SparseBitsetU::_block_size]; //create the pool (all bitsets are empty)

		for(int i=0; i<SparseBitsetU::_block_size; i++)
		{ 
			SparseBitsetU::_unit_vector_blocks_pool[i].resize(SparseBitsetU::_block_size); //resize each bitset
			SparseBitsetU::_unit_vector_blocks_pool[i].set(i); //set the corresponding bit to for the unit vector 
		}
	}
}


SparseBitsetU::SparseBitsetU(const SparseBitsetU& o)
{
/*	this->_size = o._size;
	this->_block_size = o._block_size;
	this->_blocks_num = o._blocks_num;*/
	this->_block_ptrs = new dynamic_bitset<>*[SparseBitsetU::_blocks_num];
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if(o._block_ptrs[i]!=NULL )
		{
			if(o._block_ptrs[i]->count()==1)
				this->_block_ptrs[i] = o._block_ptrs[i]; //if unit vector block, then just get the pointer in the pool
			else
				this->_block_ptrs[i] = new dynamic_bitset<>((*o._block_ptrs[i]));
		}
		else
		{
			this->_block_ptrs[i] = NULL;
		}
	}
}

SparseBitsetU::SparseBitsetU(const SparseBitsetU* o)
{
	this->_block_ptrs = new dynamic_bitset<>*[SparseBitsetU::_blocks_num];
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if(o->_block_ptrs[i]==NULL)
			this->_block_ptrs[i]=NULL;
		else if(o->_block_ptrs[i]->count()==1 )
			this->_block_ptrs[i] = o->_block_ptrs[i];
		else
			this->_block_ptrs[i]= new dynamic_bitset<>((*o->_block_ptrs[i]));
	}
}

SparseBitsetU::~SparseBitsetU()
{ 
	for( int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if( this->_block_ptrs[i] != NULL )
		{
			if( this->_block_ptrs[i]->count()!=1) //do not delete the pool now
				delete this->_block_ptrs[i];
		}
	}
	delete [] this->_block_ptrs;
}

/*
void SparseBitsetU::shiftLSB(int shift_by)
{
	if( shift_by>0)
	{ 
		bool car; 
		car = false; 
		int i,j;
		int rightmost_non_empty_block;
		rightmost_non_empty_block = 0;
		int new_rightmost = SparseBitsetU::_blocks_num-1;

		for(j=1; j<=shift_by; j++)
		{
			for( i=(SparseBitsetU::_blocks_num-1); i>=rightmost_non_empty_block; i--)
			{   
				if( this->_block_ptrs[i]==NULL)
				{ 
					if( car )
					{
						car = false;
						this->_block_ptrs[i] = &SparseBitsetU::_unit_vector_blocks_pool[SparseBitsetU::_block_size-1];

//						this->_block_ptrs[i] = new dynamic_bitset<>(SparseBitsetU::_block_size);
//						this->_block_ptrs[i]->set(SparseBitsetU::_block_size-1);	
						new_rightmost = i;					
					}
					else
					{ 
						//do nothing
						continue;
					}
				}
				else if( this->_block_ptrs[i]->count()==1)
				{ 
					if( car )
					{ 
						if( this->_block_ptrs[i]->test(0) )
						{
							this->_block_ptrs[i] = new dynamic_bitset<>(SparseBitsetU::_block_size);
							this->_block_ptrs[i]->set(SparseBitsetU::_block_size-1);	
							new_rightmost = i;							
							car = true;
						}
						else
						{
							new_rightmost = i;
							car = false;
							
							int set_block_bit_id; //the already set bit from a unit vector block

							set_block_bit_id = this->_block_ptrs[i] - SparseBitsetU::_unit_vector_blocks_pool;
							this->_block_ptrs[i] = new dynamic_bitset<>(SparseBitsetU::_block_size);
							this->_block_ptrs[i]->set(set_block_bit_id);							
							
							this->_block_ptrs[i]->set(SparseBitsetU::_block_size-1);
						}
					}
					else
					{ 
						if( this->_block_ptrs[i]->test(0) )
						{ 
							this->_block_ptrs[i]=NULL;
							car = true;
						}
						else
						{ 
							this->_block_ptrs[i] = this->_block_ptrs[i]-1;
							new_rightmost = i; 
						}
					}
				}
				else
				{ 
					if( this->_block_ptrs[i]->test(0) )
					{ 
						car = true;
					}
					else
					{ 
						car = false;
					} 
					(*this->_block_ptrs[i])>>=1;	
					//As initially it is not a unit vector block, and as we shift by 1, 
					//there is no way to become empty.
//					if(this->_block_ptrs[i]->none())
//					{
//						delete this->_block_ptrs[i];
//						this->_block_ptrs[i]=NULL;
//					}	
//					else
//					{	
						new_rightmost = i;
//					}


				} 
			}
			rightmost_non_empty_block = new_rightmost;
		}
	}
}*/

double SparseBitsetU::getEmptyBlocksNum()
{
	double empty_blocks_num;
	empty_blocks_num = 0;
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			empty_blocks_num++;
		}
	}
	return empty_blocks_num;
}

double SparseBitsetU::getUnaryBlocksNum()
{
	double blocks_num;
	blocks_num = 0;
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL)
		{
			if(this->_block_ptrs[i]->count()==1)
			{
				blocks_num++;
			}
		}
	}
	return blocks_num;			
}

void SparseBitsetU::set(int bit_id)
{ 
	int block_id; 
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitsetU::_block_size);
	block_bit_id = bit_id%SparseBitsetU::_block_size;
	
	if( this->_block_ptrs[block_id] == NULL )
	{
		this->_block_ptrs[block_id] = &SparseBitsetU::_unit_vector_blocks_pool[block_bit_id];
		return;
	}
	else if( this->_block_ptrs[block_id]->count()==1)
	{ 
		int set_block_bit_id; //the already set bit from a unit vector block

		set_block_bit_id = this->_block_ptrs[block_id] - SparseBitsetU::_unit_vector_blocks_pool;

		if( set_block_bit_id == block_bit_id )
			return; //the bit is already set

		this->_block_ptrs[block_id] = new dynamic_bitset<>(SparseBitsetU::_block_size);
		this->_block_ptrs[block_id]->set(set_block_bit_id); //cout<<"set: "<<set_block_bit_id<<", new: "<<block_bit_id<<endl;
	}

		this->_block_ptrs[block_id]->set(block_bit_id);
}

/*
void SparseBitsetU::flipNonEmptyBlock(int bit_id)
{
	int block_id; 
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitsetU::_block_size);
	block_bit_id = bit_id%SparseBitsetU::_block_size;
	
	this->_block_ptrs[block_id]->flip(block_bit_id);
}*/
/*
void SparseBitsetU::flipNonEmptyBlock(int block_id, int block_bit_id)
{
	if( this->_block_ptrs[block_id]->count()==1 )
	{ cout<<"UNIT!"<<endl;
		int set_block_bit_id;
		set_block_bit_id = this->_block_ptrs[block_id] - SparseBitsetU::_unit_vector_blocks_pool;
		cout<<"id:"<<set_block_bit_id<<endl;
		this->_block_ptrs[block_id] = new dynamic_bitset<>(SparseBitsetU::_block_size);
		this->_block_ptrs[block_id]->set(set_block_bit_id);
	}
	this->_block_ptrs[block_id]->flip(block_bit_id);
}*/
/*
bool SparseBitsetU::runNoneInBlock(int block_id)
{
	return this->_block_ptrs[block_id]->none();
}*/

bool SparseBitsetU::test(int bit_id)
{
	int block_id;
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitsetU::_block_size);
	block_bit_id = bit_id%SparseBitsetU::_block_size;
	
	if( this->_block_ptrs[block_id] == NULL )
		return false;
	else
		return this->_block_ptrs[block_id]->test(block_bit_id);
}

/*
bool SparseBitsetU::testInNonEmptyBlock(int bit_id)
{
	int block_id;
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)this->_block_size);
	block_bit_id = bit_id%this->_block_size;
	
	return this->_block_ptrs[block_id]->test(block_bit_id);
}

bool SparseBitsetU::testInNonEmptyBlock(int block_id, int block_bit_id)
{
	return this->_block_ptrs[block_id]->test(block_bit_id);
}
*/

bool SparseBitsetU::testBlock(int block_id)
{
	if(this->_block_ptrs[block_id]==NULL )
		return false;
	else 
		return true;
}

void SparseBitsetU::print(void)
{ 
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			for(int j=0; j<SparseBitsetU::_block_size; j++)
			{
				cout<<"0";
			}
			cout<<" ";
		}
		else
		{
			cout<<(*this->_block_ptrs[i])<<" ";
		}
	}
	cout<<endl;
}

int SparseBitsetU::count(void)
{
	int set_bits;
	set_bits = 0;
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if(this->_block_ptrs[i]!=NULL)
		{
			set_bits += this->_block_ptrs[i]->count();
		}
	}
	return set_bits;
}

dynamic_bitset<> SparseBitsetU::getBitset(void)
{
	dynamic_bitset<> bitset(SparseBitsetU::_size);
	for(int i=0; i<SparseBitsetU::_size; i++)
	{
		if( this->test(i) )
		{
			bitset.set(i);
		}
	}
	return bitset;
}

dynamic_bitset<> SparseBitsetU::getBlock(int block_id)
{
	return (*this->_block_ptrs[block_id]);
}

void SparseBitsetU::clear(void)
{
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL )
		{
			if( this->_block_ptrs[i]->count()!=1 )
				delete this->_block_ptrs[i];
			this->_block_ptrs[i] = NULL;
		}
	}
}

void SparseBitsetU::reset(void)
{ 
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{ 
		if( this->_block_ptrs[i]!=NULL )
		{ 
			if( this->_block_ptrs[i]->count()!=1 )
				delete this->_block_ptrs[i];
			this->_block_ptrs[i] = NULL;
		}
	}
}


void SparseBitsetU::set(dynamic_bitset<> bitset)
{
	this->clear();
	for(int i=0; i<SparseBitsetU::_size; i++)
	{
		if( bitset.test(i) )
		{
			this->set(i);
		}
	}
}

void SparseBitsetU::set(dynamic_bitset<>* bitset_ptr)
{ 
	this->clear();
	for(int i=0; i<SparseBitsetU::_size; i++)
	{
		if( bitset_ptr->test(i) )
		{
			this->set(i);
		}
	}
}

void SparseBitsetU::setEfficient(dynamic_bitset<>* bitset_ptr)
{
	this->clear();
	for(int i=0; i<SparseBitsetU::_size; i++)
	{
		if( bitset_ptr->test(i) )
		{
			this->set(i);
			bitset_ptr->flip(i);
			if( bitset_ptr->none() )
				break;
		}
	}
}

void SparseBitsetU::set(void)
{
	for(int i=0; i<SparseBitsetU::_blocks_num; i++)
	{
		if( this->_block_ptrs[i] == NULL || this->_block_ptrs[i]->count()==1)
		{
			this->_block_ptrs[i] = new dynamic_bitset<>(SparseBitsetU::_block_size);
		}
		this->_block_ptrs[i]->set();
	}
}

bool SparseBitsetU::empty(void)
{
	for(int i=0; i<this->_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL )
		{
			return false;
		}
	}
	return true; 
}

int SparseBitsetU::size(void)
{
	return SparseBitsetU::_size;
}

void SparseBitsetU::randomSet(int bit_num)
{
	int bit_id; //The id of the bit to be set. 
	
	SparseBitsetU::randomInit();
	
	for(int i=0; i<bit_num; i++)
	{
		bit_id = rand() % SparseBitsetU::_size;
		this->set(bit_id);
	}
}

int SparseBitsetU::getBlocksNum(void)
{
	return SparseBitsetU::_blocks_num;
}

int SparseBitsetU::getBlockSize(void)
{
	return SparseBitsetU::_block_size;
} 




//=========================================================================================

bool SparseBitsetC::_random_init = false; 
int SparseBitsetC::_size;
int SparseBitsetC::_block_size;
int SparseBitsetC::_blocks_num;
int SparseBitsetC::_last_bit_id;
//vector<dynamic_bitset<> > SparseBitsetC::_blocks_pool_clean;
map<dynamic_bitset<>,short int> SparseBitsetC::_blocks_pool;


SparseBitsetC::SparseBitsetC(int size, int block_size)
{ 
	SparseBitsetC::_size = size;
	SparseBitsetC::_block_size = block_size;
	SparseBitsetC::_blocks_num = ceil((double)SparseBitsetC::_size/(double)SparseBitsetC::_block_size);
	SparseBitsetC::_last_bit_id = (SparseBitsetC::_size%SparseBitsetC::_block_size)-1;
	this->_block_ptrs = new dynamic_bitset<>*[this->_blocks_num];
	for( int i=0; i<this->_blocks_num; i++)
	{
		this->_block_ptrs[i] = NULL; //initially all blocks are assumed to contain zeros
	}
}

SparseBitsetC::SparseBitsetC(const SparseBitsetC& o)
{
	this->_block_ptrs = new dynamic_bitset<>*[SparseBitsetC::_blocks_num];
	for(int i=0; i<SparseBitsetC::_blocks_num; i++)
	{
		if(o._block_ptrs[i]!=NULL )
		{
			this->_block_ptrs[i] = o._block_ptrs[i]; //if unit vector block, then just get the pointer in the pool
		}
		else
		{
			this->_block_ptrs[i] = NULL;
		}
	}
}

SparseBitsetC::~SparseBitsetC()
{
	delete [] this->_block_ptrs;
}

void SparseBitsetC::cleanBlockPool( void )
{
	SparseBitsetC::_blocks_pool.clear();
}

double SparseBitsetC::poolByteSize(void)
{
	double size;
	size = 0;
	size += SparseBitsetC::_blocks_pool.size()*(SparseBitsetC::_blocks_num*sizeof(unsigned long)+sizeof(short int));
	return size;
}

void SparseBitsetC::set(int bit_id)
{  
	int block_id; 
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitsetC::_block_size); 
	block_bit_id = bit_id%SparseBitsetC::_block_size;

	if( this->_block_ptrs[block_id] == NULL ) //if the block was empty
	{ 
		map<dynamic_bitset<>,short int>::iterator cur_bs;

		//Create the new block
		dynamic_bitset<> tmp_block(SparseBitsetC::_block_size);
		tmp_block.set(block_bit_id);

		//Update the blocks pool and the block pointer
		cur_bs = SparseBitsetC::_blocks_pool.find(tmp_block);
		if( cur_bs==SparseBitsetC::_blocks_pool.end() ) //if the new block is not in the block pool
		{ 
			SparseBitsetC::_blocks_pool.insert( std::pair<dynamic_bitset<>,short int>(tmp_block,1)); //insert the block in the pool
		}
		else
		{ 
			cur_bs->second++;
		}
		cur_bs = SparseBitsetC::_blocks_pool.find(tmp_block);
		this->_block_ptrs[block_id] = (dynamic_bitset<>*) &cur_bs->first; //get a pointer to the block
	}
	else //if the pointer was associated in another bitset in the pool
	{  
		map<dynamic_bitset<>,short int>::iterator cur_bs;
		dynamic_bitset<> tmp_block(SparseBitsetC::_block_size);
		tmp_block = (*this->_block_ptrs[block_id]);
		
		//Delete the previous entry if it is necessary
		cur_bs = SparseBitsetC::_blocks_pool.find(*this->_block_ptrs[block_id]);
		if( cur_bs->second == 1 ) //if this was the only one pointer to this bitset
			SparseBitsetC::_blocks_pool.erase(cur_bs); 
		
//		cout<<"id="<<(*this->_block_ptrs[block_id])<<endl; 
//		this->print();
		
//		cout<<">"<<this->_block_ptrs[block_id]->size()<<endl;
		//Create the new block
//		dynamic_bitset<> tmp_block(500); tmp_block = (*this->_block_ptrs[block_id]);
//		cout<<"<"<<this->_block_ptrs[block_id]->size()<<endl;
//		cout<<"{"<<tmp_block.size()<<endl;// cout<<"'"<<tmp_block<<"'"<<endl;

		tmp_block.set(block_bit_id);

		//Update the blocks pool and the block pointer
		cur_bs = SparseBitsetC::_blocks_pool.find(tmp_block);
		if( cur_bs==SparseBitsetC::_blocks_pool.end() ) //if the new block is not in the block pool
		{
			SparseBitsetC::_blocks_pool.insert( std::pair<dynamic_bitset<>,short int>(tmp_block,1)); //insert the block in the pool
		}
		else
		{ 
			cur_bs->second++;
		}
		cur_bs = SparseBitsetC::_blocks_pool.find(tmp_block);
		this->_block_ptrs[block_id] = (dynamic_bitset<>*) &cur_bs->first; //get a pointer to the block
		

	} //cout<<"after:"<<endl; //cout<<(*this->_block_ptrs[block_id])<<endl;
}

double SparseBitsetC::byteSize(bool no_opts)
{
	double size; 
	
	//Consider the size of the block pointers.
//	size = SparseBitsetC::_blocks_num*sizeof(dynamic_bitset<>*); 

dynamic_bitset<> xxx(SparseBitsetC::_blocks_num);
size = xxx.num_blocks()*sizeof(unsigned long);
 

	//Consider the size of the blocks.
	for(int i=0; i<SparseBitsetC::_blocks_num; i++)
	{ 
		if( this->_block_ptrs[i]!=NULL ) //if null pointer then no block!
		{ 
			if( no_opts )
			{ 
				size += this->_block_ptrs[i]->num_blocks()*sizeof(unsigned long);
				//@note boost::dynamic_bitset<> stores the bitset as a set of blocks,
				//where each block is an integer (default: unsigned long)
			}
			size+=sizeof(int);
		}
	}
	return size;
}

double SparseBitsetC::countEmptyBlocks()
{
	double empty_blocks_num;
	empty_blocks_num = 0;
	for(int i=0; i<SparseBitsetC::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			empty_blocks_num++;
		}
	}
	return empty_blocks_num;
}

int SparseBitsetC::count(void)
{
	int set_bits;
	set_bits = 0;
	for(int i=0; i<SparseBitsetC::_blocks_num; i++)
	{
		if(this->_block_ptrs[i]!=NULL)
		{
			set_bits += this->_block_ptrs[i]->count();
		}
	}
	return set_bits;
}

bool SparseBitsetC::empty(void)
{
	for(int i=0; i<this->_blocks_num; i++)
	{
		if( this->_block_ptrs[i]!=NULL )
		{
			return false;
		}
	}
	return true; 
}

void SparseBitsetC::print(void)
{ 
	for(int i=0; i<SparseBitsetC::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			for(int j=0; j<SparseBitsetC::_block_size; j++)
			{
				cout<<"0";
			}
			cout<<" ";
		}
		else
		{
			cout<<(*this->_block_ptrs[i])<<" ";
		}
	}
	cout<<endl;
}

int SparseBitsetC::size(void)
{
	return SparseBitsetC::_size;
}

bool SparseBitsetC::testBlock(int block_id)
{
	if(this->_block_ptrs[block_id]==NULL )
		return false;
	else 
		return true;
}

dynamic_bitset<> SparseBitsetC::getBlock(int block_id)
{
	return (*this->_block_ptrs[block_id]);
}

int SparseBitsetC::getBlocksNum(void)
{
	return SparseBitsetC::_blocks_num;
}




//=========================================================================================

bool SparseBitsetUltraC::_random_init = false; 
int SparseBitsetUltraC::_size;
int SparseBitsetUltraC::_block_size;
int SparseBitsetUltraC::_blocks_num;
int SparseBitsetUltraC::_last_bit_id;
map<dynamic_bitset<>,short int> SparseBitsetUltraC::_blocks_pool;


SparseBitsetUltraC::SparseBitsetUltraC(int size, int block_size)
{ 
	SparseBitsetUltraC::_size = size;
	SparseBitsetUltraC::_block_size = block_size;
	SparseBitsetUltraC::_blocks_num = ceil((double)SparseBitsetUltraC::_size/(double)SparseBitsetUltraC::_block_size);
	SparseBitsetUltraC::_last_bit_id = (SparseBitsetUltraC::_size%SparseBitsetUltraC::_block_size)-1;

	this->_blocks_status.resize(SparseBitsetUltraC::_blocks_num); 
}

SparseBitsetUltraC::SparseBitsetUltraC(const SparseBitsetUltraC& o)
{
	this->_blocks_status = o._blocks_status;
	
	this->_block_ptrs = o._block_ptrs;
}

SparseBitsetUltraC::~SparseBitsetUltraC()
{
}

void SparseBitsetUltraC::cleanBlockPool( void )
{
	SparseBitsetUltraC::_blocks_pool.clear();
}

double SparseBitsetUltraC::poolByteSize(void)
{
	double size;
	size = 0;
	size += SparseBitsetUltraC::_blocks_pool.size()*(SparseBitsetUltraC::_blocks_num*sizeof(unsigned long)+sizeof(short int));
	return size;
}

void SparseBitsetUltraC::set(int bit_id)
{  
	int block_id; 
	int block_bit_id;
	
	block_id = floor((double)bit_id/(double)SparseBitsetUltraC::_block_size); 
	block_bit_id = bit_id%SparseBitsetUltraC::_block_size;

	if( !this->_blocks_status.test(block_id) ) //if the block was empty
	{
		map<dynamic_bitset<>,short int>::iterator cur_bs;
		
		//Create a tmp block
		dynamic_bitset<>tmp_block(SparseBitsetUltraC::_block_size);
		tmp_block.set(block_bit_id);
		
		//Update the pool (if needed) and the block pointer
		cur_bs = SparseBitsetUltraC::_blocks_pool.find(tmp_block);
		if( cur_bs == SparseBitsetUltraC::_blocks_pool.end() ) //if the tmp block is not in the blocks pool
		{
			SparseBitsetUltraC::_blocks_pool.insert( std::pair<dynamic_bitset<>,short int>(tmp_block,1)); //insert it in the blocks pool
		}
		else //otherwise
		{
			cur_bs->second++;
		}
		this->_blocks_status.set(block_id); //update the block status bitset
		cur_bs = SparseBitsetUltraC::_blocks_pool.find(tmp_block); //find the proper entry in the pool
		this->_block_ptrs.push_back((dynamic_bitset<>*)&cur_bs->first);
	}
	else //otherwise
	{
		dynamic_bitset<>** tmp_arr;
//		vector<dynamic_bitset<>*> tmp_vct;
		vector<dynamic_bitset<>*>::iterator it;
//		tmp_vct = this->_block_ptrs;
		it = this->_block_ptrs.begin();
		tmp_arr = new dynamic_bitset<>*[SparseBitsetUltraC::_blocks_num];
		for(int i=0; i<SparseBitsetUltraC::_blocks_num; i++)
		{
			if( this->_blocks_status.test(i) )
			{
				tmp_arr[i] = (*it);
				it++;
			}
			else
			{
				tmp_arr[i]=0;
			}
		}
		this->_block_ptrs.clear();

		map<dynamic_bitset<>,short int>::iterator cur_bs;
		dynamic_bitset<> tmp_block(SparseBitsetUltraC::_block_size);
		tmp_block = (*tmp_arr[block_id]);
		
		//Delete the previous entry if it is necessary
		cur_bs = SparseBitsetUltraC::_blocks_pool.find(*tmp_arr[block_id]);
		if( cur_bs->second == 1 ) //if this was the only one pointer to this bitset
			SparseBitsetUltraC::_blocks_pool.erase(cur_bs); 		

		tmp_block.set(block_bit_id);

		//Update the blocks pool and the block pointer
		cur_bs = SparseBitsetUltraC::_blocks_pool.find(tmp_block);
		if( cur_bs==SparseBitsetUltraC::_blocks_pool.end() ) //if the new block is not in the block pool
		{
			SparseBitsetUltraC::_blocks_pool.insert( std::pair<dynamic_bitset<>,short int>(tmp_block,1)); //insert the block in the pool
		}
		else
		{ 
			cur_bs->second++;
		}
		cur_bs = SparseBitsetUltraC::_blocks_pool.find(tmp_block);
		tmp_arr[block_id] = (dynamic_bitset<>*) &cur_bs->first; //get a pointer to the block
		
				this->_blocks_status.set(block_id); //update the block status bitset

		
		for(int i=0; i<SparseBitsetUltraC::_blocks_num; i++)
		{
			if( this->_blocks_status.test(block_id) )
			{
				this->_block_ptrs.push_back(tmp_arr[i]);
			}
		}
		
		delete [] tmp_arr;
		
	}
}

/*
double SparseBitsetUltraC::byteSize(bool no_opts)
{
	double size; 
	
	//Consider the size of the block pointers.
//	size = SparseBitsetC::_blocks_num*sizeof(dynamic_bitset<>*); 

dynamic_bitset<> xxx(SparseBitsetUltraC::_blocks_num);
size = xxx.num_blocks()*sizeof(unsigned long);
 

	//Consider the size of the blocks.
	for(int i=0; i<SparseBitsetUltraC::_blocks_num; i++)
	{ 
		if( this->_block_ptrs[i]!=NULL ) //if null pointer then no block!
		{ 
			if( no_opts )
			{ 
				size += this->_block_ptrs[i]->num_blocks()*sizeof(unsigned long);
				//@note boost::dynamic_bitset<> stores the bitset as a set of blocks,
				//where each block is an integer (default: unsigned long)
			}
			size+=sizeof(int);
		}
	}
	return size;
}

double SparseBitsetUltraC::countEmptyBlocks()
{
	double empty_blocks_num;
	empty_blocks_num = 0;
	for(int i=0; i<SparseBitsetUltraC::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			empty_blocks_num++;
		}
	}
	return empty_blocks_num;
}
*/
int SparseBitsetUltraC::count(void)
{
	int set_bits;
	set_bits = 0;
	vector<dynamic_bitset<>*>::iterator it;
	it = this->_block_ptrs.begin();
	for(int i=0; i<SparseBitsetUltraC::_blocks_num; i++)
	{
		if(this->_blocks_status.test(i))
		{
			set_bits += (*it)->count();
			it++;
		}
	}
	return set_bits;
}

bool SparseBitsetUltraC::empty(void)
{
	if( this->_blocks_status.none() )
		return true;
	else
		return false;
}
/*
void SparseBitsetUltraC::print(void)
{ 
	for(int i=0; i<SparseBitsetUltraC::_blocks_num; i++)
	{
		if( this->_block_ptrs[i]==NULL)
		{
			for(int j=0; j<SparseBitsetUltraC::_block_size; j++)
			{
				cout<<"0";
			}
			cout<<" ";
		}
		else
		{
			cout<<(*this->_block_ptrs[i])<<" ";
		}
	}
	cout<<endl;
}
*/
int SparseBitsetUltraC::size(void)
{
	return SparseBitsetUltraC::_size;
}

bool SparseBitsetUltraC::testBlock(int block_id)
{
	if( this->_blocks_status.test(block_id) )
		return true;
	else
		return false;
/*	if(this->_block_ptrs[block_id]==NULL )
		return false;
	else 
		return true;*/
}
/*
dynamic_bitset<> SparseBitsetUltraC::getBlock(int block_id)
{
	return (*this->_block_ptrs[block_id]);
}*/

int SparseBitsetUltraC::getBlocksNum(void)
{
	return SparseBitsetUltraC::_blocks_num;
}
