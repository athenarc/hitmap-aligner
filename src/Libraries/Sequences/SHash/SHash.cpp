/************************************************************************************************//*
 * @file SHash.h
 *
 * This file implements methods of class SHash. 
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/
#include "SHash.h"

/***************************************************************************************************
 * @name HitmapTable methods
 */
//@{

HitmapTable::HitmapTable( string& data_str, unsigned long gram_length, unsigned long q_len, unsigned long q_thr, int hitmap_size)
{
	string tmp_str; //A buffer string. 
	unsigned long frag_num; //The number of fragments in the data sequence. 
	tr1::unordered_map<string,int> tmp_map; //used to get the hash function and bucket number.

	this->_hitmap_size = hitmap_size; //Get the size of each hitmap (i.e., the number of table columns).
	
	//get the hash function.
	this->_hash_func = tmp_map.hash_function();	

	frag_num = data_str.length()-gram_length+1;
	cout<<"number of fragments to be indexed:"<<frag_num<<endl;
	
	
	this->_buckets_num = frag_num;

	unsigned long r_len; //the region length
	
	r_len = (data_str.length()+(this->_hitmap_size-1)*(q_len+q_thr-1))/this->_hitmap_size; 
	
	cout<<"Length of regions: "<<r_len<<endl;
	cout<<"Clean length: "<<(r_len-q_len-q_thr)<<endl;

//cout<<"r_len:"<<r_len<<endl;


//	this->_hitmaps_num = hitmaps_num; //Get the number of hitmaps (i.e., the number of table rows).

	//Allocate space for the table of hits. 
//	this->_hits = new vector<bool>*[this->_buckets_num];
	this->_hits = new bitset<HITMAP_SIZE>*[this->_buckets_num];


//	unsigned long qgram_num;
	unsigned long tmp_p;
//debugging
int percentage,cur_percentage;





unsigned long insertions;
insertions = 0;

//	this->_h_table = new vector<unsigned long>*[this->_buckets_num];
	
//just to be sure
for(int f=0; f<this->_buckets_num; f++)
{
	this->_hits[f] = 0; //this value will remain only to those buckets that contain no hit
}
	
	cur_percentage = -1; int hitn; hitn=0;
	
	bitset<HITMAP_SIZE> zero (0ul);

	/***********************************************************************************************
	 * Scan the data sequence to get all the fragments. For each fragment store the hits at the 
	 * corresponding bit of the hitmap. 
	 */
	for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
	{ 
		tmp_str = data_str.substr(i,gram_length); //get the current q-gram 

		#ifdef VERBOSE
		percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-gram_length+1)))*(double)100);
		if( percentage%10 ==0 && cur_percentage!=percentage)
		{ 
			cur_percentage = percentage;
			cout<<"Importing grams: ";
			cout<<percentage<<"%";
			cout<<endl;
		}
		#endif

		//If this is the first hit for this bucket 
		if( this->_hits[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0)
		{ 
			//Create the hitmap (boolean vector).
//			this->_hits[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<bool>;
			this->_hits[(this->_hash_func(tmp_str)%this->_buckets_num)] = new bitset<HITMAP_SIZE>;
			
			//Fill the hitmap with zeros.
//			this->_hits[(this->_hash_func(tmp_str)%this->_buckets_num)]->assign(this->_hitmap_size,0);	
			(*this->_hits[(this->_hash_func(tmp_str)%this->_buckets_num)]) = zero;
			
		} 


		int j;
		j = floor((i+gram_length-1)/(r_len-q_len-q_thr)); //get the last window that contains this gram

		if( j<this->_hitmap_size)
		{  
			this->setHit( (this->_hash_func(tmp_str)%this->_buckets_num), j);
			
			//attention! the largest occurrence length is q_len+q_thr. in order to don't miss 
			//occurrences we get the regions with overlaps of size q_len+q_thr-1. therefore, the 
			//j-th region starts at position j*(r_len-q_len-q_thr).
			

			
			for( int u=(j-1); u>0; u--)
			{
				if( (i+gram_length-1) <= (u*(r_len-q_len-q_thr)+r_len-1))
				{
					this->setHit( (this->_hash_func(tmp_str)%this->_buckets_num), u);
				}
				else
				{
					break;
				}
			}
		}
		else
		{ 
			//TODO na ta lamvano ypopsin
		}
			
		

 
	}


//	cout<<"r_len:"<<r_len<<", q_len:"<<q_len<<", q_thr:"<<q_thr<<", frag_num:"<<frag_num<<", gram_length:"<<gram_length<<endl;
//	cout<<"hitn:"<<hitn<<endl;

}

void HitmapTable::setHit( int hitmap_id, int hit_id)
{
//	this->_hits[hitmap_id]->at(hit_id) = 1;
	(*this->_hits[hitmap_id])[hit_id] = 1;
}

void HitmapTable::getMergePlan( unsigned long* hitmaps_list, int hl_size, int* merge_plan)
{ 
	for( int i=0; i<this->_hitmap_size; i++)
	{ 
		merge_plan[i]=0;
		for( int j=0; j<hl_size; j++)
		{ 
//			if( this->_hits[hitmaps_list[j]]->at(i)==1)
			if( (*this->_hits[hitmaps_list[j]])[i]==1)
			{ 
				merge_plan[i]++;
			}
		}	
	} //cout<<endl<<"end get merge plan"<<endl;
}

/*
void HitmapTable::getAreas( unsigned long* hitmaps_list, int hl_size, bitset<HITMAP_SIZE>* areas, int thr)
{ 
	bitset<HITMAP_SIZE>* products;
	bitset<HITMAP_SIZE>* items;
	bitset<HITMAP_SIZE> zero (0ul);
	
	unsigned long items_num;
	unsigned long products_num;
	items_num = hl_size;
	unsigned long u;

	items = new bitset<HITMAP_SIZE>[items_num];
	for( int x=0; x<items_num; x++)
	{
		items[x] = (*this->_hits[hitmaps_list[x]]);
		
	}
	
	for( int z=1; z<thr; z++)
	{ 
		products_num = ((items_num-1)*items_num)/2; //the maximum number of products
		u = 0;
		products = new bitset<HITMAP_SIZE>[products_num];
		for( int i=0; i<items_num; i++)
		{
			for( int j=i+1; j<items_num; j++)
			{ 
				products[u] = items[i] & items[j];
				
				if( products[u]!= zero )
				{
					u++;
				}
			}
		}
		items_num = u;
		delete [] items;
		items = products;
	}
	
	(*areas) = zero;
	for( int g=0; g<items_num; g++)
	{ 
		(*areas) = (*areas) | items[g]; 
	}

	
	delete [] items;
}*/

void HitmapTable::getCandRegions( unsigned long* hitmaps_list, int hl_size, bitset<HITMAP_SIZE>* regions, int occ_thr)
{ 
	occ_thr = hl_size - occ_thr; //ATTENTION!!! From now on occ_thr is the number of zeros neeeded

	//Create a bitset full of aces (by using the value -1 which has "11...1" representation)
	bitset<HITMAP_SIZE> aces (-1);
	
	//We use occ_thr mask-bitsets. Initially, each of them is full of '1's.
	bitset<HITMAP_SIZE>* mask_bitsets;
	mask_bitsets = new bitset<HITMAP_SIZE>[occ_thr];
	for( int i=0; i<occ_thr; i++)
	{
		mask_bitsets[i] = aces;
	}

	bitset<HITMAP_SIZE>* swap_bitsets;	
	bitset<HITMAP_SIZE>* mask_bitsets_new;
	mask_bitsets_new = new bitset<HITMAP_SIZE>[occ_thr];
	for( int u=0; u<occ_thr; u++)
	{
		mask_bitsets_new[u] = aces;
	}
	
	//We also use hl_size result-bitsets. These bitsets are the results of performing OR between
	//each hitmap and a mask-bitset. At the end, we perform an AND of them to get the regions 
	//bitset.
	bitset<HITMAP_SIZE>* res_bitsets;
	res_bitsets = new bitset<HITMAP_SIZE>[hl_size];

	int z,q; //Auxiliary counters (declared here for time opt).
	bitset<HITMAP_SIZE> tmp_bitset; //Auxiliary bitset
	//Now we compute the values for the results-bitsets.	
	for( int j=0; j<hl_size; j++)
	{
		//Compute the j-th results-bitset.
		res_bitsets[j] = (*this->_hits[hitmaps_list[j]]);
		for( z=0; z<occ_thr; z++)
		{
			res_bitsets[j] |= mask_bitsets[z];
			
			//Update the value for the current mask-bitset.
			tmp_bitset = (*this->_hits[hitmaps_list[j]]);
			for( q=(z-1); q>=0; q--)
			{
				tmp_bitset |= mask_bitsets[q];
			}
			mask_bitsets_new[z] = mask_bitsets[z] & tmp_bitset;
		}
		swap_bitsets = mask_bitsets;
		mask_bitsets = mask_bitsets_new;
		mask_bitsets_new = swap_bitsets;
	}
	
	//Finally, we perform an AND to all res_bitsets.
	(*regions) = aces; 
	for( int w=0; w<hl_size; w++)
	{
		(*regions) &= res_bitsets[w]; 
	}
	
	//Free memory!
	delete [] res_bitsets;
	delete [] mask_bitsets;
} 

unsigned long HitmapTable::getHashKey(string query_str)
{
	return (this->_hash_func(query_str)%this->_buckets_num);
}

int HitmapTable::getHitmapSize()
{
	return this->_hitmap_size;
}

void HitmapTable::showStats(void)
{
	cout<<"==Hitmap statistics=="<<endl;
	cout<<"\t*Size: "<<this->_buckets_num<<endl;

	unsigned long num_of_empty;
	num_of_empty = 0;
	for(int i=0; i<this->_buckets_num; i++)
	{
		if(this->_hits[i]==0)
		{
			num_of_empty++;
		}
	}
	cout<<"\t*Empty buckets: "<<num_of_empty<<endl;
}

//@}


/***************************************************************************************************
 * @name HitmapTableOld methods
 */
//@{

HitmapTableOld::HitmapTableOld( int hitmaps_num, int hitmap_size)
{
	this->_hitmaps_num = hitmaps_num; //Get the number of hitmaps (i.e., the number of table rows).
	this->_hitmap_size = hitmap_size; //Get the size of each hitmap (i.e., the number of table columns).
cout<<"ALLOCATE!"<<endl;	
	//Allocate space for the table of hits. 
	this->_hits = new vector<bool>[this->_hitmaps_num];
cout<<"END ALLOCATE!"<<endl;	

	//Initialize all hitmaps to contain zeros.
/*	for( int i=0; i<this->_hitmaps_num; i++)
	{
		this->_hits[i].assign(this->_hitmap_size,0);
	}*/
//Don't do the previous now, because it will consume much memory.
}

void HitmapTableOld::setHit( int hitmap_id, int hit_id)
{
	this->_hits[hitmap_id][hit_id] = 1;
}

void HitmapTableOld::initHitmap( int hitmap_id )
{
	this->_hits[hitmap_id].assign(this->_hitmap_size,0);
}

int HitmapTableOld::getHit( int hitmap_id, int hit_id)
{
	return this->_hits[hitmap_id][hit_id];
}

void HitmapTableOld::getMergePlan( unsigned long* hitmaps_list, int hl_size, int* merge_plan)
{
	//Initialize the merge plan
	for( int i=0; i<this->_hitmap_size; i++)
	{
		merge_plan[i]=0;
	}
	
	for( int i=0; i<this->_hitmap_size; i++)
	{
		for( int j=0; j<hl_size; j++)
		{
			if( this->_hits[hitmaps_list[j]][i]==1)
			{
				merge_plan[i]++;
			}
		}		
	}
}

int HitmapTableOld::getHitmapSize()
{
	return this->_hitmap_size;
}

//@}

/***************************************************************************************************
 * @name SHashDir methods
 */
//@{

SHashDir::SHashDir( string& data_str, unsigned long gram_length)
{
	string tmp_str; //A buffer string. 
	unsigned long frag_num;
//	unsigned long qgram_num;
	unsigned long tmp_p;
//debugging
int percentage,cur_percentage;



	tr1::unordered_map<string,int> tmp_map; //used to get the hash function and bucket number.
	//get the hash function.
	this->_hash_func = tmp_map.hash_function();	

	frag_num = data_str.length()-gram_length+1;
	cout<<"number of fragments to be indexed:"<<frag_num<<endl;
	
//	qgram_num = floor(window_length/gram_length);
	
	char opt;
	cout<<"Do you want to avoid collisions in the hash table (this will need more memory): ";
	cin>>opt;
	
	if(opt=='y')
	{
		int* frag_cnt;
		char** frag_str;
		bool coll = true;
		
		cout<<"start computing proper modulo..."<<endl;
		for( unsigned long m=frag_num; coll; m=m+1000)
		{
			cout<<"\tTesting modulo: "<<m<<endl;
	
			frag_cnt = new int[m];
			frag_str = new char*[m];
		
	
			cout<<"\t\tInitializing..."<<endl;
			//First initialize the counters.
			for(unsigned long i=0; i<m; i++)
			{
				frag_cnt[i] = 0;
				frag_str[i] = new char[gram_length];
			}
	
			coll = false;
	
			cout<<"\t\tImporting solid fragments..."<<endl;		
			for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
			{
				tmp_str = data_str.substr(i,gram_length); //get the current q-gram
		
				if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())!=0 )
				{
					coll = true;
						
					cout<<"collision!"<<endl;
					
					break;
				}
				else if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())==0 )
				{
				
				}
				else
				{
					frag_cnt[this->_hash_func(tmp_str)%m]++;
					strcpy( frag_str[this->_hash_func(tmp_str)%m], tmp_str.c_str());
				}
			}	
			
			if(coll==false)
			{
				this->_buckets_num = m;
			}
			
			delete [] frag_cnt;
			for(int r=0; r<m; r++)
			{
				delete [] frag_str[r];
			}
			delete [] frag_str;
	}
	}
	else
	{
		cout<<"Specify the number of buckets by hand ,or fill '0' to get the default ("<<frag_num<<"):";
		cin>>this->_buckets_num;
		
		if( this->_buckets_num == 0 )
		{
			this->_buckets_num = frag_num;
		}
	}

unsigned long insertions;
insertions = 0;

	this->_h_table = new vector<unsigned long>*[this->_buckets_num];
	
//just to be sure
for(int f=0; f<this->_buckets_num; f++)
{
	this->_h_table[f] = 0;
}
	
	cur_percentage = -1;

	for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
	{
		tmp_str = data_str.substr(i,gram_length); //get the current q-gram

		percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-gram_length+1)))*(double)100);
		if( percentage%10 ==0 && cur_percentage!=percentage)
		{ cur_percentage = percentage;
				cout<<"Importing grams: ";
				cout<<percentage<<"%";
				cout<<endl;
		}

//		tmp_p._position = i;
//		tmp_p._type = 0;
//		tmp_p = i;
		
//		cout<<"[[["<<(this->_hash_func(tmp_str)%this->_buckets_num)<<"]]]"<<endl;
		
		if( this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0) 
		{
			this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<unsigned long>;
		}
		this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)]->push_back(i); 
		insertions++;
	}


	
	cout<<"\tinsertions:"<<insertions<<endl; 
}

SHashDir::SHashDir( string& data_str, unsigned long gram_length, string mode)
{
	if( mode=="memobserve")
	{
		string tmp_str; //A buffer string. 
		unsigned long frag_num;
		unsigned long tmp_p;
		tr1::unordered_map<string,int> tmp_map; //used to get the hash function and bucket number.
	
		//get the hash function.
		this->_hash_func = tmp_map.hash_function();	
	
		frag_num = data_str.length()-gram_length+1;
		this->_buckets_num = frag_num;
	
		this->_h_table = new vector<unsigned long>*[this->_buckets_num];
		
		//just to be sure
		for(int f=0; f<this->_buckets_num; f++)
		{
			this->_h_table[f] = 0;
		}
	
		for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
		{
			tmp_str = data_str.substr(i,gram_length); //get the current q-gram
	
			
			if( this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0) 
			{
				this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<unsigned long>;
			}
			this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)]->push_back(i); 
		}
	}
	else
	{
		string tmp_str; //A buffer string. 
		unsigned long frag_num;
		unsigned long tmp_p;
		tr1::unordered_map<string,int> tmp_map; //used to get the hash function and bucket number.
	
		//get the hash function.
		this->_hash_func = tmp_map.hash_function();	
	
		frag_num = data_str.length()-gram_length+1;
		this->_buckets_num = frag_num;
	
		this->_h_table = new vector<unsigned long>*[this->_buckets_num];
		
		//just to be sure
		for(int f=0; f<this->_buckets_num; f++)
		{
			this->_h_table[f] = 0;
		}
	
		for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
		{
			tmp_str = data_str.substr(i,gram_length); //get the current q-gram
	
			
			if( this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0) 
			{
				this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<unsigned long>;
			}
			this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)]->push_back(i); 
		}
	}
	
	unsigned long lists = 0;
	unsigned long items = 0;
	for(unsigned long i=0; i<this->_buckets_num; i++)
	{
		if( this->_h_table[i]!=0 )
		{
			items += this->_h_table[i]->size();
			lists++;
		}
	}
	cout<<"llen:"<<(float)((float)items/(float)lists)<<endl;
}

SHashDir::~SHashDir(void)
{
}

vector<unsigned long>* SHashDir::find(string query_str)
{
	return _h_table[(this->_hash_func(query_str)%this->_buckets_num)];
}

vector<unsigned long>* SHashDir::get(unsigned long i)
{
	return _h_table[i];
}

unsigned long SHashDir::getBucketsNum()
{
	return  this->_buckets_num;
}

void SHashDir::clear()
{
	//First delete the vectors.
	for(int i=0; i<this->_buckets_num; i++)
	{
		delete this->_h_table[i]; 
	}
	
	
	//Then delete the array.
	delete [] this->_h_table;
}

void SHashDir::showStats(void)
{
	cout<<"==Hash statistics=="<<endl; //cout<<this->_buckets_num<<endl;// this->getHitmapSize();
	cout<<"\t*Size: "<<this->_buckets_num<<endl;

	unsigned long num_of_empty;
	num_of_empty = 0;
	for(int i=0; i<this->_buckets_num; i++)
	{
		if(this->_h_table[i]==0)
		{
			num_of_empty++;
		}
	}
	cout<<"\t*Empty buckets: "<<num_of_empty<<endl;
}

//@}

/***************************************************************************************************
 * @name StrHashDirLR methods
 */
//@{

StrHashDirLR::StrHashDirLR( string& data_str, unsigned long gram_length, unsigned long q_len, unsigned long q_thr, unsigned long r_num) : SHashDir( data_str, gram_length)
{
	//Just initialize the HitmapTableOld.
	vector<unsigned long>::iterator cur_pos;
	unsigned long r_len; //the region length
	
	r_len = (data_str.length()+(r_num-1)*(q_len+q_thr-1))/r_num; cout<<"rlen:"<<r_len<<endl;
	
	this->_hitmap_table = new HitmapTableOld(this->_buckets_num,r_num);

//cout<<"buckets_num="<<this->_buckets_num<<", r_num="<<r_num<<endl;

cout<<"START"<<endl;
	for( int i=0; i<this->_buckets_num; i++)
	{ 
		this->_hitmap_table->initHitmap(i);
		if(this->_h_table[i]!=0) //If there is a list.
		{
			cur_pos = this->_h_table[i]->begin();
			while( cur_pos != this->_h_table[i]->end())
			{ 
				for( int j=0; j<r_num; j++)
				{
					int hit_start;
					int hit_end;
					
					hit_start = j*(r_len-q_len-q_thr+1);
					hit_end = hit_start+r_len-1;
					
					if( (data_str.length()-1-hit_end+1+q_len+q_thr) < r_len )
					{ 
						hit_end = data_str.length()-1;
					}
										
/*					if( i==1414313 )
					{
						cout<<"j:"<<j<<", cur_pos:"<<(*cur_pos)<<", hit_start:"<<hit_start<<", hit_end:"<<hit_end<<"(data_size:"<<data_str.length()<<")"<<endl;
					}*/
					
					if( (*cur_pos)>=hit_start && (*cur_pos)<=hit_end )
					{
//						cout<<"[i:"<<i<<",j:"<<j<<"], cur_pos:"<<(*cur_pos)<<", hit_start:"<<hit_start<<", hit_end:"<<hit_end<<endl;
						this->_hitmap_table->setHit( i, j);
					}
				} 
				cur_pos++;
			}
			
			//==delete the corrsponding list==
			this->_h_table[i]->clear();
		}
	}
	//==delete the hash table==
	delete [] this->_h_table;
	
cout<<"END"<<endl;
}

vector<unsigned long>* StrHashDirLR::find(string query_str)
{
	return _h_table[(this->_hash_func(query_str)%this->_buckets_num)];
}

unsigned long StrHashDirLR::getHashKey(string query_str)
{
	return (this->_hash_func(query_str)%this->_buckets_num);
}

void StrHashDirLR::clear()
{
	//First delete the vectors.
	for(int i=0; i<this->_buckets_num; i++)
	{
		delete this->_h_table[i]; 
	}
		
	//Then delete the array.
	delete [] this->_h_table;
	
	//Delete the hitmap table.
	delete [] this->_hitmap_table;
}

void StrHashDirLR::getMergePlan( unsigned long* hitmaps_list, int hl_size, int* merge_plan)
{
	this->_hitmap_table->getMergePlan( hitmaps_list, hl_size, merge_plan);
}

int StrHashDirLR::getHitmapSize()
{
	return this->_hitmap_table->getHitmapSize();
}

//@}

SHash::SHash( string& data_str, int gram_length)
{
	string tmp_str; //A buffer string. 
	tr1::unordered_map<string,IList>::iterator it; //An iterator to the hash.
int percentage, cur_percentage;

	if( data_str.length() < gram_length ) 
	{
		cout<<"ERROR -- [creating hash table] The gram length is larger than the length of the data sequence."<<endl;
	}
	
	//Get all the <gram_length>-grams into the hash table. 
	for(int i=0; i<=(data_str.length()-gram_length); i++)
	{
//percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-gram_length+1)))*(double)100);
//if( percentage%10 ==0 && cur_percentage!=percentage)
//{ cur_percentage = percentage;
//		cout<<"Importing grams: ";
//		cout<<percentage<<"%";
//		cout<<endl;
//}

		tmp_str = data_str.substr(i,gram_length); //get the current q-gram
		it = _h_table.find(tmp_str); //search for it in the hash
		
		if( it != _h_table.end()) //if there is in the hash
		{
			it->second._positions.push_back(i); //add the current position into the entry of this q-gram
		}
		else //if not
		{
			//add a new entry 
			IList tmp_list;
			tmp_list._positions.push_back(i);
			_h_table.insert(pair<string,IList>(tmp_str,tmp_list));
		}
		
	}
	
	//
}

SHash2::SHash2( string& data_str, unsigned long gram_length, unsigned long window_length)
{
	string tmp_str; //A buffer string. 
	unsigned long frag_num;
	unsigned long qgram_num;
	Position tmp_p;
//debugging
int percentage,cur_percentage;



	tr1::unordered_map<string,int> tmp_map; //used to get the hash function and bucket number.
	//get the hash function.
	this->_hash_func = tmp_map.hash_function();	

	frag_num = data_str.length()-gram_length+1+(data_str.length()-window_length+1)*((gram_length)-1);
	cout<<"number of fragments to be indexed:"<<frag_num<<endl;
	
	qgram_num = floor(window_length/gram_length);
	
/*	char opt;
	cout<<"Do you want to avoid collisions in the hash table (this will need more memory): ";
	cin>>opt;
	
	if(opt=='y')
	{
		int* frag_cnt;
		char** frag_str;
		bool coll = true;
		
		cout<<"start computing proper modulo..."<<endl;
		for( unsigned long m=frag_num; coll; m=m+1000)
		{
			cout<<"\tTesting modulo: "<<m<<endl;
	
			frag_cnt = new int[m];
			frag_str = new char*[m];
		
	
			cout<<"\t\tInitializing..."<<endl;
			//First initialize the counters.
			for(unsigned long i=0; i<m; i++)
			{
				frag_cnt[i] = 0;
				frag_str[i] = new char[gram_length];
			}
	
			coll = false;
	
			cout<<"\t\tImporting solid fragments..."<<endl;		
			for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
			{
				tmp_str = data_str.substr(i,gram_length); //get the current q-gram
		
				if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())!=0 )
				{
					coll = true;
						
					cout<<"collision!"<<endl;
					
					break;
				}
				else if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())==0 )
				{
				
				}
				else
				{
					frag_cnt[this->_hash_func(tmp_str)%m]++;
					strcpy( frag_str[this->_hash_func(tmp_str)%m], tmp_str.c_str());
				}
			}	
	
			if(coll==false)
			{
				cout<<"\t\tImporting diveded fragments..."<<endl;		
				for(unsigned long i=0; i<=data_str.length()-window_length; i++)
				{
					for( unsigned long l_len=1; l_len<gram_length; l_len++)
					{
						unsigned long r_len = gram_length-l_len;
		
						tmp_str = data_str.substr(i+qgram_num*gram_length-r_len,r_len)+data_str.substr(i,l_len);
						
						if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())!=0 )
						{
							coll = true;
								
							cout<<"collision!"<<endl;
							
							break;
						}
						else if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())==0 )
						{
						
						}
						else
						{
							frag_cnt[this->_hash_func(tmp_str)%m]++;
							strcpy( frag_str[this->_hash_func(tmp_str)%m], tmp_str.c_str());
						}
					}
				}
			}
			
			if(coll==false)
			{
				this->_buckets_num = m;
			}
			
			delete [] frag_cnt;
			for(int r=0; r<m; r++)
			{
				delete [] frag_str[r];
			}
			delete [] frag_str;
	}
	}
	else
	{*/
		cout<<"Specify the number of buckets by hand ,or fill '0' to get the default ("<<frag_num<<"):";
		cin>>this->_buckets_num;
		
		if( this->_buckets_num == 0 )
		{
			this->_buckets_num = frag_num;
		}
//	}

unsigned long insertions;
insertions = 0;

	this->_h_table = new vector<Position>*[this->_buckets_num];
	
//just to be sure
for(int f=0; f<this->_buckets_num; f++)
{
	this->_h_table[f] = 0;
}
	
	cur_percentage = -1;

	for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
	{
		tmp_str = data_str.substr(i,gram_length); //get the current q-gram

		percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-gram_length+1)))*(double)100);
		if( percentage%10 ==0 && cur_percentage!=percentage)
		{ cur_percentage = percentage;
				cout<<"Importing grams: ";
				cout<<percentage<<"%";
				cout<<endl;
		}

		tmp_p._position = i;
		tmp_p._type = 0;
		
//		cout<<"[[["<<(this->_hash_func(tmp_str)%this->_buckets_num)<<"]]]"<<endl;
		
		if( this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0) 
		{
			this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<Position>;
		}
		this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)]->push_back(tmp_p); 
		insertions++;
	}

	//Get the extra fragments.
	for(unsigned long i=0; i<=data_str.length()-window_length; i++)
	{ 
		percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-window_length+1)))*(double)100);
		if( percentage%10 ==0 && cur_percentage!=percentage)
		{ cur_percentage = percentage;
				cout<<"Importing grams{"<<i<<"}: ";
				cout<<percentage<<"%";
				cout<<endl;
		}
	
		for( unsigned long l_len=1; l_len<gram_length; l_len++)
		{
			unsigned long r_len = gram_length-l_len;
			tmp_str = data_str.substr(i+qgram_num*gram_length-r_len,r_len)+data_str.substr(i,l_len);
			tmp_p._position = i+qgram_num*gram_length-r_len;
			tmp_p._type = l_len;
			if( this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0) 
			{
				this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<Position>;
			}
			this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)]->push_back(tmp_p);	
			insertions++;
		}
	} 
	
	cout<<"\tinsertions:"<<insertions<<endl;
}

SHash2::SHash2( string& data_str, unsigned long gram_length, unsigned long window_length, unsigned long sl_num)
{
	string tmp_str; //A buffer string. 
	unsigned long frag_num;
	unsigned long qgram_num;
	Position tmp_p;
//debugging
int percentage,cur_percentage;



	tr1::unordered_map<string,int> tmp_map; //used to get the hash function and bucket number.
	//get the hash function.
	this->_hash_func = tmp_map.hash_function();	

	frag_num = data_str.length()-gram_length+1+(data_str.length()-window_length+1)*(sl_num);
	cout<<"number of fragments to be indexed:"<<frag_num<<endl;
	
	qgram_num = floor(window_length/gram_length);
	
/*	char opt;
	cout<<"Do you want to avoid collisions in the hash table (this will need more memory): ";
	cin>>opt;
	
	if(opt=='y')
	{
		int* frag_cnt;
		char** frag_str;
		bool coll = true;
		
		cout<<"start computing proper modulo..."<<endl;
		for( unsigned long m=frag_num; coll; m=m+1000)
		{
			cout<<"\tTesting modulo: "<<m<<endl;
	
			frag_cnt = new int[m];
			frag_str = new char*[m];
		
	
			cout<<"\t\tInitializing..."<<endl;
			//First initialize the counters.
			for(unsigned long i=0; i<m; i++)
			{
				frag_cnt[i] = 0;
				frag_str[i] = new char[gram_length];
			}
	
			coll = false;
	
			cout<<"\t\tImporting solid fragments..."<<endl;		
			for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
			{
				tmp_str = data_str.substr(i,gram_length); //get the current q-gram
		
				if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())!=0 )
				{
					coll = true;
						
					cout<<"collision!"<<endl;
					
					break;
				}
				else if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())==0 )
				{
				
				}
				else
				{
					frag_cnt[this->_hash_func(tmp_str)%m]++;
					strcpy( frag_str[this->_hash_func(tmp_str)%m], tmp_str.c_str());
				}
			}	
	
			if(coll==false)
			{
				cout<<"\t\tImporting diveded fragments..."<<endl;		
				for(unsigned long i=0; i<=data_str.length()-window_length; i++)
				{
					for( unsigned long l_len=1; l_len<=sl_num; l_len++)
					{
						unsigned long r_len = gram_length-l_len;
		
						tmp_str = data_str.substr(i+qgram_num*gram_length-r_len,r_len)+data_str.substr(i,l_len);
						
						if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())!=0 )
						{
							coll = true;
								
							cout<<"collision!"<<endl;
							
							break;
						}
						else if( frag_cnt[this->_hash_func(tmp_str)%m]==1 && strcmp(frag_str[this->_hash_func(tmp_str)%m],tmp_str.c_str())==0 )
						{
						
						}
						else
						{
							frag_cnt[this->_hash_func(tmp_str)%m]++;
							strcpy( frag_str[this->_hash_func(tmp_str)%m], tmp_str.c_str());
						}
					}
				}
			}
			
			if(coll==false)
			{
				this->_buckets_num = m;
			}
			
			delete [] frag_cnt;
			for(int r=0; r<m; r++)
			{
				delete [] frag_str[r];
			}
			delete [] frag_str;
	}
	}
	else
	{*/
		cout<<"Specify the number of buckets by hand ,or fill '0' to get the default ("<<frag_num<<"):";
		cin>>this->_buckets_num;
		
		if( this->_buckets_num == 0 )
		{
			this->_buckets_num = frag_num;
		}
//	}

	unsigned long insertions;
	insertions = 0;

	this->_h_table = new vector<Position>*[this->_buckets_num];
	
	//just to be sure
	for(int f=0; f<this->_buckets_num; f++)
	{
		this->_h_table[f] = 0;
	}
	
	cur_percentage = -1;

	for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
	{
		tmp_str = data_str.substr(i,gram_length); //get the current q-gram

		percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-gram_length+1)))*(double)100);
		if( percentage%10 ==0 && cur_percentage!=percentage)
		{ cur_percentage = percentage;
				cout<<"Importing grams: ";
				cout<<percentage<<"%";
				cout<<endl;
		}

		tmp_p._position = i;
		tmp_p._type = 0;
		
//		cout<<"[[["<<(this->_hash_func(tmp_str)%this->_buckets_num)<<"]]]"<<endl;
		
		if( this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0) 
		{
			this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<Position>;
		}
		this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)]->push_back(tmp_p); 
		insertions++;
	}

	//Get the extra fragments.
	for(unsigned long i=0; i<=data_str.length()-window_length; i++)
	{ 
		percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-window_length+1)))*(double)100);
		if( percentage%10 ==0 && cur_percentage!=percentage)
		{ cur_percentage = percentage;
				cout<<"Importing grams{"<<i<<"}: ";
				cout<<percentage<<"%";
				cout<<endl;
		}
	
		for( unsigned long l_len=1; l_len<=sl_num; l_len++)
		{
			unsigned long r_len = gram_length-l_len;
			tmp_str = data_str.substr(i+qgram_num*gram_length-r_len,r_len)+data_str.substr(i,l_len);
			tmp_p._position = i+qgram_num*gram_length-r_len;
			tmp_p._type = l_len;
			if( this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] == 0) 
			{
				this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)] = new vector<Position>;
			}
			this->_h_table[(this->_hash_func(tmp_str)%this->_buckets_num)]->push_back(tmp_p);	
			insertions++;
		}
	} 
	
	cout<<"\tinsertions:"<<insertions<<endl;
}

void SHash2::clear()
{
	//First delete the vectors.
	for(int i=0; i<this->_buckets_num; i++)
	{
		if( this->_h_table[i]!=0 )
		{
			delete this->_h_table[i]; 
		}
	}
	
	
	//Then delete the array.
	delete [] this->_h_table;
}

SHash::SHash( string& data_str, int min_gram_length, int max_gram_length)
{
	string tmp_str; //A buffer string.
	tr1::unordered_map<string,IList>::iterator it; //An iterator to the hash.
	int gram_length; //The current gram length.
	
	for( gram_length=min_gram_length; gram_length<=max_gram_length; gram_length++)
	{
		//Get all the <gram_length>-grams into the hash table. 
		for(int i=0; i<(data_str.length()-gram_length); i++)
		{
			tmp_str = data_str.substr(i,gram_length); //get the current q-gram
			it = _h_table.find(tmp_str); //search for it in the hash
			
			if( it != _h_table.end()) //if there is in the hash
			{
				it->second._positions.push_back(i); //add the current position into the entry of this q-gram
			}
			else //if not
			{
				//add a new entry 
				IList tmp_list;
				tmp_list._positions.push_back(i);
				_h_table.insert(pair<string,IList>(tmp_str,tmp_list));
			}
			
		}
		cout<<"Size :"<<gram_length<<" is ready..."<<endl;
	}
}

SHash2::~SHash2(void)
{
}

SHash::~SHash(void)
{
	//Free memory which you don't need. 
	
	tr1::unordered_map<string,IList>::iterator it; //an iterator to the hash
//	list<int>::iterator cur_pos; //an iterator to the inverted list
	
	it = _h_table.begin(); //the iterator points to the first
	
	while( it!=_h_table.end())
	{
		(*it).second._positions.clear(); //clear the elements of the list. 
/*		cur_pos = (*it).second._positions.begin();
		while( cur_pos!= (*it).second._positions.end())
		{
			cout<<(*cur_pos)<<" ";
			cur_pos++;
		}
		cout<<endl;*/
		it++;
	}
	
	_h_table.clear(); //clear the elements of the hash table.
	
	
}

int SHash::size(void)
{
	return _h_table.size();
}

int SHash2::size(void)
{
//	return _h_table.size();
}

int SHash2::byteSize(void)
{
/*	tr1::unordered_map<string,list<Position> >::iterator it; //an iterator to the hash
	list<Position>::iterator cur_pos; //an iterator to the inverted list
	int bsize = 0;
	
	it = _h_table.begin(); //the iterator points to the first
	
	while( it!=_h_table.end())
	{
		bsize += (*it).first.length()*sizeof(char); 
//		bsize += (*it).second._positions.size()*(sizeof(int)+sizeof(short int));
		bsize += (*it).second.size()*(sizeof(Position));

		it++;
	}
//	cout<<"{"<<sizeof(Position)<<"}"<<endl;

	cout<<"Size of hash:"<<_h_table.size()<<endl;


	bsize = bsize/1024; //KB
	bsize = bsize/1024; //MB

	return bsize;*/
}

/*
int SHash2::byteSize(void)
{
	tr1::unordered_map<string,IList2>::iterator it; //an iterator to the hash
	list<Position>::iterator cur_pos; //an iterator to the inverted list
	int bsize = 0;
	
	it = _h_table.begin(); //the iterator points to the first
	
	while( it!=_h_table.end())
	{
		bsize += (*it).first.length()*sizeof(char); 
//		bsize += (*it).second._positions.size()*(sizeof(int)+sizeof(short int));
		bsize += (*it).second._positions.size()*(sizeof(Position));

		it++;
	}
//	cout<<"{"<<sizeof(Position)<<"}"<<endl;

	bsize = bsize/1024; //KB
	bsize = bsize/1024; //MB

	return bsize;
}*/

int SHash::byteSize(void)
{
	tr1::unordered_map<string,IList>::iterator it; //an iterator to the hash
	list<int>::iterator cur_pos; //an iterator to the inverted list
	int bsize = 0;
	
	it = _h_table.begin(); //the iterator points to the first
	
	while( it!=_h_table.end())
	{
		bsize += (*it).first.length()*sizeof(char); 
		bsize += (*it).second._positions.size()*sizeof(int);

		it++;
	}
	cout<<"{"<<sizeof(int)<<"}"<<endl;
	return bsize;
}

tr1::unordered_map<string,IList>::iterator SHash::find(string query_str)
{
	return _h_table.find(query_str);
}

vector<Position>* SHash2::find(string query_str)
{
	return _h_table[(this->_hash_func(query_str)%this->_buckets_num)];
}

bool SHash::exists(tr1::unordered_map<string,IList>::iterator it)
{
	if( it == _h_table.end() ) return false;
	else return true;
}

void SHash::print(void)
{
	tr1::unordered_map<string,IList>::iterator it; //an iterator to the hash
	list<int>::iterator cur_pos; //an iterator to the inverted list
	
	it = _h_table.begin(); //the iterator points to the first
	
	while( it!=_h_table.end())
	{
		cout<<"["<<(*it).first<<"] ";
		
		cur_pos = (*it).second._positions.begin();
		 while( cur_pos!= (*it).second._positions.end())
		 {
			 cout<<(*cur_pos)<<" ";
			 cur_pos++;
		 }
		cout<<endl;
		it++;
	}
	
}

/*
void SHash2::print(void)
{
	tr1::unordered_map<string,IList2>::iterator it; //an iterator to the hash
	list<Position>::iterator cur_pos; //an iterator to the inverted list
	
	it = _h_table.begin(); //the iterator points to the first
	
	while( it!=_h_table.end())
	{
		cout<<"["<<(*it).first<<"] ";
		
		cur_pos = (*it).second._positions.begin();
		while( cur_pos!= (*it).second._positions.end())
		{
			cout<<(*cur_pos)._position<<"(t"<<(*cur_pos)._type<<") ";
			cur_pos++;
		}
		cout<<endl;
		it++;
	}
	
}*/

void SHash::showStats(void)
{
	tr1::unordered_map<string,IList>::iterator cur_entry;
	unsigned long avg_list_len;

	avg_list_len = 0;
		
	cout<<"Info about hash table."<<endl;
	cout<<"|\tnum of entries: "<<this->size()<<endl;
	cur_entry = this->_h_table.begin();
	while( cur_entry!= this->_h_table.end() )
	{
		avg_list_len += (*cur_entry).second._positions.size();
		cur_entry++;
	}
	cout<<"|\tavg list len: "<<(avg_list_len/this->size())<<endl;
}

void SHash2::showStats(void)
{
/*	tr1::unordered_map<string,IList2>::iterator cur_entry;
	unsigned long avg_list_len;

	avg_list_len = 0;
		
	cout<<"Info about hash table."<<endl;
	cout<<"|\tnum of entries: "<<this->size()<<endl;
	cur_entry = this->_h_table.begin();
	while( cur_entry!= this->_h_table.end() )
	{
		avg_list_len += (*cur_entry).second._positions.size();
		cur_entry++;
	}
	cout<<"|\tavg list len: "<<(avg_list_len/this->size())<<endl;*/
}

SHash3::SHash3( string& data_str, unsigned long gram_length, unsigned long window_length)
{
	string tmp_str; //A buffer string. 
	tr1::unordered_map<string,IList2>::iterator it; //An iterator to the hash.
	int percentage,cur_percentage;
	
	if( data_str.length() < gram_length ) 
	{
		cout<<"ERROR -- [creating hash table] The gram length is larger than the length of the data sequence."<<endl;
	}
	
	//Get all the <gram_length>-grams into the hash table. 
	for(unsigned long i=0; i<=(data_str.length()-gram_length); i++)
	{
		tmp_str = data_str.substr(i,gram_length); //get the current q-gram
		it = _h_table.find(tmp_str); //search for it in the hash

// percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-gram_length+1)))*(double)100);
// if( percentage%10 ==0 && cur_percentage!=percentage)
// { cur_percentage = percentage;
// 		cout<<"Importing grams: ";
// 		cout<<percentage<<"%";
// 		cout<<endl;
// }
		
		if( it != _h_table.end()) //if there is in the hash
		{
			Position tmp_p;
			tmp_p._position = i;
			tmp_p._type = 0;
//			tmp_p._order = -1;
			
			it->second._positions.push_back(tmp_p); //add the current position into the entry of this q-gram
		}
		else //if not
		{
			//add a new entry 
			IList2 tmp_list;
			Position tmp_p;
			tmp_p._position = i;
			tmp_p._type = 0;
//			tmp_p._order = -1;
			tmp_list._positions.push_back(tmp_p);
			_h_table.insert(pair<string,IList2>(tmp_str,tmp_list));
//			tmp_list._positions.clear();
		}
		
	}

	unsigned long qgram_num = floor(window_length/gram_length);
//	unsigned long order = 0;	
	//Get the extra fragments.
	for(unsigned long i=0; i<=data_str.length()-window_length; i++)
	{ //cout<<"\twin:{"<<i<<","<<(i+window_length-1)<<"}"<<endl;
		
percentage = (int)(((double) ((double)(i+1)/(double)(data_str.length()-window_length+1)))*(double)100);
if( percentage%10 == 0 && percentage!= cur_percentage)
{	
	cur_percentage = percentage;
	cout<<"Importing extra grams: ";
	cout<<percentage<<"%"<<endl;
	cout<<this->byteSize();
	cout<<endl;
}
		for( unsigned long l_len=1; l_len<gram_length; l_len++)
		{
			unsigned long r_len = gram_length-l_len;
	
			tmp_str = data_str.substr(i+qgram_num*gram_length-r_len,r_len)+data_str.substr(i,l_len);
	//		cout<<"t"<<l_len<<" '"<<tmp_str<<"'"<<endl;
			it = _h_table.find(tmp_str); //search for it in the hash
			if( it != _h_table.end()) //if there is in the hash
			{
				Position tmp_p;
				tmp_p._position = i+qgram_num*gram_length-r_len;
				tmp_p._type = l_len;
//				tmp_p._order = order;
//				order++;
				
				it->second._positions.push_back(tmp_p);
			}
			else
			{
				Position tmp_p;
				tmp_p._position = i+qgram_num*gram_length-r_len;
				tmp_p._type = l_len;
//				tmp_p._order = order;
//				order++;
				IList2 tmp_list;
				tmp_list._positions.push_back(tmp_p);
				_h_table.insert(pair<string,IList2>(tmp_str,tmp_list));
//			tmp_list._positions.clear();


			}

		}
	} 
	
}

tr1::unordered_map<string,IList2>::iterator SHash3::find(string query_str)
{
	return _h_table.find(query_str);
}

int SHash3::size(void)
{
	return _h_table.size();
}

int SHash3::byteSize(void)
{
	tr1::unordered_map<string,IList2>::iterator it; //an iterator to the hash
	list<Position>::iterator cur_pos; //an iterator to the inverted list
	int bsize = 0;
	
	it = _h_table.begin(); //the iterator points to the first
	
	while( it!=_h_table.end())
	{
		bsize += (*it).first.length()*sizeof(char); 
//		bsize += (*it).second._positions.size()*(sizeof(int)+sizeof(short int));
		bsize += (*it).second._positions.size()*(sizeof(Position));

		it++;
	}
//	cout<<"{"<<sizeof(Position)<<"}"<<endl;

	bsize = bsize/1024; //KB
	bsize = bsize/1024; //MB

	return bsize;
}

void SHash3::showStats(void)
{
	tr1::unordered_map<string,IList2>::iterator cur_entry;
	unsigned long avg_list_len;

	avg_list_len = 0;
		
	cout<<"Info about hash table."<<endl;
	cout<<"|\tnum of entries: "<<this->size()<<endl;
	cur_entry = this->_h_table.begin();
	while( cur_entry!= this->_h_table.end() )
	{
		avg_list_len += (*cur_entry).second._positions.size();
		cur_entry++;
	}
	cout<<"|\tavg list len: "<<(avg_list_len/this->size())<<endl;
}

SHash3::~SHash3(void)
{
}
