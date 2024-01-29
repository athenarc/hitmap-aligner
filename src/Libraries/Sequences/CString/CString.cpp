/************************************************************************************************//*
 * @file CString.h
 *
 * This file implements functions related to the generation of random sequences. 
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/
#include "CString.h"


CString::CString( string& or_string, string& alphabet)
{
	int ab_size; //The size of the alphabet.
	int str_byte_num; //The number of bytes needed for the whole compressed string.

	_length = or_string.length();
	ab_size = alphabet.length();
	_sym_size = ceil(log2(ab_size));
	str_byte_num = ceil((float)(((float)_length*_sym_size)/8));
	_compressed_str = new bitset<8>[str_byte_num];

	//Build a map for the alphabet <symbol,compr_sym>
	map<char,unsigned long> cmpr_map;
	unsigned long tmp = 0ul;
	//The rest of the bitsets...
	for( int i=0; i<ab_size; i++)
	{
		cmpr_map[alphabet[i]] = tmp;
		tmp++;
	}
	//@note These bitsets contain the bit-code for each symbol at their start (i.e., at the right).

	//Convert the original string into the compressed string. 
	int byte_num = 0;
	int bit_num = 0;
	int shift_amount = 0;
	int over = 0;
	for( int i=0; i<_length; i++)
	{
		bit_num += _sym_size;
		over = bit_num-8;
		if( over<0 )
		{
			map<char,unsigned long>::iterator cur_sym;
			cur_sym = cmpr_map.find(or_string[i]);
			bitset<8> tmp_bset((*cur_sym).second);
			tmp_bset<<=shift_amount;
			_compressed_str[byte_num] |= tmp_bset;
			cout<<"=="<<or_string[i]<<"== "<<tmp_bset<<" "<<shift_amount<<" "<<byte_num<<endl;
			shift_amount += _sym_size;
		}
		else
		{
			//First write the first part of the previous symbol.
			map<char,unsigned long>::iterator cur_sym;
			cur_sym = cmpr_map.find(or_string[i]);
			bitset<8> tmp_bset((*cur_sym).second);
			tmp_bset<<=shift_amount;
			_compressed_str[byte_num] |= tmp_bset;
			cout<<"=="<<or_string[i]<<"== "<<tmp_bset<<" "<<shift_amount<<" "<<byte_num<<endl;
			
			//update the shift_amount & byte number
			shift_amount = over;
			byte_num++;
			bit_num = 0;
			
			//Then write the second part (if there is any).
			if(over>0)
			{
				bitset<8> tmp_bset2((*cur_sym).second);
				tmp_bset2>>=over;
				_compressed_str[byte_num] |= tmp_bset2;
				cout<<"=="<<or_string[i]<<"== "<<tmp_bset2<<" "<<shift_amount<<" "<<byte_num<<endl;
			}
			

		}
	}
	
	
	//For debugging...
	map<char,unsigned long>::iterator cur_item;
	cur_item = cmpr_map.begin();
	while(cur_item!=cmpr_map.end())
	{
		bitset<8> tmp_bt((*cur_item).second);
		cout<<(*cur_item).first<<": "<<tmp_bt;
		cout<<endl;
		cur_item++;
	}
	cout<<"The size of the alphabet is: "<<ab_size<<endl;
	cout<<"The number of bits for each symbol is: "<<_sym_size<<endl;
	cout<<"The number of bytes for the string is: "<<str_byte_num<<endl;
	
	for( int i=0; i<str_byte_num; i++)
	{
		cout<<_compressed_str[i]<<endl;
	}
	
}

bitset<8> CString::operator[](int i)
{
	int abs_sym_start; //The bit from where the i-th symbol starts.
	int abs_sym_end; //The bit where the i-th symbol ends.
	int sym_start_byte_num;
	int sym_start_bit_num;
	int sym_end_byte_num;
	int sym_end_bit_num;
	abs_sym_start = i*_sym_size;
	abs_sym_end = abs_sym_start + _sym_size - 1;
	sym_start_byte_num = floor((float)abs_sym_start/8);
	sym_start_bit_num = abs_sym_start - (sym_start_byte_num)*8; 
	sym_end_byte_num = floor((float)abs_sym_end/8);
	sym_end_bit_num = abs_sym_end - (sym_end_byte_num)*8;
	
	cout<<"["<<abs_sym_start<<","<<abs_sym_end<<"] ["<<sym_start_byte_num<<":"<<sym_start_bit_num<<","<<sym_end_byte_num<<":"<<sym_end_bit_num<<"]"<<endl;
	
	bitset<8> third (string("00"));
	return third;
}