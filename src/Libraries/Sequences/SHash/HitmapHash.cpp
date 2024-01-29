#include "HitmapHash.h"
using namespace hmp; // using the Hitmap namespace

//========================================================================================
//

#ifdef COLUMN_STORED_BUCKETS

HitmapLightHash::HitmapLightHash(string *dseq_ptr, unsigned long fnum, unsigned long flen, unsigned long qlen, unsigned long qthr, unsigned long block_size, unsigned long buckets_num)
{
	string cur_str; // a string bucket
	unsigned long cur_bucket;

	this->_dseq_ptr = dseq_ptr; // get the pointer to the data sequence
	this->_qthr = qthr;
	this->_qlen = qlen;
	this->_flen = flen; // get the length of the fragments
	this->_fnum = fnum; // get the number of fragments
	//	this->_part_len = 2*(this->_qlen+2*this->_qthr); //get the length of the parts
	this->_part_len = this->_qlen + this->_qthr; // NEW!
	this->_dseq_len = this->_dseq_ptr->length();
	this->_block_size = block_size;
	this->_buckets_num = buckets_num;
	//	this->_part_shift = this->_part_len/2;
	this->_part_shift = 1; // NEW!
}

HitmapLightHash::HitmapLightHash(void)
{
}

HitmapLightHash::~HitmapLightHash(void)
{
	tr1::unordered_map<string, HitmapLightBucket>::iterator it;
	it = this->_hash_table.begin();
	while (it != this->_hash_table.end())
	{
		delete it->second._hitmap;
		it++;
	}
}

void HitmapLightHash::build(void)
{
	string cur_str;
	unsigned long parts_num;
	unsigned long part_shift;
	//	part_shift = this->_qlen+2*this->_qthr;
	//	parts_num = 2*ceil((float)this->_dseq_len/(float)this->_part_len)-1;
	part_shift = 1;									   // NEW!
	parts_num = this->_dseq_len - this->_part_len + 1; // NEW!

	unsigned long cur_part_start_l;
	unsigned long cur_part_start_r;
	unsigned long next_part_start_l;
	unsigned long next_part_start_r;

	unsigned long cur_part_id;
	cur_part_id = 0;
	cur_part_start_l = 0;
	cur_part_start_r = cur_part_start_l + this->_part_len - this->_flen;
	next_part_start_l = cur_part_start_l + part_shift;
	next_part_start_r = cur_part_start_r + part_shift;

	HitmapLightBucket tmp;
	//	unsigned long cnt; cnt = 0;

	// Scan dseq to fill the hash table buckets
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{
		// Get the corresponding dseq subsequence
		cur_str = this->_dseq_ptr->substr(p, this->_flen);

		if (this->_hash_table.find(cur_str) == this->_hash_table.end())
		{
			tmp._hitmap = new SparseBitset(parts_num, this->_block_size); // cnt++;
			this->_hash_table.insert(std::make_pair /*<std::string,HitmapLightBucket>*/ (cur_str, tmp));
		}

		for (int i = 0; i < this->_part_len; i++) // NEW!
		{
			int part_id = p - i; // NEW!
			if (part_id >= 0)	 // NEW!
			{
				this->_hash_table.find(cur_str)->second._hitmap->set(part_id); // NEW!
			}
		}

		/*		//each fragment can be contained in at most 2 parts.
				if( p>=cur_part_start_l && p<=cur_part_start_r )
				{
					this->_hash_table.find(cur_str)->second._hitmap->set(cur_part_id);
				}
				else
				{
					cur_part_id++;
					cur_part_start_l = next_part_start_l;
					cur_part_start_r = next_part_start_r;
					next_part_start_l += part_shift;
					next_part_start_r += part_shift;
					if( p>=cur_part_start_l && p<=cur_part_start_r )
						this->_hash_table.find(cur_str)->second._hitmap->set(cur_part_id);
				}

				if( p>=next_part_start_l && p<=next_part_start_r )
					this->_hash_table.find(cur_str)->second._hitmap->set(cur_part_id+1);*/
	}

	// cout<<"cnt="<<cnt<<endl;

	if (this->_buckets_num != 0) // 0 means use default...
		this->_hash_table.rehash(this->_buckets_num);
	else
		this->_hash_table.bucket_count();
}

void HitmapLightHash::store(string output_file, string ab, double constr_time)
{
	ofstream fout;
	fout.open(output_file.c_str());
	unsigned long cur_bucket;

	cout << "\t * Producing file meta-data..." << endl;

	// Write time related metadata.
	time_t rawtime;
	time(&rawtime);
	fout << "@created:\t" << ctime(&rawtime);
	fout << "@construction:\t" << constr_time << " sec" << endl;
	fout << "@fragment_length:\t" << this->_flen << endl;
	fout << "@fragments:\t" << this->_fnum << endl;
	fout << "@part_length:\t" << this->_part_len << endl;
	fout << "@err_threshold:\t" << this->_qthr << endl;
	fout << "@buckets:\t" << this->_buckets_num << endl;
	fout << "@block_size:\t" << this->_block_size << endl;
	fout << "@alphabet:\t" << ab << endl;

	// Write server related metadata.
	char hostname[HOST_NAME_MAX];
	int result;
	result = gethostname(hostname, HOST_NAME_MAX);
	if (result)
	{
		cout << "=> ERROR: Problem while reading system info." << endl;
		return;
	}
	fout << "@server:\t" << hostname << endl;

	// Write size related metadata.
	fout << "@block_size:\t" << this->_block_size << endl;
	//	fout<<"@size:\t\t"<<this->byteSize()<<" bytes"<<endl;
	fout << "@data:" << endl;

	cout << "\t * Done!" << endl;

	cout << "\t * Producing data..." << endl;

	tr1::unordered_map<string, HitmapLightBucket>::iterator it;
	it = this->_hash_table.begin();
	cur_bucket = this->_hash_table.bucket(it->first);
	fout << ">" << cur_bucket << " ";
	while (it != this->_hash_table.end())
	{
		if (cur_bucket != this->_hash_table.bucket(it->first))
		{
			cur_bucket = this->_hash_table.bucket(it->first);
			fout << endl
				 << ">" << cur_bucket << " ";
		}
		fout << it->first << ":" << it->second._hitmap->getOnesListDelim() << " ";
		it++;
	}

	fout << endl;
	cout << "\t * Done!" << endl;
	fout.close();
}

void HitmapLightHash::load(string input_file, string *dseq_ptr, unsigned long qlen)
{
	this->_qlen = qlen;
	this->_dseq_ptr = dseq_ptr;
	this->_dseq_len = this->_dseq_ptr->length();
	ifstream fin;
	fin.open(input_file.c_str());
	if (fin.fail())
	{
		cout << "=> ERROR: Cannot open '" << input_file << "' (idx file). No such file." << endl
			 << endl;
		return;
	}
	string cur_token;

	cout << "\t * Reading file meta-data..." << endl;
	fin >> cur_token;
	while (cur_token != "@fragment_length:")
	{
		fin >> cur_token;
	}
	fin >> this->_flen;
	while (cur_token != "@fragments:")
	{
		fin >> cur_token;
	}
	fin >> this->_fnum;
	fin >> cur_token;
	while (cur_token != "@part_length:")
	{
		fin >> cur_token;
	}
	fin >> this->_part_len;
	fin >> cur_token;
	while (cur_token != "@err_threshold:")
	{
		fin >> cur_token;
	}
	fin >> this->_qthr;
	fin >> cur_token;
	while (cur_token != "@buckets:")
	{
		fin >> cur_token;
	}
	fin >> this->_buckets_num;
	fin >> cur_token;
	while (cur_token != "@block_size:")
	{
		fin >> cur_token;
	}
	fin >> this->_block_size;
	fin >> cur_token;
	while (cur_token != "@alphabet:")
	{
		fin >> cur_token;
	}
	fin >> this->_ab;

	//	this->_part_shift = this->_part_len/2;
	this->_part_shift = 1; // NEW!

	cout << "\t\t - alphabet:\t" << this->_ab << endl;
	cout << "\t\t - buckets_num:\t" << this->_buckets_num << endl;
	cout << "\t\t - block_size:\t" << this->_block_size << endl;
	cout << "\t\t - dseq_len:\t" << this->_dseq_len << endl;
	cout << "\t\t - qthr:\t" << this->_qthr << endl;
	cout << "\t\t - qlen:\t" << this->_qlen << endl;
	cout << "\t\t - flen:\t" << this->_flen << endl;
	cout << "\t\t - fnum:\t" << this->_fnum << endl;
	cout << "\t\t - part_len:\t" << this->_part_len << endl;
	cout << "\t\t - part_shift:\t" << this->_part_shift << endl;

	this->_hash_table.rehash(this->_buckets_num);

	fin >> cur_token;
	string hkey;
	string hvalues;
	string hvalue;
	unsigned long parts_num;
	//	parts_num = 2*ceil((float)this->_dseq_len/(float)this->_part_len)-1;
	parts_num = this->_dseq_len - this->_part_len + 1; // NEW!
	while (!fin.eof())
	{
		HitmapLightBucket tmp_bucket;

		if (cur_token.substr(0, 1) == ">")
		{
			tmp_bucket._hitmap = new SparseBitset(parts_num, this->_block_size); // allocate memory

			fin >> cur_token; // cout<<"token="<<cur_token<<endl;

			std::size_t delim_pos = cur_token.find(":");
			hkey = cur_token.substr(0, delim_pos);
			hvalues = cur_token.substr(delim_pos + 1).c_str();
			delim_pos = hvalues.find(",");
			if (delim_pos != string::npos) // only one position
			{
				while (delim_pos != string::npos)
				{
					hvalue = hvalues.substr(0, delim_pos); // cout<<"to insert: "<<hvalue<<endl;
					hvalues = hvalues.substr(delim_pos + 1);
					tmp_bucket._hitmap->set(atoi(hvalue.c_str()));
					delim_pos = hvalues.find(",");
				}
			}
			tmp_bucket._hitmap->set(atoi(hvalues.c_str()));
			this->_hash_table.insert(std::make_pair /*<std::string,HitmapLightBucket>*/ (hkey, tmp_bucket));

			//		cout<<"hkey:"<<hkey<<endl;
			//		cout<<"=>"<<this->_hash_table.find(hkey)->second._hitmap->count()<<endl;
			//		delete tmp_bucket._hitmap; //free memory
			//		cout<<"=>"<<this->_hash_table.find(hkey)->second._hitmap->count()<<endl;
		}
		fin >> cur_token;
	}

	fin.close();
	cout << "\t * Done!" << endl;
}

unsigned long HitmapLightHash::getBucketsNum(void)
{
	return this->_buckets_num;
}

unsigned long HitmapLightHash::getFNum(void)
{
	return this->_fnum;
}

unsigned long HitmapLightHash::getQthr(void)
{
	return this->_qthr;
}

unsigned long HitmapLightHash::getFLen(void)
{
	return this->_flen;
}

string HitmapLightHash::getAb(void)
{
	return this->_ab;
}

vector<int> HitmapLightHash::getWindows(void)
{
	return this->_wins;
}

vector<int> HitmapLightHash::getResults(void)
{
	this->keepUniqueResults();
	return this->_results;
}

void HitmapLightHash::keepUniqueResults(void)
{
	vector<int>::iterator tmp_it;
	sort(this->_results.begin(), this->_results.end());
	tmp_it = unique(this->_results.begin(), this->_results.end());
	this->_results.resize(std::distance(this->_results.begin(), tmp_it));
}

void HitmapLightHash::setVerificationStatistics(string &qseq, string ab)
{
	int ver_areas_num;
	int ver_size;
	ver_size = 0;
	ver_areas_num = 0;
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	if (this->_wins.empty()) // in case of no results
	{
		this->_stats._ver_areas_num = 0;
		this->_stats._non_merged_areas = 0;
		this->_stats._data_size = this->_dseq_len;
		this->_stats._ver_size = 0;
		this->_stats._ver_size_percentage = 0;
	}
	else // otherwise
	{
		// verify the windows
		vector<int>::iterator cur_win; // the current verification window
		cur_win = this->_wins.begin();
		ver_area_start = (*cur_win);
		ver_area_end = ver_area_start + this->_part_len;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
		while (cur_win != this->_wins.end())
		{
			if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
			{
				ver_areas_num++;
				ver_size += (ver_area_end - ver_area_start + 1);
				//			cout<<"ar. start: "<<ver_area_start<<", ar. end: "<<ver_area_end<<endl;

				ver_area_start = (*cur_win);
			}
			ver_area_end = (*cur_win) + this->_part_len;
			if (ver_area_end >= this->_dseq_ptr->length())
				ver_area_end = last_pos;
			cur_win++;
		}
		ver_areas_num++;
		ver_size += (ver_area_end - ver_area_start + 1);
		//	cout<<"ar. start: "<<ver_area_start<<", ar. end: "<<ver_area_end<<endl;

		this->_stats._ver_areas_num = ver_areas_num;
		this->_stats._non_merged_areas = (double)this->_wins.size();
		this->_stats._data_size = this->_dseq_len;
		this->_stats._ver_size = ver_size;
		this->_stats._ver_size_percentage = (double)this->_stats._ver_size / (double)this->_stats._data_size;
	}
	cout << "dlen=" << this->_dseq_ptr->length() << ", fnum=" << this->_fnum << ", flen=" << this->_flen << ", qthr=" << this->_qthr << ", ab=" << ab.length() << endl;
	this->_stats._estimated_candidates = HitmapHash::estimateVerAreas(this->_dseq_ptr->length(), this->_fnum, this->_flen, this->_qthr, ab.length());
}

VerificationStatistics HitmapLightHash::getVerificationStatistics(void)
{
	return this->_stats;
}

void HitmapLightHash::findWindows(string &qseq)
{
	unsigned long k;
	unsigned long parts_num;
	//	parts_num = 2*ceil((float)this->_dseq_len/(float)this->_part_len)-1;
	parts_num = this->_dseq_len - this->_part_len + 1; // NEW!

	this->_wins.clear();
	this->_results.clear();

	SparseBitset hitmap(parts_num, this->_block_size);
	string fragment;

	for (k = 0; k < this->_fnum; k++)
	{
		fragment = qseq.substr(k * this->_flen, this->_flen); // get the current fragment

		if (this->_hash_table.find(fragment) != this->_hash_table.end())
		{ // cout<<fragment<<":"<<this->_hash_table.find(fragment)->first<<endl;
			//		int xx; xx = this->_hash_table.find(fragment)->second._hitmap->count();
			//		cout<<"hitmap"<<k<<": "<<xx<<endl;
			hitmap |= this->_hash_table.find(fragment)->second._hitmap;
		}
	}

	unsigned long tmp_1;
	unsigned long blocks_num;
	unsigned long block_size;
	unsigned long bit_id;
	unsigned long win_start;
	block_size = hitmap.getBlockSize();
	blocks_num = hitmap.getBlocksNum();

	// Calculate the windows that need to be verified (based on this eq. class).
	for (k = 0; k < blocks_num; k++) // get a block each time
	{
		if (hitmap.testBlock(k)) // check that there is at least one 1 in the block
		{
			for (tmp_1 = 0; tmp_1 < block_size; tmp_1++) // check one by one the bits
			{
				if (hitmap.testInNonEmptyBlock(k, tmp_1))
				{
					bit_id = k * block_size + tmp_1;
					win_start = bit_id * this->_part_shift;
					this->_wins.push_back(win_start); // cout<<"WIN="<<win_start<<endl;
													  /*						if( bit_id<this->_tsetflags_new->size() ) //otherwise it is one of the padding bits
																			  {
																				  win_start = bit_id*this->_part_len+i[0];
								  
																				  //get the window verification
																				  this->_wins.push_back(win_start);
																			  }
								  
																			  this->_tsetflags_new->flipNonEmptyBlock(k,tmp_1);
																			  if( this->_tsetflags_new->runNoneInBlock(k) )
																				  break;*/
				}
			}
		}
	}
	// hitmap.print();
}

void HitmapLightHash::verifyWindows(string &qseq)
{
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	if (this->_wins.empty())
		return; // no windows - no results

	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	ver_area_start = (*cur_win);
	ver_area_end = ver_area_start + this->_part_len;
	if (ver_area_end >= this->_dseq_ptr->length())
		ver_area_end = last_pos;
	cur_win++;
	while (cur_win != this->_wins.end())
	{
		if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
		{
			dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
			ver_area_start = (*cur_win);
		}
		ver_area_end = (*cur_win) + this->_part_len;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
	}
	dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
}

void HitmapLightHash::keepUniqueWindows(void)
{
	if (!this->_wins.empty())
	{
		vector<int>::iterator tmp_it;
		sort(this->_wins.begin(), this->_wins.end());
		tmp_it = unique(this->_wins.begin(), this->_wins.end());
		this->_wins.resize(std::distance(this->_wins.begin(), tmp_it));
	}
}

//=====
// Declaration of static variables.

unsigned long HitmapBucket::_bs_num;
unsigned long HitmapBucket::_bs_size;
unsigned long HitmapBucket::_block_size;
SparseBitset *HitmapBucket::_zero_bitset = NULL;

//=====
// HitmapBucket methods.

HitmapBucket::HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size)
{
	// Initialise the static variables.
	HitmapBucket::_bs_num = bs_num;
	HitmapBucket::_bs_size = bs_size;
	HitmapBucket::_block_size = block_size;

	if (HitmapBucket::_zero_bitset == NULL)
	{
		HitmapBucket::_zero_bitset = new SparseBitset(HitmapBucket::_bs_size, HitmapBucket::_block_size);
	}
}

HitmapBucket::~HitmapBucket(void)
{
}

/*
void HitmapBucket::setHit(unsigned long bit_id, unsigned long first_bitset, unsigned long bitsets_to_set_num)
{
	bool col_exists;
	col_exists = false;
	vector<BucketCol>::iterator cur_col; //the current column

	// Create a new column if needed.
	cur_col = this->_bucket_columns.begin();
	while( cur_col!=this->_bucket_columns.end() )
	{
		if( cur_col->_col_id==bit_id ) //if there is already a column for this bit_id
		{
			col_exists = true;
			break;
		}
		else if( cur_col->_col_id>bit_id )
		{
			break;
		}
		cur_col++;
	}
	if( !col_exists )
	{
		BucketCol tmp_col;
		tmp_col._col_id = bit_id;
		tmp_col._col_cnt.resize(HitmapBucket::_bs_num);
		this->_bucket_columns.push_back(tmp_col);
	}

	// Make the iterator pointing at the column that corresponds to bit_id.
	cur_col = this->_bucket_columns.begin();
	while( cur_col!=this->_bucket_columns.end() )
	{
		if( cur_col->_col_id==bit_id ) //if there is already a column for this bit_id
		{
			break;
		}
		cur_col++;
	}

	// Set the bits.
	int set_bits_num;
	set_bits_num = 0;
	for(int cur_bitset=first_bitset; set_bits_num<bitsets_to_set_num; cur_bitset--)
	{
		if( cur_bitset<0 )
			cur_bitset = HitmapBucket::_bs_num-1;

		//set bit
		cur_col->_col_cnt.set(cur_bitset);

		set_bits_num++;
	}
}*/

void HitmapBucket::setHit(unsigned long pos, unsigned long part_len, unsigned long flen)
{
	unsigned long i;
	unsigned long j;
	vector<BucketCol>::iterator cur_col; // the current column
	BucketCol tmp_col;
	bool col_exists;

	// Find the part column in the first partition. Note that any part column in the next
	// partitions cannot have a larger index, since it is shifted to the right of the
	// column of the first partition. Eg if the part column in the first partition is the
	// 2nd column, then the part columns of the next partitions that contain the same
	// gram are in the 1st or 2nd column. Cannot be in the 3rd column.
	unsigned long part_col_in_first_partition;
	part_col_in_first_partition = floor(pos / part_len);

	// Only one column to the left (or the same column) can have parts to contain the
	// same gram...
	unsigned long first;
	if (part_col_in_first_partition < 1)
		first = 0;
	else
		first = part_col_in_first_partition - 1;

	for (j = first; j <= part_col_in_first_partition; j++)
	{
		for (i = 0; i < HitmapBucket::_bs_num; i++)
		{
			if ((i + j * part_len) <= pos && (i + (j + 1) * part_len - flen) >= pos)
			{
				// HIT!
				col_exists = false;

				// Create a new column if needed.
				cur_col = this->_bucket_columns.begin();
				while (cur_col != this->_bucket_columns.end())
				{
					if (cur_col->_col_id == j) // if there is already a column for this bit_id
					{
						col_exists = true;
						break;
					}
					else if (cur_col->_col_id > j)
					{
						break;
					}
					cur_col++;
				}
				if (!col_exists)
				{
					tmp_col._col_id = j;
					tmp_col._col_cnt.resize(HitmapBucket::_bs_num);
					this->_bucket_columns.push_back(tmp_col);
				}

				// Make the iterator pointing at the column that corresponds to bit_id.
				cur_col = this->_bucket_columns.begin();
				while (cur_col != this->_bucket_columns.end())
				{
					if (cur_col->_col_id == j) // if there is already a column for this bit_id
					{
						break;
					}
					cur_col++;
				}

				cur_col->_col_cnt.set(i);
			}
		}
	}
}

/*
void HitmapBucket::setHit_quick(unsigned long pos, unsigned long part_len, unsigned long flen)
{
	unsigned long i;
	unsigned long j;
	vector<BucketCol>::iterator cur_col; //the current column
	BucketCol tmp_col;
	tmp_col._col_cnt.resize(HitmapBucket::_bs_num);
	bool col_exists;

	//Find the part column in the first partition. Note that any part column in the next
	//partitions cannot have a larger index, since it is shifted to the right of the
	//column of the first partition. Eg if the part column in the first partition is the
	//2nd column, then the part columns of the next partitions that contain the same
	//gram are in the 1st or 2nd column. Cannot be in the 3rd column.
	unsigned long part_col_in_first_partition;
	part_col_in_first_partition = floor(pos/part_len);

	//Only one column to the left (or the same column) can have parts to contain the
	//same gram...
	unsigned long first;
	if( part_col_in_first_partition<1 )
		first = 0;
	else
		first = part_col_in_first_partition-1;

	unsigned long tmp1,tmp2,tmp3; // temporal variables


	for( j=first; j<=part_col_in_first_partition; j++)
	{
		tmp1 = j*part_len; //calculate part of the condition A, only once, to avoid redundant calculations
		if(pos<tmp1)
			tmp1 = pos;
		tmp2 = (j+1)*part_len-flen; //calculate part of the condition A
		if( pos>=tmp2 && (pos-tmp2)>=HitmapBucket::_bs_num )
			tmp2 = pos-HitmapBucket::_bs_num-1;
		if( tmp1 > tmp2 )
			tmp1 = tmp2;
		cout<<"pos="<<pos<<", tmp1="<<tmp1<<", tmp2="<<tmp2<<", bs_num="<<HitmapBucket::_bs_num<<endl;

//a		for(i=0; i<HitmapBucket::_bs_num; i++)
		for( tmp3=tmp1; tmp3<=tmp2; tmp3++)
		{
//a			tmp3 = pos-i;

//			if( (i+tmp1)<=pos && (i+tmp2)>=pos ) //condition A
//a			if( tmp1<=tmp3 && tmp2>=tmp3 )
//a			{
				//HIT!
				col_exists = false;

				// Create a new column if needed.
				cur_col = this->_bucket_columns.begin();
				while( cur_col!=this->_bucket_columns.end() )
				{
					if( cur_col->_col_id==j ) //if there is already a column for this bit_id
					{
						col_exists = true;
						break;
					}
					else if( cur_col->_col_id>j )
					{
						break;
					}
					cur_col++;
				}
				if( !col_exists )
				{
					tmp_col._col_id = j;
					//tmp_col._col_cnt.resize(HitmapBucket::_bs_num);
					this->_bucket_columns.push_back(tmp_col);
					cur_col = this->_bucket_columns.end()-1;
				}

//a				cur_col->_col_cnt.set(i);

//				if( pos>=tmp3 && (pos-tmp3)<HitmapBucket::_bs_num){ cout<<"pos="<<pos<<", tmp3="<<tmp3<<endl;
					cur_col->_col_cnt.set(pos-tmp3);  // }

//a			}
		}
	}
}*/

void HitmapBucket::setHit_quick(unsigned long pos, unsigned long part_len, unsigned long flen)
{
	unsigned long i;
	unsigned long j;
	vector<BucketCol>::iterator cur_col; // the current column
	BucketCol tmp_col;
	tmp_col._col_cnt.resize(HitmapBucket::_bs_num);
	bool col_exists;
	bool delete_new_col;
	delete_new_col = true;

	// Find the part column in the first partition. Note that any part column in the next
	// partitions cannot have a larger index, since it is shifted to the right of the
	// column of the first partition. Eg if the part column in the first partition is the
	// 2nd column, then the part columns of the next partitions that contain the same
	// gram are in the 1st or 2nd column. Cannot be in the 3rd column.
	unsigned long part_col_in_first_partition;
	part_col_in_first_partition = floor(pos / part_len);

	// Only one column to the left (or the same column) can have parts to contain the
	// same gram...
	unsigned long first;
	if (part_col_in_first_partition < 1)
		first = 0;
	else
		first = part_col_in_first_partition - 1;

	unsigned long part_start, last_frag_in_part_start, tmp3; // temporal variables
	unsigned long distance_from_last_frag;

	for (j = first; j <= part_col_in_first_partition; j++)
	{
		part_start = j * part_len;							 // get start of current part
		last_frag_in_part_start = (j + 1) * part_len - flen; // get start position of last fragment in the part

		if (pos >= last_frag_in_part_start)
			distance_from_last_frag = pos - last_frag_in_part_start;
		else
			distance_from_last_frag = 0;

		// test pos start
		col_exists = false;

		// Create a new column if needed.
		cur_col = this->_bucket_columns.begin();
		while (cur_col != this->_bucket_columns.end())
		{
			if (cur_col->_col_id == j) // if there is already a column for this bit_id
			{
				col_exists = true;
				break;
			}
			else if (cur_col->_col_id > j)
			{
				break;
			}
			cur_col++;
		}
		if (!col_exists)
		{
			tmp_col._col_id = j;
			this->_bucket_columns.push_back(tmp_col);
			cur_col = this->_bucket_columns.end() - 1;
		}
		// test pos end

		for (i = distance_from_last_frag; i < HitmapBucket::_bs_num; i++)
		{
			tmp3 = pos - i;
			if (part_start > tmp3) // next loops will also have the same problem...
				break;

			// from here....
			delete_new_col = false;
			cur_col->_col_cnt.set(i);
		}

		if ((delete_new_col == true) && (col_exists == false))
			this->_bucket_columns.pop_back();
	}
}

void HitmapBucket::lsbShiftBitset(SparseBitset *result, unsigned long bs_id, unsigned long shift_by)
{
	vector<BucketCol>::iterator cur_col;
	int bit_to_set;

	(*result) = (*HitmapBucket::_zero_bitset);
	cur_col = this->_bucket_columns.begin();
	while (cur_col != this->_bucket_columns.end())
	{ // cout<<"\t cur_col:"<<cur_col->_col_cnt<<endl;
		//	cout<<"\t get bit: "<<bs_id<<endl;
		if (cur_col->_col_cnt.test(bs_id))
		{
			bit_to_set = cur_col->_col_id - shift_by; // find the bit id if shifted
			//			cout<<"col:"<<cur_col->_col_id<<", shift-by:"<<shift_by<<endl;
			if (bit_to_set >= 0)
				result->set(bit_to_set);
		}
		cur_col++;
	}
	//	cout<<"result: "<<endl;
	//	result->print();
}

string HitmapBucket::serialize(void)
{
	string serialized;
	string tmp;

	vector<BucketCol>::iterator cur_col;
	cur_col = this->_bucket_columns.begin();
	while (cur_col != this->_bucket_columns.end())
	{
		to_string(cur_col->_col_cnt, tmp);
		serialized += itoa(cur_col->_col_id) + ":" + tmp + " ";
		cur_col++;
	}

	if (serialized == "")
		serialized = "PROBLEM SERIALIZE EMPTY BUCKET ";

	serialized = serialized.substr(0, serialized.length() - 1); // remove space at the end

	return serialized;
}

string HitmapBucket::serializeCompact(void)
{
	string serialized;

	// bitset
	string tmp;

	// current symbol
	char cur_sym;

	// current symbol number
	unsigned long cur_sym_num;
	string tmp_ser;

	vector<BucketCol>::iterator cur_col;
	cur_col = this->_bucket_columns.begin();
	while (cur_col != this->_bucket_columns.end())
	{
		to_string(cur_col->_col_cnt, tmp);
		// cout << "tmp: " << tmp << endl;
		// compress the bitset string
		cur_sym = tmp[0];
		cur_sym_num = 1;
		tmp_ser = "";
		// cout << "init: " << tmp << endl;
		for (unsigned long i = 1; i < tmp.length(); i++)
		{
			if (cur_sym == tmp[i])
			{
				cur_sym_num++;
			}
			else
			{
				tmp_ser += cur_sym + itoa(cur_sym_num);
				cur_sym = tmp[i];
				cur_sym_num = 1;
			}
		}
		// cout << "compr: " << tmp_ser << endl;
		serialized += itoa(cur_col->_col_id) + ":" + tmp_ser + " ";
		cur_col++;
	}

	if (serialized == "")
		serialized = "PROBLEM SERIALIZE EMPTY BUCKET ";
	// cout << "serialized: " << serialized << endl;
	serialized = serialized.substr(0, serialized.length() - 1); // remove space at the end

	return serialized;
}

void HitmapBucket::addColumn(string column)
{
	// cout << "Column: " << column << endl;
	BucketCol tmp_col;
	std::size_t delim_pos = column.find(":");
	tmp_col._col_id = atoi(column.substr(0, delim_pos).c_str());
	// cout << "Substring: " << string(column.substr(delim_pos + 1)) << endl;
	tmp_col._col_cnt = dynamic_bitset<>(string(column.substr(delim_pos + 1)));
	this->_bucket_columns.push_back(tmp_col);
}

//=====
// HitmapHash methods.

HitmapHash::HitmapHash(string *dseq_ptr, unsigned long qthr, unsigned long part_shift, unsigned long flen, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, unsigned long buck_groups_num) : StrHash<HitmapBucket>(flen, buckets_num, buck_groups_num)
{
	string cur_str; // a string bucket
	unsigned long cur_bucket;

	this->_dseq_ptr = dseq_ptr; // get the pointer to the data sequence
	this->_dseq_len = dseq_ptr->length();
	this->_qthr = qthr;															   // get the query threshold
	this->_flen = flen;															   // get the length of the fragments
	this->_fnum = fnum;															   // get the number of fragments
	this->_part_len = this->_flen + 2 * this->_qthr;							   // get the length of the parts
	this->_part_shift = part_shift;												   // get how many positions is shifted the one part for the next
	this->_partition_num = this->_part_len;										   // get the number of partitions
	this->_partition_size = ceil((float)this->_dseq_len / (float)this->_part_len); // get how many parts are in each partition
	this->_counters = NULL;														   // Initially no counters. We specify counters by calling initCounterBitsets().
	this->_block_size = block_size;
	if (this->_block_size > this->_partition_size)
	{
		cout << "WARNING - [Creating hitmap hash] The sparse bitset blocks are defined to be larger than the hitmaps!" << endl;
		// debug //
		cout << "\t * part size = " << this->_part_len << endl;
		// debug //
		cout << "\t * hitmap size = " << this->_partition_size << endl;
		cout << "\t * block size = " << this->_block_size << endl;
	}

	cout << "flen: " << this->_flen << endl;
	cout << "qthr: " << this->_qthr << endl;
	cout << "part_len: " << this->_part_len;
	this->_tsetflags_new = new SparseBitset(this->_partition_size, this->_block_size);
	this->_blocks_num = this->_tsetflags_new->getBlocksNum();

	// In order to achieve the same number of parts in all partitions we extend dseq by adding symbols at the end. These extra
	// symbols are not part of the alphabet.

	string tmp_str(this->_part_len - 1, '$');
	this->_dseq_len = this->_dseq_ptr->length(); // update the length of the data sequence
	// attention!! this may create problem in case of consecutive index creation on the same data seq

	cout << endl;
	cout << "\t"
		 << "- Bucket analysis started..." << endl;
	// Scan dseq to fill the hash table buckets
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{
		// Get the corresponding dseq subsequence
		cur_str = this->_dseq_ptr->substr(p, this->_flen);
		// cout << "cur_str: " << cur_str << ", flen: " << this->_flen << endl;
		// Test if the bucket group is going to have content.
		this->setGroupStatus(this->_hash_func(cur_str) % this->_buckets_num);
	}
	cout << "\t"
		 << "- Bucket analysis ended..." << endl;

	cout << "\t"
		 << "- Bucket calculation started..." << endl;
	this->calcBuckets();
	cout << "\t"
		 << "- Bucket calculation ended..." << endl;
}

HitmapHash::HitmapHash(void)
{
}

HitmapHash::~HitmapHash(void)
{
	HitmapBucket::freeZeroBitset();
	for (unsigned long i = 0; i < this->_buck_groups_num; i++)
	{
		this->deleteGroup(i);
	}
	delete[] this->_buckets;
	delete this->_counters;
	delete this->_tsetflags_new;
}

unsigned long HitmapHash::getBucketsNum(void)
{
	return this->_buckets_num;
}

unsigned long HitmapHash::getFNum(void)
{
	return this->_fnum;
}

unsigned long HitmapHash::getQthr(void)
{
	return this->_qthr;
}

unsigned long HitmapHash::getFLen(void)
{
	return this->_flen;
}

string HitmapHash::getAb(void)
{
	return this->_ab;
}

void HitmapHash::store(string output_file, string ab, double constr_time)
{
	ofstream fout;
	fout.open(output_file.c_str());

	cout << "\t * Producing file meta-data..." << endl;

	// Write time related metadata.
	time_t rawtime;
	time(&rawtime);
	fout << "@created:\t" << ctime(&rawtime);
	fout << "@construction:\t" << constr_time << " sec" << endl;
	fout << "@fragment_length:\t" << this->_flen << endl;
	fout << "@fragments:\t" << this->_fnum << endl;
	fout << "@err_threshold:\t" << this->_qthr << endl;
	fout << "@buckets:\t" << this->_buckets_num << endl;
	fout << "@block_size:\t" << this->_block_size << endl;
	fout << "@alphabet:\t" << ab << endl;

	// Write server related metadata.
	char hostname[HOST_NAME_MAX];
	int result;
	result = gethostname(hostname, HOST_NAME_MAX);
	if (result)
	{
		cout << "=> ERROR: Problem while reading system info." << endl;
		return;
	}
	fout << "@server:\t" << hostname << endl;

	// Write size related metadata.
	fout << "@size:\t\t" << this->byteSize() << " bytes" << endl;
	fout << "@data:" << endl;

	cout << "\t * Saving idx..." << endl;
	for (int i = 0; i < this->_buckets_num; i++)
	{ // cout<<"buck $"<<i<<" ("<<this->_buckets[i]<<") out of "<<this->_buckets_num<<endl;
		if (this->_buckets[0][i] != 0)
		{
			fout << ">" << i << " ";

			fout << this->_buckets[0][i]->serialize();

			fout << endl;
		}
	}

	cout << "\t * Done!" << endl;
}

void HitmapHash::load(string input_file, string *dseq_ptr)
{
	ifstream fin;
	fin.open(input_file.c_str());
	if (fin.fail())
	{
		cout << "=> ERROR: Cannot open '" << input_file << "'. No such file." << endl
			 << endl;
		return;
	}
	string cur_token;

	cout << "\t * Reading file meta-data..." << endl;
	fin >> cur_token;
	while (cur_token != "@fragment_length:")
	{
		fin >> cur_token;
	}
	fin >> this->_flen;
	while (cur_token != "@fragments:")
	{
		fin >> cur_token;
	}
	fin >> this->_fnum;
	fin >> cur_token;
	while (cur_token != "@err_threshold:")
	{
		fin >> cur_token;
	}
	fin >> this->_qthr;
	fin >> cur_token;
	while (cur_token != "@buckets:")
	{
		fin >> cur_token;
	}
	fin >> this->_buckets_num;
	fin >> cur_token;
	while (cur_token != "@block_size:")
	{
		fin >> cur_token;
	}
	fin >> this->_block_size;
	fin >> cur_token;
	while (cur_token != "@alphabet:")
	{
		fin >> cur_token;
	}
	fin >> this->_ab;

	cout << "\t\t - alphabet:\t" << this->_ab << endl;
	cout << "\t\t - buckets_num:\t" << this->_buckets_num << endl;

	this->_key_len = this->_flen;					 // Get the length of the key-strings.
	this->_part_shift = this->_flen;				 // get how many positions is shifted the one part for the next
	this->_part_len = this->_flen + 2 * this->_qthr; // get the length of the parts
	this->_partition_num = this->_part_len;			 // get the number of partitions
	this->_buckets_per_group = this->_buckets_num;
	this->_buck_groups_num = 1;

	this->_buckets = new HitmapBucket **[1]; // Allocate space for the group of buckets.
	this->_buckets[0] = new HitmapBucket *[this->_buckets_num];
	for (int i = 0; i < this->_buckets_num; i++)
		this->_buckets[0][i] = 0;

	this->_dseq_ptr = dseq_ptr;													   // get the pointer to the data sequence
	this->_dseq_len = this->_dseq_ptr->length();								   // get the length of the data sequence
	this->_partition_size = ceil((float)this->_dseq_len / (float)this->_part_len); // get how many parts are in each partition
	if (this->_block_size > this->_partition_size)
	{
		cout << "WARNING - [Creating hitmap hash] The sparse bitset blocks are defined to be larger than the hitmaps!" << endl;
		cout << "\t * hitmap size = " << this->_partition_size << endl;
		cout << "\t * block size = " << this->_block_size << endl;
	}
	this->_counters = NULL; // Initially no counters. We specify counters by calling initCounterBitsets().
	this->_tsetflags_new = new SparseBitset(this->_partition_size, this->_block_size);
	this->_blocks_num = this->_tsetflags_new->getBlocksNum();

	//<DEPRICATED>
	this->_group_status = new dynamic_bitset<>(this->_buck_groups_num);
	string cur_str; // a string bucket
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{
		// Get the corresponding dseq subsequence
		cur_str = this->_dseq_ptr->substr(p, this->_flen);

		// Test if the bucket group is going to have content.
		this->setGroupStatus(this->_hash_func(cur_str) % this->_buckets_num);
	}
	//</DEPRICATED>

	// In order to achieve the same number of parts in all partitions we extend dseq by adding symbols at the end. These extra
	// symbols are not part of the alphabet.
	string tmp_str(this->_part_len - 1, '$');
	this->_dseq_len = this->_dseq_ptr->length(); // update the length of the data sequence
	// attention!! this may create problem in case of consecutive index creation on the same data seq

	cout << "\t * Reading data and loading idx..." << endl;

	while (cur_token.substr(0, 1) != ">")
	{
		fin >> cur_token;
	}

	unsigned long cur_bucket;
	HitmapBucket **tmp;
	while (!fin.eof())
	{
		if (cur_token.substr(0, 1) == ">") // new entry (bucket) token
		{
			cur_token = cur_token.substr(1, cur_token.length() - 1);
			cur_bucket = atoi(cur_token.c_str());
			// add new vector?

			tmp = this->getBucketPtrRef(cur_bucket);
			(*tmp) = new HitmapBucket(this->_partition_num, this->_partition_size, this->_block_size);
		}
		else
		{
			// push back to vector the new ...
			tmp = this->getBucketPtrRef(cur_bucket);
			(*tmp)->addColumn(cur_token);
		}

		fin >> cur_token;
	}
	cout << "\t * Done!" << endl;
}

/**
 * This function estimates the number of candidate positions for Hitmap.
 *
 * @param dlen The length of data sequence.
 * @param fnum The number of query fragments.
 * @param flen The length of each query fragment.
 * @param qthr The query (alignment) threshold.
 * @param ab_size The size of the alphabet used.
 *
 * @return The estimated number of candidates (based on Eq. 5 of my Google doc).
 *
 * @author Thanasis Vergoulis
 */
double HitmapHash::estimateCandidates(int dlen, int fnum, int flen, int qthr, int ab_size)
{
	double cand_num; // The number of candidate positions (by estimation)

	double sum;
	sum = 0;

	for (int y = fnum - qthr; y < fnum; y++)
	{
		sum += (binom_large(fnum, y) * pow((double)(((double)(2 * qthr + 1)) / ((double)(pow((double)ab_size, (double)flen)))), (double)y));
	}

	cand_num = dlen * sum; // this is our estimation for candidates

	return cand_num;
}

double HitmapHash::estimateVerAreas(int dlen, int fnum, int flen, int qthr, int ab_size)
{
	return ((double)HitmapHash::estimateCandidates(dlen, fnum, flen, qthr, ab_size) / (double)(qthr + 1));
}

double HitmapHash::estimateVerCost(unsigned long dlen, unsigned long qlen, unsigned long qthr, unsigned long phi, unsigned long ab_size, double cost_dp, bool random_queries)
{
	unsigned long flen;
	double cand_num;
	flen = floor((double)qlen / (double)phi);
	cand_num = HitmapHash::estimateVerAreas(dlen, phi, flen, qthr, ab_size);
	if (!random_queries && cand_num < 1) // if not random, then query was taken from the data, thus at least one area is present
		cand_num = 1;

	if (((double)dlen / (double)cand_num) < ((double)(qlen + 2 * qthr))) // if all the data must be verified
		return ((double)dlen * (double)qlen * cost_dp);
	else
		return ((double)(qlen + 2 * qthr)) * ((double)qlen) * cost_dp * cand_num;
}

double HitmapHash::estimateFiltCost(unsigned long dlen, unsigned long qlen, unsigned long qthr, unsigned long phi, double cost_prob, double cost_bit)
{
	unsigned long plen; // part length
	unsigned long flen; // fragment length

	flen = floor((double)qlen / (double)phi);
	plen = flen + 2 * qthr;

	return ((double)plen) * ((double)phi * cost_prob + ((double)2 * phi * (1 + log2(phi + 1)) + log2(phi + 1)) * ((double)ceil((double)dlen / (double)plen)) * cost_bit);
}

void HitmapHash::calcBuckets(void)
{
	string cur_str; // a string buffer
	unsigned long cur_bucket;

	// In order to achieve the same number of parts in all partitions we extend dseq by adding symbols at the end. These extra
	// symbols are not part of the alphabet.
	string tmp_str(this->_part_len - 1, '$');
	// attention!! this may create problem in case of consecutive index creation on the same data seq
	HitmapBucket **tmp;

	// Scan dseq to fill the hash table buckets
	cout << "iterations: " << this->_dseq_len - this->_flen << endl;
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{
		cout << "p=" << p << " (" << ((float)100 * (float)p / (float)(this->_dseq_len - this->_flen)) << "%)" << endl;
		// cout << "P: " << p << endl;
		// Get the corresponding dseq subsequence

		cur_str = this->_dseq_ptr->substr(p, this->_flen);

		// Get the current bucket id based on the hashing function.
		cur_bucket = (this->_hash_func(cur_str) % this->_buckets_num);
		tmp = this->getBucketPtrRef(cur_bucket);
		// If this is the first hit for this bucket.
		if ((*tmp) == 0)
		{
			// cout << "	" << cur_str << endl;
			// sleep(1);
			(*tmp) = new HitmapBucket(this->_partition_num, this->_partition_size, this->_block_size);
		}

		// Set the corresponding bits.
		//		(*tmp)->setHit(p,this->_part_len,this->_flen);
		(*tmp)->setHit_quick(p, this->_part_len, this->_flen);
	}
}

void HitmapHash::extraStatistics(void)
{
	// TODO
}

vector<int> HitmapHash::getWindows(void)
{
	return this->_wins;
}

vector<int> HitmapHash::getResults(void)
{
	this->keepUniqueResults();
	return this->_results;
}

void HitmapHash::keepUniqueResults(void)
{
	vector<int>::iterator tmp_it;
	sort(this->_results.begin(), this->_results.end());
	tmp_it = unique(this->_results.begin(), this->_results.end());
	this->_results.resize(std::distance(this->_results.begin(), tmp_it));
}

void HitmapHash::initCounterBitsets(unsigned long t_max)
{
	this->_counters = new CounterBitsets(this->_partition_size, t_max, this->_block_size);
}

void HitmapHash::setVerificationStatistics(string &qseq, string ab)
{
	int ver_areas_num;
	int ver_size;
	ver_size = 0;
	ver_areas_num = 0;
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	if (this->_wins.empty()) // in case of no results
	{
		this->_stats._ver_areas_num = 0;
		this->_stats._non_merged_areas = 0;
		this->_stats._data_size = this->_dseq_len;
		this->_stats._ver_size = 0;
		this->_stats._ver_size_percentage = 0;
	}
	else // otherwise
	{
		// verify the windows
		vector<int>::iterator cur_win; // the current verification window
		cur_win = this->_wins.begin();
		ver_area_start = (*cur_win);
		ver_area_end = ver_area_start + qseq.length() + this->_qthr - 1;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
		while (cur_win != this->_wins.end())
		{
			if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
			{
				ver_areas_num++;
				ver_size += (ver_area_end - ver_area_start + 1);
				// cout<<"ar. start: "<<ver_area_start<<", ar. end: "<<ver_area_end<<endl;

				ver_area_start = (*cur_win);
			}
			ver_area_end = (*cur_win) + qseq.length() + this->_qthr - 1;
			if (ver_area_end >= this->_dseq_ptr->length())
				ver_area_end = last_pos;
			cur_win++;
		}
		ver_areas_num++;
		ver_size += (ver_area_end - ver_area_start + 1);
		// cout<<"ar. start: "<<ver_area_start<<", ar. end: "<<ver_area_end<<endl;

		this->_stats._ver_areas_num = ver_areas_num;
		this->_stats._non_merged_areas = (double)this->_wins.size();
		this->_stats._data_size = this->_dseq_len;
		this->_stats._ver_size = ver_size;
		this->_stats._ver_size_percentage = (double)this->_stats._ver_size / (double)this->_stats._data_size;
	} // cout<<"dlen="<<this->_dseq_ptr->length()<<", fnum="<<this->_fnum<<", flen="<<this->_flen<<", qthr="<<this->_qthr<<", ab="<<ab.length()<<endl;
	this->_stats._estimated_candidates = HitmapHash::estimateVerAreas(this->_dseq_ptr->length(), this->_fnum, this->_flen, this->_qthr, ab.length());
}

VerificationStatistics HitmapHash::getVerificationStatistics(void)
{
	return this->_stats;
}

void HitmapHash::findWindows(string &qseq)
{
	unsigned long alpha; // Variable to store the current equivalence class.
	unsigned long k;
	unsigned long tmp_1;  // auxiliary variable
	unsigned long bit_id; // auxiliary variable
	string *fragments;	  // Array containing the fragments of the qseq.
	string tmp_str;

	unsigned long win_start; // the start position of a verification window
	unsigned long win_end;	 // the end position of a verification window

	unsigned long *i;
	unsigned long *j;
	unsigned long *Dj;

	SparseBitset **hitmaps_new;
	unsigned long *bucket_keys; // array containing the keys for the needed hash buckets

	HitmapBucket *tmp;

	// Check that initCounterBitsets was executed.
	if (this->_counters == NULL)
	{
		cout << "ERROR -- [Hitmap searching] Run initCounterBitsets before searching..." << endl;
		return;
	}

	// this->_wins.clear();
	// this->_results.clear();

	i = new unsigned long[this->_fnum];
	j = new unsigned long[this->_fnum];
	Dj = new unsigned long[this->_fnum];

	fragments = new string[this->_fnum];
	hitmaps_new = new SparseBitset *[this->_fnum];
	bucket_keys = new unsigned long[this->_fnum];
	for (k = 0; k < this->_fnum; k++)
	{
		fragments[k] = qseq.substr(k * this->_flen, this->_flen);					 // get the current fragment
		hitmaps_new[k] = new SparseBitset(this->_partition_size, this->_block_size); // create empty bitsets
		bucket_keys[k] = this->_hash_func(fragments[k]) % this->_buckets_num;		 // get the hash value of the bucket of the cur. fragment
																					 // cout<<"b["<<k<<"]="<<bucket_keys[k]<<endl;
	}

	for (alpha = 0; alpha < this->_part_len; alpha++)
	{ // cout<<"======> class no = "<<alpha<<" out of "<<(this->_part_len-1)<<endl;
		// Find the position of the first part.
		i[0] = (this->_part_len + alpha) % this->_part_len;						 // cout<<"\t i0="<<i[0]<<endl;
		j[0] = floor((float)(this->_part_len + alpha) / (float)this->_part_len); // cout<<"\t j0="<<j[0]<<endl;
		Dj[0] = 0;
		// x = part_len+alpha+qthr is the second part that belongs to the cover class 'alpha'.
		// For this part: i[0] = mod(x-qthr,part_len) and j[0]=floor(x-qthr/part_len).

		// Get the hitmap of the first part.
		tmp = this->getBucketPtr(bucket_keys[0]);

		if (tmp != 0)
		{
			tmp->lsbShiftBitset(hitmaps_new[0], i[0], Dj[0]);
		}
		// cout<<hitmaps_new[0]->getOnesListDelim()<<endl;

		// Find the position and the hitmaps of the rest of the parts.
		for (k = 1; k < this->_fnum; k++)
		{
			i[k] = (this->_part_len + alpha + k * this->_flen) % this->_part_len; // ATTENTION! It is (part_len+alpha+qthr-qthr+k*flen)
			// cout<<"i[k]="<<i[k]<<endl;
			//			i[k] = (i[k-1]+this->_part_shift)%this->_partition_num; //cout<<"i[k]="<<i[k];
			//			j[k] = floor((float)(this->_part_len+alpha-this->_qthr+k*this->_part_shift)/(float)this->_part_len); //cout<<" j[k]="<<j[k]<<endl;
			j[k] = floor((float)(this->_part_len + alpha + k * this->_flen) / (float)this->_part_len); // cout<<"\t j[k]="<<j[k];
			Dj[k] = j[k] - j[0];																	   // cout<<" Dj[k]="<<Dj[k]<<endl;

			//			cur_bucket = (this->_hash_func(fragments[k])%this->_buckets_num);
			//			tmp = this->getBucketPtr(cur_bucket);
			tmp = this->getBucketPtr(bucket_keys[k]);

			if (tmp != 0)
			{
				tmp->lsbShiftBitset(hitmaps_new[k], i[k], Dj[k]);
			}
			// cout<<"["<<k<<":]"<<hitmaps_new[k]->getOnesListDelim()<<endl;
			// cout<<"["<<k<<":]"<<bucket_keys[k]<<endl;
		}
		this->_counters->tSetFlagsCalc(hitmaps_new, this->_fnum, (this->_fnum - this->_qthr), this->_tsetflags_new);
		this->_counters->reset(); // ATTENTION!!!! THIS IS NOT REQUIRED FOR THE NEW COUNTER BITSETS (THOSE USING SPARSEBITSETS)
		// Calculate the windows that need to be verified (based on this eq. class).
		for (k = 0; k < this->_blocks_num; k++) // get a block each time
		{
			if (this->_tsetflags_new->testBlock(k)) // check that there is at least one 1 in the block
			{
				for (tmp_1 = 0; tmp_1 < this->_block_size; tmp_1++) // check one by one the bits
				{
					if (this->_tsetflags_new->testInNonEmptyBlock(k, tmp_1))
					{
						bit_id = k * this->_block_size + tmp_1;
						if (bit_id < this->_tsetflags_new->size()) // otherwise it is one of the padding bits
						{
							// win_start = bit_id*this->_part_len+i[0];
							win_start = alpha + bit_id * this->_part_len + this->_qthr;

							// in case the window goes outside the data seq (inside the padding area)
							if (win_start >= (this->_dseq_ptr->length() - 1))
								continue;
							if (hitmaps_new[0]->test(bit_id))
							{
								if (fragments[0] == this->_dseq_ptr->substr(win_start, this->_flen))
								{
									// get the window verification
									this->_wins.push_back(win_start);
								}
							}
							else
							{
								// get the window verification
								this->_wins.push_back(win_start);
							}

							// cout<<"bit_id="<<bit_id<<", win_start="<<win_start<<", win_real="<<(alpha+bit_id*this->_part_len+this->_qthr); if( (win_start-this->_qthr)%this->_part_len == alpha ){cout<<" Y"<<endl;}else{cout<<" N"<<endl;}
							// win_start = alpha+bit_id*this->_part_len+this->_qthr; //if( (win_start-this->_qthr)%this->_part_len == alpha ){cout<<" Y"<<endl;}
						}

						this->_tsetflags_new->flipNonEmptyBlock(k, tmp_1);
						if (this->_tsetflags_new->runNoneInBlock(k))
							break;
					}
				}
			}
		}
	}
	for (k = 0; k < this->_fnum; k++)
	{
		delete hitmaps_new[k];
	}
	delete[] hitmaps_new;
	delete[] bucket_keys;

	delete[] i;
	delete[] j;
	delete[] Dj;

	delete[] fragments;
}

void HitmapHash::verifyWindows(string &qseq)
{
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	if (this->_wins.empty())
		return; // no windows - no results

	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	ver_area_start = (*cur_win);
	ver_area_end = ver_area_start + qseq.length() + this->_qthr - 1;
	if (ver_area_end >= this->_dseq_ptr->length())
		ver_area_end = last_pos;
	cur_win++;
	while (cur_win != this->_wins.end())
	{
		if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
		{							   // cout<<"["<<ver_area_start<<","<<ver_area_end<<"] - qthr="<<this->_qthr<<endl;
			dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
			// cout<<"results="<<this->_results.size()<<endl;
			ver_area_start = (*cur_win);
		}
		ver_area_end = (*cur_win) + qseq.length() + this->_qthr - 1;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
	} // cout<<"*["<<ver_area_start<<","<<ver_area_end<<"]"<<endl;
	dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
	// cout<<"results="<<this->_results.size()<<endl;
}

void HitmapHash::keepUniqueWindows(void)
{
	if (!this->_wins.empty())
	{
		vector<int>::iterator tmp_it;
		sort(this->_wins.begin(), this->_wins.end());
		tmp_it = unique(this->_wins.begin(), this->_wins.end());
		this->_wins.resize(std::distance(this->_wins.begin(), tmp_it));
	}
}

//=====
// CounterBitset methods.

CounterBitsets::CounterBitsets(unsigned long cnt_num, unsigned long t_max, unsigned long block_size)
{
	// Get the number of counters to be encoded.
	this->_cnt_num = cnt_num;

	// Compute the number of counter (and remainder) bitsets we need to have.
	this->_cnt_bitsets_num = ceil((float)log(t_max + 1) / (float)log(2));

	// ATTENTION! The counters could take any value in [0,t_max], thus we have t_max+1 possible values.

	// Initialize counter and remainder bitsets.
	this->_cnt_bitsets_new = new SparseBitset *[this->_cnt_bitsets_num];
	this->_rem_bitsets_new = new SparseBitset *[this->_cnt_bitsets_num];

	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		this->_cnt_bitsets_new[i] = new SparseBitset(this->_cnt_num, block_size);
		this->_rem_bitsets_new[i] = new SparseBitset(this->_cnt_num, block_size);
	}
}

CounterBitsets::~CounterBitsets()
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		delete this->_cnt_bitsets_new[i];
		delete this->_rem_bitsets_new[i];
	}
	delete[] this->_cnt_bitsets_new;
	delete[] this->_rem_bitsets_new;
}

void CounterBitsets::tSetFlagsCalc(SparseBitset **bitsets, unsigned long bitsets_num, unsigned long t, SparseBitset *result)
{
	unsigned long y;
	unsigned long w;
	int empty_num;
	empty_num = 0;

	if (t == 1) // special case
	{			// cout<<(*result)<<endl;
		result->reset();
		for (w = 0; w < bitsets_num; w++)
		{
			if (!bitsets[w]->empty())
			{
				(*result) |= (*bitsets[w]);
			}
		}
		return;
		//	cout<<(*result)<<endl; int xxx; cin>>xxx;
	}
	vector<int> blocks_to_be_tested;

	// Find out if there are so much zero bitsets that there is no way to produce any results.
	/*	for(w=0; w<bitsets_num; w++)
		{
			if( bitsets[w]->empty() )
			{
				empty_num++;
			}
		}
		if( empty_num>(bitsets_num-t)){ result->reset(); return;}*/

	int non_empty_num;
	for (y = 0; y < SparseBitset::getBlocksNumSt(); y++)
	{
		non_empty_num = 0;
		for (w = 0; w < bitsets_num; w++)
		{
			if (bitsets[w]->testBlock(y))
			{
				non_empty_num++;
			}
		}
		if (non_empty_num >= t)
		{
			blocks_to_be_tested.push_back(y);
		}
	}

	//	cout<<"blocks_to_be_tested="<<blocks_to_be_tested.size()<<endl;

	// Compute the values of the counter bitsets.
	for (w = 0; w < bitsets_num; w++)
	{
		if (!bitsets[w]->empty())
		{
			//			(*this->_rem_bitsets_new[0]) = (*this->_cnt_bitsets_new[0]) & (*bitsets[w]); //first compute the remainder (requires the old counter)
			//			(*this->_cnt_bitsets_new[0]) ^= (*bitsets[w]); //compute the new counter
			this->_rem_bitsets_new[0]->selective_and(this->_cnt_bitsets_new[0], bitsets[w], blocks_to_be_tested);
			this->_cnt_bitsets_new[0]->selective_xor(bitsets[w], blocks_to_be_tested); // compute the new counter

			for (y = 1; y < this->_cnt_bitsets_num; y++)
			{
				//				(*this->_rem_bitsets_new[y]) = (*this->_cnt_bitsets_new[y]) & (*this->_rem_bitsets_new[y-1]); //first compute the remainder (requires the old counter)
				//				(*this->_cnt_bitsets_new[y]) ^= (*this->_rem_bitsets_new[y-1]); //compute the new counter
				this->_rem_bitsets_new[y]->selective_and(this->_cnt_bitsets_new[y], this->_rem_bitsets_new[y - 1], blocks_to_be_tested);
				this->_cnt_bitsets_new[y]->selective_xor(this->_rem_bitsets_new[y - 1], blocks_to_be_tested); // compute the new counter
			}
		}
		else
		{
			/*			//AND with zeros returns zeros

			//NOTE: if the initial bitset is full of zeros, then ANDs have no effect on _cnt_bitsets[0] and
			//_rem_bitsets[0] is full of zeros (ORs with zeros return zeros). However, this means that all
			//the _cnt_bitsets[y] remain the same as initially set

						for(y=0; y<this->_cnt_bitsets_num; y++)
						{
							(*this->_rem_bitsets[y]) = (*HitmapBucket::getZeros());
						}*/
		}
	}

	// Compute the t-set flags bitset.
	dynamic_bitset<> t_bin(this->_cnt_num, t); // Get the binary representation of t.
	result->set();							   // Just to be sure that initially is full of 1s.

	for (y = 0; y < this->_cnt_bitsets_num; y++)
	{
		// Note that while this loop is executed, result remains equal to 11...1. It is modified
		// only at the last loop.
		if (t_bin.test(y))
		{
			(*result) &= *(this->_cnt_bitsets_new[y]); // Because 11...1 AND x = x
			// Note: I think that when I perform an AND is faster than assigning the value of the
			// bitset to the result. This has to do with the implementation. I want to test this
			// more...
			break;
		}
		// Note: if t_bin[y]==0 I do nothing because I had to perform an OR with 11...1 and this
		// results into 11...1 always.
	}
	for (y = y + 1; y < this->_cnt_bitsets_num; y++)
	{
		if (!t_bin.test(y))
		{
			(*result) |= (*this->_cnt_bitsets_new[y]);
		}
		else
		{
			(*result) &= (*this->_cnt_bitsets_new[y]);
		}
	}
}

void CounterBitsets::reset(void)
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		this->_cnt_bitsets_new[i]->reset();
	}
}

#endif

//========================================================================================
// HitmapBucket using only the memory optimization of block pool.

#ifdef USE_POOL_ONLY

// Declare the static variables.
unsigned long HitmapBucket::_bs_num;
unsigned long HitmapBucket::_bs_size;
unsigned long HitmapBucket::_block_size;
SparseBitsetC *HitmapBucket::_zeros = NULL;
SparseBitset *HitmapBucket::_zeros2 = NULL;

//====================
// HitmapBcuket

HitmapBucket::HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size)
{
	unsigned long i;

	// Store the number and size of bitsets in each hitmap.
	HitmapBucket::_bs_num = bs_num;
	HitmapBucket::_bs_size = bs_size;
	HitmapBucket::_block_size = block_size;

	if (HitmapBucket::_zeros == NULL)
	{
		HitmapBucket::_zeros = new SparseBitsetC(HitmapBucket::_bs_size, HitmapBucket::_block_size); // Create a bitset full of zeros
	}

	if (HitmapBucket::_zeros2 == NULL)
	{
		HitmapBucket::_zeros2 = new SparseBitset(HitmapBucket::_bs_size, HitmapBucket::_block_size);
	}

	// Create the array of bitsets.
	this->_hitmap = new SparseBitsetC *[HitmapBucket::_bs_num];

	// Create the bitsets.
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		this->_hitmap[i] = 0;
	}
}

HitmapBucket::~HitmapBucket()
{
	for (int i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		{
			delete this->_hitmap[i];
		}
	}
	delete[] this->_hitmap;
	delete HitmapBucket::_zeros;
	HitmapBucket::_zeros = NULL;
	delete HitmapBucket::_zeros2;
	HitmapBucket::_zeros2 = NULL;
}

void HitmapBucket::setHit(unsigned long bs_id, unsigned long b_id)
{
	// Create the bitset if needed.
	if (this->_hitmap[bs_id] == 0)
		this->_hitmap[bs_id] = new SparseBitsetC(HitmapBucket::_bs_size, HitmapBucket::_block_size);

	// Set the bit of the corresponding bitset.
	this->_hitmap[bs_id]->set(b_id);
}

void HitmapBucket::lsbShiftBitset(SparseBitset *result, unsigned long bs_id, unsigned long shift_by)
{
	if (this->_hitmap[bs_id] != 0) // if the bitset is not empty
	{
		result->decompress(this->_hitmap[bs_id]);
		result->shiftLSB(shift_by);
	}
	else // if the corresponding bitset is empty
	{
		(*result) = (*HitmapBucket::_zeros2);
	}
}

void HitmapBucket::printBitset(unsigned long bs_id)
{
	cout << "B" << bs_id << ": ";

	if (this->_hitmap[bs_id] == 0)
	{
		cout << "EMPTY" << endl;
	}
	else
	{
		this->_hitmap[bs_id]->print();
		cout << endl;
	}
}

void HitmapBucket::print()
{
	unsigned long i;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		// print the contents of the current bitset.
		this->printBitset(i);
	}
}

SparseBitsetC HitmapBucket::getBitset(unsigned long bs_id)
{
	if (this->_hitmap[bs_id] == 0)
	{
		SparseBitsetC tmp(HitmapBucket::_bs_size, HitmapBucket::_block_size);
		return tmp;
	}
	else
	{
		return (*this->_hitmap[bs_id]);
	}
}

unsigned long HitmapBucket::countZeros()
{
	unsigned long i;
	unsigned long zeros_cnt;
	zeros_cnt = 0;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i]->empty())
		{
			zeros_cnt++;
		}
	}
	return zeros_cnt;
}

unsigned long HitmapBucket::countEmptyBitsets()
{
	unsigned long empty_bitsets_num;
	empty_bitsets_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] == 0)
		{
			empty_bitsets_num++;
		}
	}
	return empty_bitsets_num;
}

double HitmapBucket::avgSetBits()
{
	double avg_set_bits;
	double bits_examined;
	avg_set_bits = 0;
	bits_examined = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]!=0 )
		if (this->_hitmap[i] != 0)
		{
			//			avg_set_bits += (double)this->_hitmap_old[i]->count();
			avg_set_bits += (double)this->_hitmap[i]->count();
			//			bits_examined+= this->_hitmap_old[i]->size();
			bits_examined += (double)this->_hitmap[i]->size();
		}
	}
	if (avg_set_bits != 0)
		avg_set_bits = avg_set_bits / bits_examined;
	return avg_set_bits;
}

double HitmapBucket::countEmptyBlocks()
{
	double empty_blocks_num;
	empty_blocks_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		{
			empty_blocks_num += this->_hitmap[i]->countEmptyBlocks();
		}
	}
	return empty_blocks_num;
}
/*
double HitmapBucket::countUnaryBlocks()
{
	double blocks_num;
	blocks_num = 0;
	for( unsigned long i=0; i<HitmapBucket::_bs_num; i++)
	{
		if( this->_hitmap[i]!=0 )
		{
			blocks_num += this->_hitmap[i]->getUnaryBlocksNum();
		}
	}
	return blocks_num;
}*/

double HitmapBucket::countBlocks()
{
	double blocks_num;
	blocks_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		blocks_num += (double)this->_hitmap[i]->getBlocksNum();
	}
	return blocks_num;
}

int HitmapBucket::countSetBits()
{
	int set_bits = 0;
	for (int i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		//		if(this->_hitmap_old[i]!=0 )
		{
			//			set_bits+=this->_hitmap_old[i]->count();
			set_bits += this->_hitmap[i]->count();
		}
	}
	return set_bits;
}

int HitmapBucket::countSetBits(unsigned long bs_id)
{
	//	return this->_hitmap_old[bs_id]->count();
	return this->_hitmap[bs_id]->count();
}

void HitmapBucket::removeZeros()
{
	unsigned long i;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]->none() )
		if (this->_hitmap[i]->empty())
		{
			delete this->_hitmap[i];
			this->_hitmap[i] = 0;
			//			delete this->_hitmap_old[i];
			//			this->_hitmap_old[i] = 0;
		}
	}
}

//====================
// ConterBitsets

CounterBitsets::CounterBitsets(unsigned long cnt_num, unsigned long t_max, unsigned long block_size)
{
	// Get the number of counters to be encoded.
	this->_cnt_num = cnt_num;

	// Compute the number of counter (and remainder) bitsets we need to have.
	this->_cnt_bitsets_num = ceil((float)log(t_max + 1) / (float)log(2));
	// ATTENTION! The counters could take any value in [0,t_max], thus we have t_max+1 possible values.

	// Initialize counter and remainder bitsets.
	//	this->_cnt_bitsets = new dynamic_bitset<>* [this->_cnt_bitsets_num];
	this->_cnt_bitsets_new = new SparseBitset *[this->_cnt_bitsets_num];
	//	this->_rem_bitsets = new dynamic_bitset<>* [this->_cnt_bitsets_num];
	this->_rem_bitsets_new = new SparseBitset *[this->_cnt_bitsets_num];
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		this->_cnt_bitsets[i] = new dynamic_bitset<>(this->_cnt_num);
		//		this->_rem_bitsets[i] = new dynamic_bitset<>(this->_cnt_num);
		this->_cnt_bitsets_new[i] = new SparseBitset(this->_cnt_num, block_size);
		this->_rem_bitsets_new[i] = new SparseBitset(this->_cnt_num, block_size);
	}
}

CounterBitsets::~CounterBitsets()
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		delete this->_cnt_bitsets[i];
		//		delete this->_rem_bitsets[i];
		delete this->_cnt_bitsets_new[i];
		delete this->_rem_bitsets_new[i];
	}
	//	delete [] this->_cnt_bitsets;
	//	delete [] this->_rem_bitsets;
	delete[] this->_cnt_bitsets_new;
	delete[] this->_rem_bitsets_new;
}

/*
void CounterBitsets::tSetFlagsCalc(dynamic_bitset<>** bitsets, unsigned long bitsets_num, unsigned long t, dynamic_bitset<>* result)
{
	unsigned long y;
	unsigned long w;
	int empty_num;
	empty_num = 0;

	//Find out if there are so much zero bitsets that there is no way to produce any results.
	for(w=0; w<bitsets_num; w++)
	{
		if( bitsets[w]->empty() )
		{
			empty_num++;
		}
	}
	if( empty_num>(bitsets_num-t)){ result->reset(); return;}

	//Compute the values of the counter bitsets.
	for(w=0; w<bitsets_num; w++)
	{
		if( !bitsets[w]->empty() )
		{
			(*this->_rem_bitsets[0]) = (*this->_cnt_bitsets[0]) & (*bitsets[w]); //first compute the remainder (requires the old counter)
			(*this->_cnt_bitsets[0]) ^= (*bitsets[w]); //compute the new counter

			for(y=1; y<this->_cnt_bitsets_num; y++)
			{
				(*this->_rem_bitsets[y]) = (*this->_cnt_bitsets[y]) & (*this->_rem_bitsets[y-1]); //first compute the remainder (requires the old counter)
				(*this->_cnt_bitsets[y]) ^= (*this->_rem_bitsets[y-1]); //compute the new counter
			}
		}
		else
		{
			//AND with zeros returns zeros

//NOTE: if the initial bitset is full of zeros, then ANDs have no effect on _cnt_bitsets[0] and
//_rem_bitsets[0] is full of zeros (ORs with zeros return zeros). However, this means that all
//the _cnt_bitsets[y] remain the same as initially set

//			for(y=0; y<this->_cnt_bitsets_num; y++)
//			{
//				(*this->_rem_bitsets[y]) = (*HitmapBucket::getZeros());
//			}
		}
	}

	//Compute the t-set flags bitset.
	dynamic_bitset<> t_bin(this->_cnt_num,t); //Get the binary representation of t.
	result->set(); //Just to be sure that initially is full of 1s.
	for( y=0; y<this->_cnt_bitsets_num; y++)
	{
		//Note that while this loop is executed, result remains equal to 11...1. It is modified
		//only at the last loop.
		if(t_bin.test(y))
		{
			(*result) &= (*this->_cnt_bitsets[y]); //Because 11...1 AND x = x
			//Note: I think that when I perform an AND is faster than assigning the value of the
			//bitset to the result. This has to do with the implementation. I want to test this
			//more...
			break;
		}
		//Note: if t_bin[y]==0 I do nothing because I had to perform an OR with 11...1 and this
		//results into 11...1 always.
	}

	for( y=y+1; y<this->_cnt_bitsets_num; y++)
	{
		if(!t_bin.test(y))
		{
			(*result) |= (*this->_cnt_bitsets[y]);
		}
		else
		{
			(*result) &= (*this->_cnt_bitsets[y]);
		}
	}
}*/

void CounterBitsets::tSetFlagsCalc(SparseBitset **bitsets, unsigned long bitsets_num, unsigned long t, SparseBitset *result)
{
	unsigned long y;
	unsigned long w;
	int empty_num;
	empty_num = 0;

	vector<int> blocks_to_be_tested;

	// Find out if there are so much zero bitsets that there is no way to produce any results.
	/*	for(w=0; w<bitsets_num; w++)
		{
			if( bitsets[w]->empty() )
			{
				empty_num++;
			}
		}
		if( empty_num>(bitsets_num-t)){ result->reset(); return;}*/

	int non_empty_num;
	for (y = 0; y < SparseBitsetC::getBlocksNumSt(); y++)
	{
		non_empty_num = 0;
		for (w = 0; w < bitsets_num; w++)
		{
			if (bitsets[w]->testBlock(y))
			{
				non_empty_num++;
			}
		}
		if (non_empty_num >= t)
		{
			blocks_to_be_tested.push_back(y);
		}
	}

	//	cout<<"blocks_to_be_tested="<<blocks_to_be_tested.size()<<endl;

	// Compute the values of the counter bitsets.
	for (w = 0; w < bitsets_num; w++)
	{
		if (!bitsets[w]->empty())
		{
			//			(*this->_rem_bitsets_new[0]) = (*this->_cnt_bitsets_new[0]) & (*bitsets[w]); //first compute the remainder (requires the old counter)
			//			(*this->_cnt_bitsets_new[0]) ^= (*bitsets[w]); //compute the new counter
			this->_rem_bitsets_new[0]->selective_and(this->_cnt_bitsets_new[0], bitsets[w], blocks_to_be_tested);
			this->_cnt_bitsets_new[0]->selective_xor(bitsets[w], blocks_to_be_tested); // compute the new counter

			for (y = 1; y < this->_cnt_bitsets_num; y++)
			{
				//				(*this->_rem_bitsets_new[y]) = (*this->_cnt_bitsets_new[y]) & (*this->_rem_bitsets_new[y-1]); //first compute the remainder (requires the old counter)
				//				(*this->_cnt_bitsets_new[y]) ^= (*this->_rem_bitsets_new[y-1]); //compute the new counter
				this->_rem_bitsets_new[y]->selective_and(this->_cnt_bitsets_new[y], this->_rem_bitsets_new[y - 1], blocks_to_be_tested);
				this->_cnt_bitsets_new[y]->selective_xor(this->_rem_bitsets_new[y - 1], blocks_to_be_tested); // compute the new counter
			}
		}
		else
		{
			/*			//AND with zeros returns zeros

			//NOTE: if the initial bitset is full of zeros, then ANDs have no effect on _cnt_bitsets[0] and
			//_rem_bitsets[0] is full of zeros (ORs with zeros return zeros). However, this means that all
			//the _cnt_bitsets[y] remain the same as initially set

						for(y=0; y<this->_cnt_bitsets_num; y++)
						{
							(*this->_rem_bitsets[y]) = (*HitmapBucket::getZeros());
						}*/
		}
	}

	// Compute the t-set flags bitset.
	dynamic_bitset<> t_bin(this->_cnt_num, t); // Get the binary representation of t.
	result->set();							   // Just to be sure that initially is full of 1s.
	for (y = 0; y < this->_cnt_bitsets_num; y++)
	{
		// Note that while this loop is executed, result remains equal to 11...1. It is modified
		// only at the last loop.
		if (t_bin.test(y))
		{
			(*result) &= (*this->_cnt_bitsets_new[y]); // Because 11...1 AND x = x
			// Note: I think that when I perform an AND is faster than assigning the value of the
			// bitset to the result. This has to do with the implementation. I want to test this
			// more...
			break;
		}
		// Note: if t_bin[y]==0 I do nothing because I had to perform an OR with 11...1 and this
		// results into 11...1 always.
	}

	for (y = y + 1; y < this->_cnt_bitsets_num; y++)
	{
		if (!t_bin.test(y))
		{
			(*result) |= (*this->_cnt_bitsets_new[y]);
		}
		else
		{
			(*result) &= (*this->_cnt_bitsets_new[y]);
		}
	}
}

void CounterBitsets::reset(void)
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		this->_cnt_bitsets[i]->reset();
		this->_cnt_bitsets_new[i]->reset();
	}
}

void CounterBitsets::print(void)
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		cout<<"C"<<i<<": "<<(*this->_cnt_bitsets[i])<<endl;
		cout << "TODO UPDATE THIS CLASS IN ORDER TO SUPPORT NEW COUNTER BITSETS" << endl;
	}
}

//====================
// HitmapHash

HitmapHash::HitmapHash(string *dseq_ptr, unsigned long qthr, unsigned long part_shift, unsigned long flen, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, unsigned long buck_groups_num) : StrHash<HitmapBucket>(flen, buckets_num, buck_groups_num)
{
	string cur_str; // a string bucket
	unsigned long cur_bucket;

	this->_dseq_ptr = dseq_ptr;					 // get the pointer to the data sequence
	this->_dseq_len = this->_dseq_ptr->length(); // get the length of the data sequence
	this->_qthr = qthr;
	this->_flen = flen;
	this->_part_len = this->_flen + 2 * this->_qthr;							   // get the length of the parts
	this->_part_shift = part_shift;												   // get how many positions is shifted the one part for the next
	this->_partition_num = this->_part_len;										   // get the number of partitions
	this->_partition_size = ceil((float)this->_dseq_len / (float)this->_part_len); // get how many parts are in each partition
	this->_counters = NULL;														   // Initially no counters. We specify counters by calling initCounterBitsets().
	this->_fnum = fnum;
	this->_block_size = block_size;
	if (this->_block_size > this->_partition_size)
	{
		cout << "WARNING - [Creating hitmap hash] The sparse bitset blocks are defined to be larger than the hitmaps!" << endl;
		cout << "\t * hitmap size = " << this->_partition_size << endl;
		cout << "\t * block size = " << this->_block_size << endl;
	}
	this->_tsetflags_new = new SparseBitset(this->_partition_size, this->_block_size);
	this->_blocks_num = this->_tsetflags_new->getBlocksNum();

	// In order to achieve the same number of parts in all partitions we extend dseq by adding symbols at the end. These extra
	// symbols are not part of the alphabet.
	string tmp_str(this->_part_len - 1, '$');
	this->_dseq_len = this->_dseq_ptr->length(); // update the length of the data sequence
	// attention!! this may create problem in case of consecutive index creation on the same data seq

	// Scan dseq to fill the hash table buckets
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{
		// Get the corresponding dseq subsequence
		cur_str = this->_dseq_ptr->substr(p, this->_flen);

		// Test if the bucket group is going to have content.
		this->setGroupStatus(this->_hash_func(cur_str) % this->_buckets_num);
	}
	this->initBuckets();

	/*
	int* arr;
	arr = new int[10];
	for(int i=0; i<10; i++)
		cout<<"[i="<<i<<":] "<<arr[i]<<" "<<&arr[i]<<endl;

	&arr[2] = 0;

	for(int i=0; i<10; i++)
		cout<<"[i="<<i<<":] "<<&arr[i]<<endl;

	int xxx;
	cin>>xxx;*/

	this->extraStatistics();
}

void HitmapHash::initBuckets()
{
	string cur_str; // a string bucket
	unsigned long cur_bucket;

	// In order to achieve the same number of parts in all partitions we extend dseq by adding symbols at the end. These extra
	// symbols are not part of the alphabet.
	string tmp_str(this->_part_len - 1, '$');
	// attention!! this may create problem in case of consecutive index creation on the same data seq

	HitmapBucket **tmp;

	// Scan dseq to fill the hash table buckets
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{ // cout<<"p="<<p<<" out of "<<(this->_dseq_len-this->_flen)<<endl;
		// Get the corresponding dseq subsequence
		cur_str = this->_dseq_ptr->substr(p, this->_flen);

		// Get the current bucket id based on the hashing function.
		cur_bucket = (this->_hash_func(cur_str) % this->_buckets_num);
		tmp = this->getBucketPtrRef(cur_bucket);

		// If this is the first hit for this bucket.
		//		if( this->_buckets[cur_bucket]== 0)
		if ((*tmp) == 0)
		{
			//			this->_buckets[cur_bucket] = new HitmapBucket(this->_partition_num,this->_partition_size,this->_block_size);
			(*tmp) = new HitmapBucket(this->_partition_num, this->_partition_size, this->_block_size);
			//			cout<<"=== NEW HitmapBucket pointer! @ "<<tmp<<endl;
		}

		int xx;
		int yy;
		xx = 0;
		for (int i = (p % this->_part_len); xx < (this->_part_len - this->_flen + 1); i--)
		{
			if (i < 0)
			{
				i = this->_partition_num - 1;
			}
			//			this->_buckets[cur_bucket]->setHit(i,floor((double)p/(double)this->_part_len));
			(*tmp)->setHit(i, floor((double)p / (double)this->_part_len));

			xx++;
		}
	}
}

HitmapHash::~HitmapHash(void)
{
	for (unsigned long i = 0; i < this->_buck_groups_num; i++)
	{
		this->deleteGroup(i);
	}
	delete[] this->_buckets;
	delete this->_counters;
	delete this->_tsetflags_new;
	delete this->_group_status;
}

void HitmapHash::initCounterBitsets(unsigned long t_max)
{
	this->_counters = new CounterBitsets(this->_partition_size, t_max, this->_block_size);
}

void HitmapHash::findWindows(string &qseq)
{
	unsigned long alpha; // Variable to store the current equivalence class.
	unsigned long k;
	unsigned long tmp_1;  // auxiliary variable
	unsigned long bit_id; // auxiliary variable
	string *fragments;	  // Array containing the fragments of the qseq.

	// Check that initCounterBitsets was executed.
	if (this->_counters == NULL)
	{
		cout << "ERROR -- [Hitmap searching] Run initCounterBitsets before searching..." << endl;
		return;
	}

	unsigned long cur_bucket;

	// Get the query fragments.
	fragments = new string[this->_fnum];
	for (unsigned long z = 0; z < this->_fnum; z++)
	{
		fragments[z] = qseq.substr(z * this->_flen, this->_flen);
	}

	this->_wins.clear();
	this->_results.clear();

	unsigned long win_start; // the start position of a verification window
	unsigned long win_end;	 // the end position of a verification window

	unsigned long *i;
	unsigned long *j;
	unsigned long *Dj;

	//	dynamic_bitset<>** hitmaps;
	SparseBitset **hitmaps_new;

	i = new unsigned long[this->_fnum];
	j = new unsigned long[this->_fnum];
	Dj = new unsigned long[this->_fnum];

	//	hitmaps = new dynamic_bitset<>*[this->_fnum];
	hitmaps_new = new SparseBitset *[this->_fnum];
	for (k = 0; k < this->_fnum; k++)
	{
		//		hitmaps[k] = new dynamic_bitset<>; //Create empty bitsets
		hitmaps_new[k] = new SparseBitset(this->_partition_size, this->_block_size); // create empty bitsets
	}
	HitmapBucket *tmp;
	for (alpha = 0; alpha < this->_part_len; alpha++)
	{ // cout<<"alpha="<<alpha<<" of "<<this->_part_len<<endl;
		// Find the position of the first part.
		i[0] = (this->_part_len + alpha - this->_qthr) % this->_part_len;
		j[0] = floor((float)(this->_part_len + alpha - this->_qthr) / (float)this->_part_len);
		Dj[0] = 0;

		// Note: We use this->_part_len+alpha as example (the second part) just to avoid negatives

		// Get the hitmap of the first part.
		cur_bucket = (this->_hash_func(fragments[0]) % this->_buckets_num); // get the bucket
		tmp = this->getBucketPtr(cur_bucket);

		//		if( this->_buckets[cur_bucket]==0 )
		if (tmp == 0)
		{
			continue;
		}
		tmp->lsbShiftBitset(hitmaps_new[0], i[0], Dj[0]);

		//		this->_buckets[cur_bucket]->lsbShiftBitset(hitmaps_new[0],i[0],Dj[0]);

		/*		SparseBitset* tmp; tmp = new SparseBitset(this->_partition_size,this->_block_size);
				this->_buckets_alt[cur_bucket]->lsbShiftBitset(tmp,i[0],Dj[0]);
				if(tmp->count()!=hitmaps_new[0]->count()){ cout<<"========PROB1!!!!!"<<endl; int xxx; cin>>xxx;}*/

		/*

			if( !hitmaps[0]->empty())
				hitmaps_new[0]->compareWithRegBitset(hitmaps[0]); */
		/*			if( hitmaps[0]->empty() )
						hitmaps_new[0]->clear();
					else
						hitmaps_new[0]->setEfficient(hitmaps[0]);*/

		// Find the position and the hitmaps of the rest of the parts.
		for (k = 1; k < this->_fnum; k++)
		{
			i[k] = (i[k - 1] + this->_part_shift) % this->_partition_num;
			j[k] = floor((float)(this->_part_len + alpha - this->_qthr + k * this->_part_shift) / (float)this->_part_len);
			Dj[k] = j[k] - j[0];

			cur_bucket = (this->_hash_func(fragments[k]) % this->_buckets_num);
			tmp = this->getBucketPtr(cur_bucket);
			//			if( this->_buckets[cur_bucket]!=0)
			if (tmp != 0)
			{
				//				this->_buckets[cur_bucket]->lsbShiftBitset(hitmaps_new[k],i[k],Dj[k]);
				tmp->lsbShiftBitset(hitmaps_new[k], i[k], Dj[k]);

				/*			this->_buckets_alt[cur_bucket]->lsbShiftBitset(tmp,i[k],Dj[k]);
					if(tmp->count()!=hitmaps_new[k]->count())
					{
					cout<<"========"<<endl<<"PROB!!!!! buck:"<<cur_bucket<<", bitset:"<<i[k]<<endl;
					cout<<"OLD METHOD: "<<endl; tmp->print();
					cout<<"NEW METHOD: "<<endl; hitmaps_new[k]->print();int zzz; cin>>zzz;}*/

				/*				this->_buckets[cur_bucket]->lsbShiftBitset_old(hitmaps[k],i[k],Dj[k]);

				if( !hitmaps[k]->empty())
						hitmaps_new[k]->compareWithRegBitset(hitmaps[k]);*/

				/*				if( hitmaps[k]->empty() )
									hitmaps_new[k]->clear();
								else
									hitmaps_new[k]->setEfficient(hitmaps[k]);*/
			}
		}
		//		this->_counters->tSetFlagsCalc(hitmaps,this->_fnum,this->_qthr,tsetflags);
		this->_counters->tSetFlagsCalc(hitmaps_new, this->_fnum, (this->_fnum - this->_qthr), this->_tsetflags_new);
		this->_counters->reset(); // ATTENTION!!!! THIS IS NOT REQUIRED FOR THE NEW COUNTER BITSETS (THOSE USING SPARSEBITSETS)

		// cout<<"tsetflags:"<<endl;
		// this->_tsetflags_new->print();

		// Calculate the windows that need to be verified (based on this eq. class).
		for (k = 0; k < this->_blocks_num; k++) // get a block each time
		{
			if (this->_tsetflags_new->testBlock(k)) // check that there is at least one 1 in the block
			{
				for (tmp_1 = 0; tmp_1 < this->_block_size; tmp_1++) // check one by one the bits
				{
					if (this->_tsetflags_new->testInNonEmptyBlock(k, tmp_1))
					{
						bit_id = k * this->_block_size + tmp_1;
						if (bit_id < this->_tsetflags_new->size()) // otherwise it is one of the padding bits
						{
							win_start = bit_id * this->_part_len + i[0];

							// get the window verification
							this->_wins.push_back(win_start);
						}

						this->_tsetflags_new->flipNonEmptyBlock(k, tmp_1);
						if (this->_tsetflags_new->runNoneInBlock(k))
							break;
					}
				}
			}
		}

		/*		for( int ii=0; ii<this->_partition_size; ii++) //TODO MPOREI NA YPARXEI KALYTEROS TROPOS GIA AUTO
				{
		//			if( tsetflags->test(ii) )
					if( tsetflags_new->test(ii) )
					{
						win_start = ii*this->_part_len+i[0];
						//get the window verification
			//			this->_wins.push_back(win_start);
						cout<<"old: "<<win_start<<endl;
					}
				}*/
	}

	for (k = 0; k < this->_fnum; k++)
	{
		//		delete hitmaps[k];
		delete hitmaps_new[k];
	}
	//	delete [] hitmaps;
	delete[] hitmaps_new;

	delete[] i;
	delete[] j;
	delete[] Dj;

	delete[] fragments;

	//	cout<<"ALL OK!"<<endl;
}

void HitmapHash::verifyWindows(string &qseq)
{
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	ver_area_start = (*cur_win);
	ver_area_end = ver_area_start + qseq.length() + 2 * this->_qthr;
	if (ver_area_end >= this->_dseq_ptr->length())
		ver_area_end = last_pos;
	cur_win++;
	while (cur_win != this->_wins.end())
	{
		if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
		{
			dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
			ver_area_start = (*cur_win);
		}
		ver_area_end = (*cur_win) + qseq.length() + 2 * this->_qthr;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
	}
	dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
}

void HitmapHash::verifyWindows(string &qseq, unsigned long epsilon, unsigned long fnum)
{
	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	while (cur_win != this->_wins.end())
	{
		dynamicProgramming(this->_dseq_ptr, (*cur_win), ((*cur_win) + qseq.length() + epsilon), &qseq, epsilon, &this->_results);
		cur_win++;
	}
}

vector<int> HitmapHash::getWindows(void)
{
	return this->_wins;
}

vector<int> HitmapHash::getResults(void)
{
	this->keepUniqueResults();
	return this->_results;
}

void HitmapHash::keepUniqueWindows(void)
{
	vector<int>::iterator tmp_it;
	sort(this->_wins.begin(), this->_wins.end());
	tmp_it = unique(this->_wins.begin(), this->_wins.end());
	this->_wins.resize(std::distance(this->_wins.begin(), tmp_it));
}

void HitmapHash::keepUniqueResults(void)
{
	vector<int>::iterator tmp_it;
	sort(this->_results.begin(), this->_results.end());
	tmp_it = unique(this->_results.begin(), this->_results.end());
	this->_results.resize(std::distance(this->_results.begin(), tmp_it));
}

void HitmapHash::setVerificationStatistics(string &qseq)
{
	int ver_areas_num;
	int ver_size;
	ver_size = 0;
	ver_areas_num = 0;
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	ver_area_start = (*cur_win);
	ver_area_end = ver_area_start + qseq.length() + 2 * this->_qthr;
	if (ver_area_end >= this->_dseq_ptr->length())
		ver_area_end = last_pos;
	cur_win++;
	while (cur_win != this->_wins.end())
	{
		if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
		{
			ver_areas_num++;
			ver_size += (ver_area_end - ver_area_start + 1);
			//		cout<<"ar. start: "<<ver_area_start<<", ar. end: "<<ver_area_end<<endl;

			ver_area_start = (*cur_win);
		}
		ver_area_end = (*cur_win) + qseq.length() + 2 * this->_qthr;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
	}
	ver_areas_num++;
	//	if(ver_area_end-ver_area_start+1<0){ cout<<"ver_area_end="<<ver_area_end<<", ver_area_start="<<ver_area_start<<endl; int xx; cin>>xx;}
	ver_size += (ver_area_end - ver_area_start + 1);

	this->_stats._ver_areas_num = ver_areas_num;
	this->_stats._data_size = this->_dseq_len;
	this->_stats._ver_size = ver_size;
	this->_stats._ver_size_percentage = (double)this->_stats._ver_size / (double)this->_stats._data_size;
}

VerificationStatistics HitmapHash::getVerificationStatistics(void)
{
	return this->_stats;
}

StorageSize HitmapHash::byteSize()
{
	StorageSize size;
	HitmapBucket *tmp;

	// Consider the pointers to the groups of buckets.
	size._total_bytes = this->_buck_groups_num * sizeof(HitmapHash **);
	size._total_bytes_without_opts = this->_buck_groups_num * sizeof(HitmapHash **);

	cout << "Group pointers size = " << size._total_bytes << endl;

	// Consider the pointers to the buckets.
	for (int i = 0; i < this->_buck_groups_num; i++)
	{
		if (this->_buckets[i] != 0)
		{
			size._total_bytes += this->_buckets_per_group * sizeof(HitmapHash *);
			size._total_bytes_without_opts += this->_buckets_per_group * sizeof(HitmapHash *);
		}
	}
	cout << "Non empty groups = " << this->_group_status->count() << endl;
	cout << "Considering bucket pointers the size is " << size._total_bytes << endl;

	// Consider the size of the buckets.
	for (int i = 0; i < this->_buckets_num; i++)
	{
		tmp = this->getBucketPtr(i); // cout<<"after"<<endl;
		if (tmp != 0)
		{
			size._total_bytes_without_opts += tmp->byteSize(true);
			size._hitmap_bytes += tmp->byteSize(false);
		}
	}
	size._total_bytes += size._hitmap_bytes;

	// Consider the size of the UVB pool.
	size._pool_bytes = SparseBitsetC::poolByteSize();
	size._total_bytes += size._pool_bytes;
	//	size._total_bytes_without_opts += size._pool_bytes;

	// Attention!! There is also the size of counters, however it is not fixed and it is
	// very small.
	return size;
}

void HitmapHash::extraStatistics()
{
	unsigned long empty_buckets_num;
	unsigned long empty_bitsets_num;
	double empty_blocks_num;
	//	double unary_blocks_num;
	double blocks_num;
	int set_bits;
	double avg_set_bits;
	StorageSize size;
	empty_blocks_num = 0;
	empty_buckets_num = 0;
	empty_bitsets_num = 0;
	//	unary_blocks_num = 0;
	avg_set_bits = -1;
	set_bits = 0;

	HitmapBucket *tmp;

	size = this->byteSize();
	/*	for(int i=0; i<this->_buckets_num; i++)
		{
			if( this->_buckets[i]!=0 )
			{
				this->_buckets[i]->updateBlocksPool();
			}
		}
		SparseBitsetU::blocksPoolStats();*/

	cout << "=====" << endl
		 << " Hitmap statistics" << endl
		 << "=====" << endl;
	cout << " * groups: " << this->_group_status->size() << " (non-empty: " << this->_group_status->count() << ")" << endl;
	cout << " * size (actual): " << size._total_bytes << " Bytes" << endl;
	cout << " * size (without opts): " << size._total_bytes_without_opts << " Bytes" << endl;
	cout << " * hitmap size: " << size._hitmap_bytes << " Bytes" << endl;
	cout << " * pool size: " << size._pool_bytes << " Bytes (" << SparseBitsetC::poolSize() << " entries)" << endl;
	cout << " * buckets: all=" << this->_buckets_num;
	for (unsigned long i = 0; i < this->_buckets_num; i++)
	{
		tmp = this->getBucketPtr(i);
		if (tmp == 0)
		//		if( this->_buckets[i] == 0 )
		{
			empty_buckets_num++;
		}
		else
		{
			empty_bitsets_num += tmp->countEmptyBitsets();
			empty_blocks_num += tmp->countEmptyBlocks();
			blocks_num += tmp->countBlocks();

			//			empty_bitsets_num += this->_buckets[i]->countEmptyBitsets();
			//			empty_blocks_num += this->_buckets[i]->countEmptyBlocks();
			//			blocks_num += this->_buckets[i]->countBlocks();
			if (avg_set_bits == -1)
			{
				avg_set_bits = (double)tmp->avgSetBits();

				//				avg_set_bits = (double)this->_buckets[i]->avgSetBits();
			}
			else
			{
				avg_set_bits += (double)tmp->avgSetBits();
				//				avg_set_bits += (double) this->_buckets[i]->avgSetBits();

				avg_set_bits = (double)avg_set_bits / (double)2;
			}
			set_bits += tmp->countSetBits();
			//			set_bits+= this->_buckets[i]->countSetBits();
		}
	}
	cout << ", empty=" << empty_buckets_num << endl;
	cout << " * bitsets: all=" << ((this->_buckets_num - empty_buckets_num) * this->_partition_num) << ", empty=" << empty_bitsets_num << endl;
	cout << " * blocks: all=" << blocks_num << ", empty=" << empty_blocks_num << endl;
	cout << " * bitsets load: " << avg_set_bits << endl;
	cout << " * set bits in bitsets: " << set_bits << endl;
	cout << "=====" << endl;
	cout << endl;
}

void HitmapHash::print(void)
{
	HitmapBucket *tmp;
	for (unsigned long i = 0; i < this->_buckets_num; i++)
	{
		tmp = this->getBucketPtr(i);

		//		if( this->_buckets[i]!=0 )
		if (tmp != 0)
		{
			cout << "== Buck. " << i << " ==" << endl;
			tmp->print();
			//		this->_buckets[i]->print();
		}
	}
}

#endif

//========================================================================================
// This version of HitmapBucket does not use any memory optimizations.

#ifdef NO_MEM_OPTS

unsigned long HitmapBucket::_bs_num;
unsigned long HitmapBucket::_bs_size;
unsigned long HitmapBucket::_block_size;
SparseBitset *HitmapBucket::_zeros = NULL;

HitmapBucket::HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size)
{
	unsigned long i;

	// Store the number and size of bitsets in each hitmap.
	HitmapBucket::_bs_num = bs_num;
	HitmapBucket::_bs_size = bs_size;
	HitmapBucket::_block_size = block_size;

	if (HitmapBucket::_zeros == NULL)
	{
		HitmapBucket::_zeros = new SparseBitset(HitmapBucket::_bs_size, HitmapBucket::_block_size); // Create a bitset full of zeros
	}

	// Create the array of bitsets.
	this->_hitmap = new SparseBitset *[HitmapBucket::_bs_num];

	// Create the bitsets.
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		this->_hitmap[i] = 0;
		//		this->_hitmap_old[i] = 0; //TODELETE
	}
}

HitmapBucket::~HitmapBucket()
{
	for (int i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]!=0 ) //TODELETE
		//		{
		//			delete this->_hitmap_old[i]; //TODELETE
		//		}

		if (this->_hitmap[i] != 0)
		{
			delete this->_hitmap[i];
		}
	}
	//	delete [] this->_hitmap_old; //TODELETE

	delete[] this->_hitmap;

	//	HitmapBucket::_zeros_old = NULL; //TODELETE
	HitmapBucket::_zeros = NULL;
}

void HitmapBucket::setHit(unsigned long bs_id, unsigned long b_id)
{
	if (this->_hitmap[bs_id] == 0)
		this->_hitmap[bs_id] = new SparseBitset(HitmapBucket::_bs_size, HitmapBucket::_block_size);

	this->_hitmap[bs_id]->set(b_id);
}

void HitmapBucket::lsbShiftBitset(SparseBitset *result, unsigned long bs_id, unsigned long shift_by)
{
	if (this->_hitmap[bs_id] != 0)
	{
		(*result) = (*this->_hitmap[bs_id]);
		result->shiftLSB(shift_by);
	}
	else
	{
		(*result) = (*HitmapBucket::_zeros);
	}
}

void HitmapBucket::msbShiftBitset(SparseBitsetU *result, unsigned long bs_id, unsigned long shift_by)
{
	/*	if( this->_hitmap[bs_id]!=0 )
		{
			(*result) = (*this->_hitmap[bs_id]);
		}
		else
		{
			(*result) = (*HitmapBucket::_zeros);
		}*/
}

void HitmapBucket::printBitset(unsigned long bs_id)
{
	cout << "B" << bs_id << ": ";

	if (this->_hitmap[bs_id] == 0)
	{
		cout << "EMPTY" << endl;
	}
	else
	{
		this->_hitmap[bs_id]->print();
		cout << endl;
	}

	//	if( this->_hitmap_old[bs_id]== 0)
	//		cout<<"EMPTY"<<endl;
	//	else
	//		cout<<(*this->_hitmap_old[bs_id])<<endl;
}

void HitmapBucket::print()
{
	unsigned long i;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		// print the contents of the current bitset.
		this->printBitset(i);
	}
}

SparseBitset HitmapBucket::getBitset(unsigned long bs_id)
{
	if (this->_hitmap[bs_id] == 0)
	{
		SparseBitset tmp(HitmapBucket::_bs_size, HitmapBucket::_block_size);
		return tmp;
	}
	else
	{
		return (*this->_hitmap[bs_id]);
	}
}

unsigned long HitmapBucket::countZeros()
{
	unsigned long i;
	unsigned long zeros_cnt;
	zeros_cnt = 0;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]->none() )
		if (this->_hitmap[i]->empty())
		{
			zeros_cnt++;
		}
	}
	return zeros_cnt;
}

unsigned long HitmapBucket::countEmptyBitsets()
{
	unsigned long empty_bitsets_num;
	empty_bitsets_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] == 0)
		//		if( this->_hitmap_old[i] == 0 )
		{
			empty_bitsets_num++;
		}
	}
	return empty_bitsets_num;
}

double HitmapBucket::avgSetBits()
{
	double avg_set_bits;
	double bits_examined;
	avg_set_bits = 0;
	bits_examined = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]!=0 )
		if (this->_hitmap[i] != 0)
		{
			//			avg_set_bits += (double)this->_hitmap_old[i]->count();
			avg_set_bits += (double)this->_hitmap[i]->count();
			//			bits_examined+= this->_hitmap_old[i]->size();
			bits_examined += (double)this->_hitmap[i]->size();
		}
	}
	if (avg_set_bits != 0)
		avg_set_bits = avg_set_bits / bits_examined;
	return avg_set_bits;
}

double HitmapBucket::countEmptyBlocks()
{
	double empty_blocks_num;
	empty_blocks_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		{
			empty_blocks_num += this->_hitmap[i]->getEmptyBlocksNum();
		}
	}
	return empty_blocks_num;
}

double HitmapBucket::countUnaryBlocks()
{
	double blocks_num;
	blocks_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		{
			blocks_num += this->_hitmap[i]->getUnaryBlocksNum();
		}
	}
	return blocks_num;
}

double HitmapBucket::countBlocks()
{
	double blocks_num;
	blocks_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		blocks_num += (double)this->_hitmap[i]->getBlocksNum();
	}
	return blocks_num;
}

int HitmapBucket::countSetBits()
{
	int set_bits = 0;
	for (int i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		//		if(this->_hitmap_old[i]!=0 )
		{
			//			set_bits+=this->_hitmap_old[i]->count();
			set_bits += this->_hitmap[i]->count();
		}
	}
	return set_bits;
}

int HitmapBucket::countSetBits(unsigned long bs_id)
{
	//	return this->_hitmap_old[bs_id]->count();
	return this->_hitmap[bs_id]->count();
}

void HitmapBucket::removeZeros()
{
	unsigned long i;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]->none() )
		if (this->_hitmap[i]->empty())
		{
			delete this->_hitmap[i];
			this->_hitmap[i] = 0;
			//			delete this->_hitmap_old[i];
			//			this->_hitmap_old[i] = 0;
		}
	}
}

#endif

//========================================================================================
// HitmapBucket using the memory optimization of block pool and the bucket-vector.

#ifdef USE_POOL_AND_VECTORS

// Declare the static variables.
unsigned long HitmapBucket::_bs_num;
unsigned long HitmapBucket::_bs_size;
unsigned long HitmapBucket::_block_size;
SparseBitsetUltraC *HitmapBucket::_zeros = NULL;
SparseBitset *HitmapBucket::_zeros2 = NULL;

//====================
// HitmapBcuket

HitmapBucket::HitmapBucket(unsigned long bs_num, unsigned long bs_size, unsigned long block_size)
{
	unsigned long i;

	// Store the number and size of bitsets in each hitmap.
	HitmapBucket::_bs_num = bs_num;
	HitmapBucket::_bs_size = bs_size;
	HitmapBucket::_block_size = block_size;

	if (HitmapBucket::_zeros == NULL)
	{
		HitmapBucket::_zeros = new SparseBitsetUltraC(HitmapBucket::_bs_size, HitmapBucket::_block_size); // Create a bitset full of zeros
	}

	if (HitmapBucket::_zeros2 == NULL)
	{
		HitmapBucket::_zeros2 = new SparseBitset(HitmapBucket::_bs_size, HitmapBucket::_block_size);
	}

	// Create the array of bitsets.
	this->_hitmap = new SparseBitsetUltraC *[HitmapBucket::_bs_num];

	// Create the bitsets.
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		this->_hitmap[i] = 0;
	}
}

HitmapBucket::~HitmapBucket()
{
	for (int i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		{
			delete this->_hitmap[i];
		}
	}
	delete[] this->_hitmap;
	delete HitmapBucket::_zeros;
	HitmapBucket::_zeros = NULL;
	delete HitmapBucket::_zeros2;
	HitmapBucket::_zeros2 = NULL;
}

void HitmapBucket::setHit(unsigned long bs_id, unsigned long b_id)
{
	// Create the bitset if needed.
	if (this->_hitmap[bs_id] == 0)
		this->_hitmap[bs_id] = new SparseBitsetUltraC(HitmapBucket::_bs_size, HitmapBucket::_block_size);

	// Set the bit of the corresponding bitset.
	this->_hitmap[bs_id]->set(b_id);
}

void HitmapBucket::lsbShiftBitset(SparseBitset *result, unsigned long bs_id, unsigned long shift_by)
{
	if (this->_hitmap[bs_id] != 0) // if the bitset is not empty
	{
		result->decompress(this->_hitmap[bs_id]);
		result->shiftLSB(shift_by);
	}
	else // if the corresponding bitset is empty
	{
		(*result) = (*HitmapBucket::_zeros2);
	}
}

void HitmapBucket::printBitset(unsigned long bs_id)
{
	cout << "B" << bs_id << ": ";

	if (this->_hitmap[bs_id] == 0)
	{
		cout << "EMPTY" << endl;
	}
	else
	{
		this->_hitmap[bs_id]->print();
		cout << endl;
	}
}

void HitmapBucket::print()
{
	unsigned long i;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		// print the contents of the current bitset.
		this->printBitset(i);
	}
}

SparseBitsetUltraC HitmapBucket::getBitset(unsigned long bs_id)
{
	if (this->_hitmap[bs_id] == 0)
	{
		SparseBitsetUltraC tmp(HitmapBucket::_bs_size, HitmapBucket::_block_size);
		return tmp;
	}
	else
	{
		return (*this->_hitmap[bs_id]);
	}
}

unsigned long HitmapBucket::countZeros()
{
	unsigned long i;
	unsigned long zeros_cnt;
	zeros_cnt = 0;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i]->empty())
		{
			zeros_cnt++;
		}
	}
	return zeros_cnt;
}

unsigned long HitmapBucket::countEmptyBitsets()
{
	unsigned long empty_bitsets_num;
	empty_bitsets_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] == 0)
		{
			empty_bitsets_num++;
		}
	}
	return empty_bitsets_num;
}

double HitmapBucket::avgSetBits()
{
	double avg_set_bits;
	double bits_examined;
	avg_set_bits = 0;
	bits_examined = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]!=0 )
		if (this->_hitmap[i] != 0)
		{
			//			avg_set_bits += (double)this->_hitmap_old[i]->count();
			avg_set_bits += (double)this->_hitmap[i]->count();
			//			bits_examined+= this->_hitmap_old[i]->size();
			bits_examined += (double)this->_hitmap[i]->size();
		}
	}
	if (avg_set_bits != 0)
		avg_set_bits = avg_set_bits / bits_examined;
	return avg_set_bits;
}

double HitmapBucket::countEmptyBlocks()
{
	double empty_blocks_num;
	empty_blocks_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		{
			empty_blocks_num += this->_hitmap[i]->countEmptyBlocks();
		}
	}
	return empty_blocks_num;
}

/*
double HitmapBucket::countUnaryBlocks()
{
	double blocks_num;
	blocks_num = 0;
	for( unsigned long i=0; i<HitmapBucket::_bs_num; i++)
	{
		if( this->_hitmap[i]!=0 )
		{
			blocks_num += this->_hitmap[i]->getUnaryBlocksNum();
		}
	}
	return blocks_num;
}*/

double HitmapBucket::countBlocks()
{
	double blocks_num;
	blocks_num = 0;
	for (unsigned long i = 0; i < HitmapBucket::_bs_num; i++)
	{
		blocks_num += (double)this->_hitmap[i]->getBlocksNum();
	}
	return blocks_num;
}

int HitmapBucket::countSetBits()
{
	int set_bits = 0;
	for (int i = 0; i < HitmapBucket::_bs_num; i++)
	{
		if (this->_hitmap[i] != 0)
		//		if(this->_hitmap_old[i]!=0 )
		{
			//			set_bits+=this->_hitmap_old[i]->count();
			set_bits += this->_hitmap[i]->count();
		}
	}
	return set_bits;
}

int HitmapBucket::countSetBits(unsigned long bs_id)
{
	//	return this->_hitmap_old[bs_id]->count();
	return this->_hitmap[bs_id]->count();
}

void HitmapBucket::removeZeros()
{
	unsigned long i;
	for (i = 0; i < HitmapBucket::_bs_num; i++)
	{
		//		if( this->_hitmap_old[i]->none() )
		if (this->_hitmap[i]->empty())
		{
			delete this->_hitmap[i];
			this->_hitmap[i] = 0;
			//			delete this->_hitmap_old[i];
			//			this->_hitmap_old[i] = 0;
		}
	}
}

//====================
// ConterBitsets

CounterBitsets::CounterBitsets(unsigned long cnt_num, unsigned long t_max, unsigned long block_size)
{
	// Get the number of counters to be encoded.
	this->_cnt_num = cnt_num;

	// Compute the number of counter (and remainder) bitsets we need to have.
	this->_cnt_bitsets_num = ceil((float)log(t_max + 1) / (float)log(2));
	// ATTENTION! The counters could take any value in [0,t_max], thus we have t_max+1 possible values.

	// Initialize counter and remainder bitsets.
	//	this->_cnt_bitsets = new dynamic_bitset<>* [this->_cnt_bitsets_num];
	this->_cnt_bitsets_new = new SparseBitset *[this->_cnt_bitsets_num];
	//	this->_rem_bitsets = new dynamic_bitset<>* [this->_cnt_bitsets_num];
	this->_rem_bitsets_new = new SparseBitset *[this->_cnt_bitsets_num];
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		this->_cnt_bitsets[i] = new dynamic_bitset<>(this->_cnt_num);
		//		this->_rem_bitsets[i] = new dynamic_bitset<>(this->_cnt_num);
		this->_cnt_bitsets_new[i] = new SparseBitset(this->_cnt_num, block_size);
		this->_rem_bitsets_new[i] = new SparseBitset(this->_cnt_num, block_size);
	}
}

CounterBitsets::~CounterBitsets()
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		delete this->_cnt_bitsets[i];
		//		delete this->_rem_bitsets[i];
		delete this->_cnt_bitsets_new[i];
		delete this->_rem_bitsets_new[i];
	}
	//	delete [] this->_cnt_bitsets;
	//	delete [] this->_rem_bitsets;
	delete[] this->_cnt_bitsets_new;
	delete[] this->_rem_bitsets_new;
}

/*
void CounterBitsets::tSetFlagsCalc(dynamic_bitset<>** bitsets, unsigned long bitsets_num, unsigned long t, dynamic_bitset<>* result)
{
	unsigned long y;
	unsigned long w;
	int empty_num;
	empty_num = 0;

	//Find out if there are so much zero bitsets that there is no way to produce any results.
	for(w=0; w<bitsets_num; w++)
	{
		if( bitsets[w]->empty() )
		{
			empty_num++;
		}
	}
	if( empty_num>(bitsets_num-t)){ result->reset(); return;}

	//Compute the values of the counter bitsets.
	for(w=0; w<bitsets_num; w++)
	{
		if( !bitsets[w]->empty() )
		{
			(*this->_rem_bitsets[0]) = (*this->_cnt_bitsets[0]) & (*bitsets[w]); //first compute the remainder (requires the old counter)
			(*this->_cnt_bitsets[0]) ^= (*bitsets[w]); //compute the new counter

			for(y=1; y<this->_cnt_bitsets_num; y++)
			{
				(*this->_rem_bitsets[y]) = (*this->_cnt_bitsets[y]) & (*this->_rem_bitsets[y-1]); //first compute the remainder (requires the old counter)
				(*this->_cnt_bitsets[y]) ^= (*this->_rem_bitsets[y-1]); //compute the new counter
			}
		}
		else
		{
			//AND with zeros returns zeros

//NOTE: if the initial bitset is full of zeros, then ANDs have no effect on _cnt_bitsets[0] and
//_rem_bitsets[0] is full of zeros (ORs with zeros return zeros). However, this means that all
//the _cnt_bitsets[y] remain the same as initially set

//			for(y=0; y<this->_cnt_bitsets_num; y++)
//			{
//				(*this->_rem_bitsets[y]) = (*HitmapBucket::getZeros());
//			}
		}
	}

	//Compute the t-set flags bitset.
	dynamic_bitset<> t_bin(this->_cnt_num,t); //Get the binary representation of t.
	result->set(); //Just to be sure that initially is full of 1s.
	for( y=0; y<this->_cnt_bitsets_num; y++)
	{
		//Note that while this loop is executed, result remains equal to 11...1. It is modified
		//only at the last loop.
		if(t_bin.test(y))
		{
			(*result) &= (*this->_cnt_bitsets[y]); //Because 11...1 AND x = x
			//Note: I think that when I perform an AND is faster than assigning the value of the
			//bitset to the result. This has to do with the implementation. I want to test this
			//more...
			break;
		}
		//Note: if t_bin[y]==0 I do nothing because I had to perform an OR with 11...1 and this
		//results into 11...1 always.
	}

	for( y=y+1; y<this->_cnt_bitsets_num; y++)
	{
		if(!t_bin.test(y))
		{
			(*result) |= (*this->_cnt_bitsets[y]);
		}
		else
		{
			(*result) &= (*this->_cnt_bitsets[y]);
		}
	}
}*/

void CounterBitsets::tSetFlagsCalc(SparseBitset **bitsets, unsigned long bitsets_num, unsigned long t, SparseBitset *result)
{
	unsigned long y;
	unsigned long w;
	int empty_num;
	empty_num = 0;

	vector<int> blocks_to_be_tested;

	// Find out if there are so much zero bitsets that there is no way to produce any results.
	/*	for(w=0; w<bitsets_num; w++)
		{
			if( bitsets[w]->empty() )
			{
				empty_num++;
			}
		}
		if( empty_num>(bitsets_num-t)){ result->reset(); return;}*/

	int non_empty_num;
	for (y = 0; y < SparseBitsetC::getBlocksNumSt(); y++)
	{
		non_empty_num = 0;
		for (w = 0; w < bitsets_num; w++)
		{
			if (bitsets[w]->testBlock(y))
			{
				non_empty_num++;
			}
		}
		if (non_empty_num >= t)
		{
			blocks_to_be_tested.push_back(y);
		}
	}

	//	cout<<"blocks_to_be_tested="<<blocks_to_be_tested.size()<<endl;

	// Compute the values of the counter bitsets.
	for (w = 0; w < bitsets_num; w++)
	{
		if (!bitsets[w]->empty())
		{
			//			(*this->_rem_bitsets_new[0]) = (*this->_cnt_bitsets_new[0]) & (*bitsets[w]); //first compute the remainder (requires the old counter)
			//			(*this->_cnt_bitsets_new[0]) ^= (*bitsets[w]); //compute the new counter
			this->_rem_bitsets_new[0]->selective_and(this->_cnt_bitsets_new[0], bitsets[w], blocks_to_be_tested);
			this->_cnt_bitsets_new[0]->selective_xor(bitsets[w], blocks_to_be_tested); // compute the new counter

			for (y = 1; y < this->_cnt_bitsets_num; y++)
			{
				//				(*this->_rem_bitsets_new[y]) = (*this->_cnt_bitsets_new[y]) & (*this->_rem_bitsets_new[y-1]); //first compute the remainder (requires the old counter)
				//				(*this->_cnt_bitsets_new[y]) ^= (*this->_rem_bitsets_new[y-1]); //compute the new counter
				this->_rem_bitsets_new[y]->selective_and(this->_cnt_bitsets_new[y], this->_rem_bitsets_new[y - 1], blocks_to_be_tested);
				this->_cnt_bitsets_new[y]->selective_xor(this->_rem_bitsets_new[y - 1], blocks_to_be_tested); // compute the new counter
			}
		}
		else
		{
			/*			//AND with zeros returns zeros

			//NOTE: if the initial bitset is full of zeros, then ANDs have no effect on _cnt_bitsets[0] and
			//_rem_bitsets[0] is full of zeros (ORs with zeros return zeros). However, this means that all
			//the _cnt_bitsets[y] remain the same as initially set

						for(y=0; y<this->_cnt_bitsets_num; y++)
						{
							(*this->_rem_bitsets[y]) = (*HitmapBucket::getZeros());
						}*/
		}
	}

	// Compute the t-set flags bitset.
	dynamic_bitset<> t_bin(this->_cnt_num, t); // Get the binary representation of t.
	result->set();							   // Just to be sure that initially is full of 1s.
	for (y = 0; y < this->_cnt_bitsets_num; y++)
	{
		// Note that while this loop is executed, result remains equal to 11...1. It is modified
		// only at the last loop.
		if (t_bin.test(y))
		{
			(*result) &= (*this->_cnt_bitsets_new[y]); // Because 11...1 AND x = x
			// Note: I think that when I perform an AND is faster than assigning the value of the
			// bitset to the result. This has to do with the implementation. I want to test this
			// more...
			break;
		}
		// Note: if t_bin[y]==0 I do nothing because I had to perform an OR with 11...1 and this
		// results into 11...1 always.
	}

	for (y = y + 1; y < this->_cnt_bitsets_num; y++)
	{
		if (!t_bin.test(y))
		{
			(*result) |= (*this->_cnt_bitsets_new[y]);
		}
		else
		{
			(*result) &= (*this->_cnt_bitsets_new[y]);
		}
	}
}

void CounterBitsets::reset(void)
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		this->_cnt_bitsets[i]->reset();
		this->_cnt_bitsets_new[i]->reset();
	}
}

void CounterBitsets::print(void)
{
	for (unsigned long i = 0; i < this->_cnt_bitsets_num; i++)
	{
		//		cout<<"C"<<i<<": "<<(*this->_cnt_bitsets[i])<<endl;
		cout << "TODO UPDATE THIS CLASS IN ORDER TO SUPPORT NEW COUNTER BITSETS" << endl;
	}
}

//====================
// HitmapHash

HitmapHash::HitmapHash(string *dseq_ptr, unsigned long qthr, unsigned long part_shift, unsigned long flen, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, unsigned long buck_groups_num) : StrHash<HitmapBucket>(flen, buckets_num, buck_groups_num)
{
	string cur_str; // a string bucket
	unsigned long cur_bucket;

	this->_dseq_ptr = dseq_ptr;					 // get the pointer to the data sequence
	this->_dseq_len = this->_dseq_ptr->length(); // get the length of the data sequence
	this->_qthr = qthr;
	this->_flen = flen;
	this->_part_len = this->_flen + 2 * this->_qthr;							   // get the length of the parts
	this->_part_shift = part_shift;												   // get how many positions is shifted the one part for the next
	this->_partition_num = this->_part_len;										   // get the number of partitions
	this->_partition_size = ceil((float)this->_dseq_len / (float)this->_part_len); // get how many parts are in each partition
	this->_counters = NULL;														   // Initially no counters. We specify counters by calling initCounterBitsets().
	this->_fnum = fnum;
	this->_block_size = block_size;
	if (this->_block_size > this->_partition_size)
	{
		cout << "WARNING - [Creating hitmap hash] The sparse bitset blocks are defined to be larger than the hitmaps!" << endl;
		cout << "\t * hitmap size = " << this->_partition_size << endl;
		cout << "\t * block size = " << this->_block_size << endl;
	}
	this->_tsetflags_new = new SparseBitset(this->_partition_size, this->_block_size);
	this->_blocks_num = this->_tsetflags_new->getBlocksNum();

	// In order to achieve the same number of parts in all partitions we extend dseq by adding symbols at the end. These extra
	// symbols are not part of the alphabet.
	string tmp_str(this->_part_len - 1, '$');
	this->_dseq_len = this->_dseq_ptr->length(); // update the length of the data sequence
	// attention!! this may create problem in case of consecutive index creation on the same data seq

	// Scan dseq to fill the hash table buckets
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{
		// Get the corresponding dseq subsequence
		cur_str = this->_dseq_ptr->substr(p, this->_flen);

		// Test if the bucket group is going to have content.
		this->setGroupStatus(this->_hash_func(cur_str) % this->_buckets_num);
	}
	this->initBuckets();

	/*
	int* arr;
	arr = new int[10];
	for(int i=0; i<10; i++)
		cout<<"[i="<<i<<":] "<<arr[i]<<" "<<&arr[i]<<endl;

	&arr[2] = 0;

	for(int i=0; i<10; i++)
		cout<<"[i="<<i<<":] "<<&arr[i]<<endl;

	int xxx;
	cin>>xxx;*/

	this->extraStatistics();
}

void HitmapHash::initBuckets()
{
	string cur_str; // a string bucket
	unsigned long cur_bucket;

	// In order to achieve the same number of parts in all partitions we extend dseq by adding symbols at the end. These extra
	// symbols are not part of the alphabet.
	string tmp_str(this->_part_len - 1, '$');
	// attention!! this may create problem in case of consecutive index creation on the same data seq

	HitmapBucket **tmp;

	// Scan dseq to fill the hash table buckets
	for (unsigned long p = 0; p <= (this->_dseq_len - this->_flen); p++)
	{ // cout<<"p="<<p<<" out of "<<(this->_dseq_len-this->_flen)<<endl;
		// Get the corresponding dseq subsequence
		cur_str = this->_dseq_ptr->substr(p, this->_flen);

		// Get the current bucket id based on the hashing function.
		cur_bucket = (this->_hash_func(cur_str) % this->_buckets_num);
		tmp = this->getBucketPtrRef(cur_bucket);

		// If this is the first hit for this bucket.
		//		if( this->_buckets[cur_bucket]== 0)
		if ((*tmp) == 0)
		{
			//			this->_buckets[cur_bucket] = new HitmapBucket(this->_partition_num,this->_partition_size,this->_block_size);
			(*tmp) = new HitmapBucket(this->_partition_num, this->_partition_size, this->_block_size);
			//			cout<<"=== NEW HitmapBucket pointer! @ "<<tmp<<endl;
		}

		int xx;
		int yy;
		xx = 0;
		for (int i = (p % this->_part_len); xx < (this->_part_len - this->_flen + 1); i--)
		{
			if (i < 0)
			{
				i = this->_partition_num - 1;
			}
			//			this->_buckets[cur_bucket]->setHit(i,floor((double)p/(double)this->_part_len));
			(*tmp)->setHit(i, floor((double)p / (double)this->_part_len));

			xx++;
		}
	}
}

HitmapHash::~HitmapHash(void)
{
	for (unsigned long i = 0; i < this->_buck_groups_num; i++)
	{
		this->deleteGroup(i);
	}
	delete[] this->_buckets;
	delete this->_counters;
	delete this->_tsetflags_new;
	delete this->_group_status;
}

void HitmapHash::initCounterBitsets(unsigned long t_max)
{
	this->_counters = new CounterBitsets(this->_partition_size, t_max, this->_block_size);
}

void HitmapHash::findWindows(string &qseq)
{
	unsigned long alpha; // Variable to store the current equivalence class.
	unsigned long k;
	unsigned long tmp_1;  // auxiliary variable
	unsigned long bit_id; // auxiliary variable
	string *fragments;	  // Array containing the fragments of the qseq.

	// Check that initCounterBitsets was executed.
	if (this->_counters == NULL)
	{
		cout << "ERROR -- [Hitmap searching] Run initCounterBitsets before searching..." << endl;
		return;
	}

	unsigned long cur_bucket;

	// Get the query fragments.
	fragments = new string[this->_fnum];
	for (unsigned long z = 0; z < this->_fnum; z++)
	{
		fragments[z] = qseq.substr(z * this->_flen, this->_flen);
	}

	this->_wins.clear();
	this->_results.clear();

	unsigned long win_start; // the start position of a verification window
	unsigned long win_end;	 // the end position of a verification window

	unsigned long *i;
	unsigned long *j;
	unsigned long *Dj;

	//	dynamic_bitset<>** hitmaps;
	SparseBitset **hitmaps_new;

	i = new unsigned long[this->_fnum];
	j = new unsigned long[this->_fnum];
	Dj = new unsigned long[this->_fnum];

	//	hitmaps = new dynamic_bitset<>*[this->_fnum];
	hitmaps_new = new SparseBitset *[this->_fnum];
	for (k = 0; k < this->_fnum; k++)
	{
		//		hitmaps[k] = new dynamic_bitset<>; //Create empty bitsets
		hitmaps_new[k] = new SparseBitset(this->_partition_size, this->_block_size); // create empty bitsets
	}
	HitmapBucket *tmp;
	for (alpha = 0; alpha < this->_part_len; alpha++)
	{ // cout<<"alpha="<<alpha<<" of "<<this->_part_len<<endl;
		// Find the position of the first part.
		i[0] = (this->_part_len + alpha - this->_qthr) % this->_part_len;
		j[0] = floor((float)(this->_part_len + alpha - this->_qthr) / (float)this->_part_len);
		Dj[0] = 0;

		// Note: We use this->_part_len+alpha as example (the second part) just to avoid negatives

		// Get the hitmap of the first part.
		cur_bucket = (this->_hash_func(fragments[0]) % this->_buckets_num); // get the bucket
		tmp = this->getBucketPtr(cur_bucket);

		//		if( this->_buckets[cur_bucket]==0 )
		if (tmp == 0)
		{
			continue;
		}
		tmp->lsbShiftBitset(hitmaps_new[0], i[0], Dj[0]);

		//		this->_buckets[cur_bucket]->lsbShiftBitset(hitmaps_new[0],i[0],Dj[0]);

		/*		SparseBitset* tmp; tmp = new SparseBitset(this->_partition_size,this->_block_size);
				this->_buckets_alt[cur_bucket]->lsbShiftBitset(tmp,i[0],Dj[0]);
				if(tmp->count()!=hitmaps_new[0]->count()){ cout<<"========PROB1!!!!!"<<endl; int xxx; cin>>xxx;}*/

		/*

			if( !hitmaps[0]->empty())
				hitmaps_new[0]->compareWithRegBitset(hitmaps[0]); */
		/*			if( hitmaps[0]->empty() )
						hitmaps_new[0]->clear();
					else
						hitmaps_new[0]->setEfficient(hitmaps[0]);*/

		// Find the position and the hitmaps of the rest of the parts.
		for (k = 1; k < this->_fnum; k++)
		{
			i[k] = (i[k - 1] + this->_part_shift) % this->_partition_num;
			j[k] = floor((float)(this->_part_len + alpha - this->_qthr + k * this->_part_shift) / (float)this->_part_len);
			Dj[k] = j[k] - j[0];

			cur_bucket = (this->_hash_func(fragments[k]) % this->_buckets_num);
			tmp = this->getBucketPtr(cur_bucket);
			//			if( this->_buckets[cur_bucket]!=0)
			if (tmp != 0)
			{
				//				this->_buckets[cur_bucket]->lsbShiftBitset(hitmaps_new[k],i[k],Dj[k]);
				tmp->lsbShiftBitset(hitmaps_new[k], i[k], Dj[k]);

				/*			this->_buckets_alt[cur_bucket]->lsbShiftBitset(tmp,i[k],Dj[k]);
					if(tmp->count()!=hitmaps_new[k]->count())
					{
					cout<<"========"<<endl<<"PROB!!!!! buck:"<<cur_bucket<<", bitset:"<<i[k]<<endl;
					cout<<"OLD METHOD: "<<endl; tmp->print();
					cout<<"NEW METHOD: "<<endl; hitmaps_new[k]->print();int zzz; cin>>zzz;}*/

				/*				this->_buckets[cur_bucket]->lsbShiftBitset_old(hitmaps[k],i[k],Dj[k]);

				if( !hitmaps[k]->empty())
						hitmaps_new[k]->compareWithRegBitset(hitmaps[k]);*/

				/*				if( hitmaps[k]->empty() )
									hitmaps_new[k]->clear();
								else
									hitmaps_new[k]->setEfficient(hitmaps[k]);*/
			}
		}
		//		this->_counters->tSetFlagsCalc(hitmaps,this->_fnum,this->_qthr,tsetflags);
		this->_counters->tSetFlagsCalc(hitmaps_new, this->_fnum, (this->_fnum - this->_qthr), this->_tsetflags_new);
		this->_counters->reset(); // ATTENTION!!!! THIS IS NOT REQUIRED FOR THE NEW COUNTER BITSETS (THOSE USING SPARSEBITSETS)

		// cout<<"tsetflags:"<<endl;
		// this->_tsetflags_new->print();

		// Calculate the windows that need to be verified (based on this eq. class).
		for (k = 0; k < this->_blocks_num; k++) // get a block each time
		{
			if (this->_tsetflags_new->testBlock(k)) // check that there is at least one 1 in the block
			{
				for (tmp_1 = 0; tmp_1 < this->_block_size; tmp_1++) // check one by one the bits
				{
					if (this->_tsetflags_new->testInNonEmptyBlock(k, tmp_1))
					{
						bit_id = k * this->_block_size + tmp_1;
						if (bit_id < this->_tsetflags_new->size()) // otherwise it is one of the padding bits
						{
							win_start = bit_id * this->_part_len + i[0];

							// get the window verification
							this->_wins.push_back(win_start);
						}

						this->_tsetflags_new->flipNonEmptyBlock(k, tmp_1);
						if (this->_tsetflags_new->runNoneInBlock(k))
							break;
					}
				}
			}
		}

		/*		for( int ii=0; ii<this->_partition_size; ii++) //TODO MPOREI NA YPARXEI KALYTEROS TROPOS GIA AUTO
				{
		//			if( tsetflags->test(ii) )
					if( tsetflags_new->test(ii) )
					{
						win_start = ii*this->_part_len+i[0];
						//get the window verification
			//			this->_wins.push_back(win_start);
						cout<<"old: "<<win_start<<endl;
					}
				}*/
	}

	for (k = 0; k < this->_fnum; k++)
	{
		//		delete hitmaps[k];
		delete hitmaps_new[k];
	}
	//	delete [] hitmaps;
	delete[] hitmaps_new;

	delete[] i;
	delete[] j;
	delete[] Dj;

	delete[] fragments;

	//	cout<<"ALL OK!"<<endl;
}

void HitmapHash::verifyWindows(string &qseq)
{
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	ver_area_start = (*cur_win);
	ver_area_end = ver_area_start + qseq.length() + 2 * this->_qthr;
	if (ver_area_end >= this->_dseq_ptr->length())
		ver_area_end = last_pos;
	cur_win++;
	while (cur_win != this->_wins.end())
	{
		if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
		{
			dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
			ver_area_start = (*cur_win);
		}
		ver_area_end = (*cur_win) + qseq.length() + 2 * this->_qthr;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
	}
	dynamicProgramming(this->_dseq_ptr, ver_area_start, ver_area_end, &qseq, this->_qthr, &this->_results);
}

void HitmapHash::verifyWindows(string &qseq, unsigned long epsilon, unsigned long fnum)
{
	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	while (cur_win != this->_wins.end())
	{
		dynamicProgramming(this->_dseq_ptr, (*cur_win), ((*cur_win) + qseq.length() + epsilon), &qseq, epsilon, &this->_results);
		cur_win++;
	}
}

vector<int> HitmapHash::getWindows(void)
{
	return this->_wins;
}

vector<int> HitmapHash::getResults(void)
{
	this->keepUniqueResults();
	return this->_results;
}

void HitmapHash::keepUniqueWindows(void)
{
	vector<int>::iterator tmp_it;
	sort(this->_wins.begin(), this->_wins.end());
	tmp_it = unique(this->_wins.begin(), this->_wins.end());
	this->_wins.resize(std::distance(this->_wins.begin(), tmp_it));
}

void HitmapHash::keepUniqueResults(void)
{
	vector<int>::iterator tmp_it;
	sort(this->_results.begin(), this->_results.end());
	tmp_it = unique(this->_results.begin(), this->_results.end());
	this->_results.resize(std::distance(this->_results.begin(), tmp_it));
}

void HitmapHash::setVerificationStatistics(string &qseq)
{
	int ver_areas_num;
	int ver_size;
	ver_size = 0;
	ver_areas_num = 0;
	int ver_area_start;
	int ver_area_end;
	ver_area_end = -1;
	int last_pos;
	last_pos = this->_dseq_ptr->length() - 1;

	// verify the windows
	vector<int>::iterator cur_win; // the current verification window
	cur_win = this->_wins.begin();
	ver_area_start = (*cur_win);
	ver_area_end = ver_area_start + qseq.length() + 2 * this->_qthr;
	if (ver_area_end >= this->_dseq_ptr->length())
		ver_area_end = last_pos;
	cur_win++;
	while (cur_win != this->_wins.end())
	{
		if (ver_area_end < (*cur_win)) // new ver. area must be created - verify the previous
		{
			ver_areas_num++;
			ver_size += (ver_area_end - ver_area_start + 1);
			//		cout<<"ar. start: "<<ver_area_start<<", ar. end: "<<ver_area_end<<endl;

			ver_area_start = (*cur_win);
		}
		ver_area_end = (*cur_win) + qseq.length() + 2 * this->_qthr;
		if (ver_area_end >= this->_dseq_ptr->length())
			ver_area_end = last_pos;
		cur_win++;
	}
	ver_areas_num++;
	//	if(ver_area_end-ver_area_start+1<0){ cout<<"ver_area_end="<<ver_area_end<<", ver_area_start="<<ver_area_start<<endl; int xx; cin>>xx;}
	ver_size += (ver_area_end - ver_area_start + 1);

	this->_stats._ver_areas_num = ver_areas_num;
	this->_stats._data_size = this->_dseq_len;
	this->_stats._ver_size = ver_size;
	this->_stats._ver_size_percentage = (double)this->_stats._ver_size / (double)this->_stats._data_size;
}

VerificationStatistics HitmapHash::getVerificationStatistics(void)
{
	return this->_stats;
}

StorageSize HitmapHash::byteSize()
{
	StorageSize size;
	HitmapBucket *tmp;

	// Consider the pointers to the groups of buckets.
	size._total_bytes = this->_buck_groups_num * sizeof(HitmapHash **);
	size._total_bytes_without_opts = this->_buck_groups_num * sizeof(HitmapHash **);

	cout << "Group pointers size = " << size._total_bytes << endl;

	// Consider the pointers to the buckets.
	for (int i = 0; i < this->_buck_groups_num; i++)
	{
		if (this->_buckets[i] != 0)
		{
			size._total_bytes += this->_buckets_per_group * sizeof(HitmapHash *);
			size._total_bytes_without_opts += this->_buckets_per_group * sizeof(HitmapHash *);
		}
	}
	cout << "Non empty groups = " << this->_group_status->count() << endl;
	cout << "Considering bucket pointers the size is " << size._total_bytes << endl;

	// Consider the size of the buckets.
	for (int i = 0; i < this->_buckets_num; i++)
	{
		tmp = this->getBucketPtr(i); // cout<<"after"<<endl;
		if (tmp != 0)
		{
			size._total_bytes_without_opts += tmp->byteSize(true);
			size._hitmap_bytes += tmp->byteSize(false);
		}
	}
	size._total_bytes += size._hitmap_bytes;

	// Consider the size of the UVB pool.
	size._pool_bytes = SparseBitsetC::poolByteSize();
	size._total_bytes += size._pool_bytes;
	//	size._total_bytes_without_opts += size._pool_bytes;

	// Attention!! There is also the size of counters, however it is not fixed and it is
	// very small.
	return size;
}

void HitmapHash::extraStatistics()
{
	unsigned long empty_buckets_num;
	unsigned long empty_bitsets_num;
	double empty_blocks_num;
	//	double unary_blocks_num;
	double blocks_num;
	int set_bits;
	double avg_set_bits;
	StorageSize size;
	empty_blocks_num = 0;
	empty_buckets_num = 0;
	empty_bitsets_num = 0;
	//	unary_blocks_num = 0;
	avg_set_bits = -1;
	set_bits = 0;

	HitmapBucket *tmp;

	size = this->byteSize();
	/*	for(int i=0; i<this->_buckets_num; i++)
		{
			if( this->_buckets[i]!=0 )
			{
				this->_buckets[i]->updateBlocksPool();
			}
		}
		SparseBitsetU::blocksPoolStats();*/

	cout << "=====" << endl
		 << " Hitmap statistics" << endl
		 << "=====" << endl;
	cout << " * groups: " << this->_group_status->size() << " (non-empty: " << this->_group_status->count() << ")" << endl;
	cout << " * size (actual): " << size._total_bytes << " Bytes" << endl;
	cout << " * size (without opts): " << size._total_bytes_without_opts << " Bytes" << endl;
	cout << " * hitmap size: " << size._hitmap_bytes << " Bytes" << endl;
	cout << " * pool size: " << size._pool_bytes << " Bytes (" << SparseBitsetC::poolSize() << " entries)" << endl;
	cout << " * buckets: all=" << this->_buckets_num;
	for (unsigned long i = 0; i < this->_buckets_num; i++)
	{
		tmp = this->getBucketPtr(i);
		if (tmp == 0)
		//		if( this->_buckets[i] == 0 )
		{
			empty_buckets_num++;
		}
		else
		{
			empty_bitsets_num += tmp->countEmptyBitsets();
			empty_blocks_num += tmp->countEmptyBlocks();
			blocks_num += tmp->countBlocks();

			//			empty_bitsets_num += this->_buckets[i]->countEmptyBitsets();
			//			empty_blocks_num += this->_buckets[i]->countEmptyBlocks();
			//			blocks_num += this->_buckets[i]->countBlocks();
			if (avg_set_bits == -1)
			{
				avg_set_bits = (double)tmp->avgSetBits();

				//				avg_set_bits = (double)this->_buckets[i]->avgSetBits();
			}
			else
			{
				avg_set_bits += (double)tmp->avgSetBits();
				//				avg_set_bits += (double) this->_buckets[i]->avgSetBits();

				avg_set_bits = (double)avg_set_bits / (double)2;
			}
			set_bits += tmp->countSetBits();
			//			set_bits+= this->_buckets[i]->countSetBits();
		}
	}
	cout << ", empty=" << empty_buckets_num << endl;
	cout << " * bitsets: all=" << ((this->_buckets_num - empty_buckets_num) * this->_partition_num) << ", empty=" << empty_bitsets_num << endl;
	cout << " * blocks: all=" << blocks_num << ", empty=" << empty_blocks_num << endl;
	cout << " * bitsets load: " << avg_set_bits << endl;
	cout << " * set bits in bitsets: " << set_bits << endl;
	cout << "=====" << endl;
	cout << endl;
}

void HitmapHash::print(void)
{
	HitmapBucket *tmp;
	for (unsigned long i = 0; i < this->_buckets_num; i++)
	{
		tmp = this->getBucketPtr(i);

		//		if( this->_buckets[i]!=0 )
		if (tmp != 0)
		{
			cout << "== Buck. " << i << " ==" << endl;
			tmp->print();
			//		this->_buckets[i]->print();
		}
	}
}

#endif
