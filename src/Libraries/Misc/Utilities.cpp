#include "Utilities.h"

string itoa( int num )
{
	stringstream ss( stringstream::in| stringstream::out);
	string sbuffer;
	
	//convert num to string
	sbuffer = "";
	ss<<num;
	sbuffer = ss.str();
	
	//clear stringstream
	ss.str(std::string());
	ss.clear();
	
	return sbuffer;
}

string ltoa( long int num)
{
	stringstream ss( stringstream::in| stringstream::out);
	string sbuffer;
	
	//convert num to string
	sbuffer = "";
	ss<<num;
	sbuffer = ss.str();
	
	//clear stringstream
	ss.str(std::string());
	ss.clear();
	
	return sbuffer;
}

string dtoa( double num )
{
	stringstream ss( stringstream::in| stringstream::out);
	string sbuffer;
	
	//convert num to string
	sbuffer = "";
	ss<<num;
	sbuffer = ss.str();
	
	//clear stringstream
	ss.str(std::string());
	ss.clear();
	
	return sbuffer;
}

vector<string> tokenize( string str, char delimiter)
{
	vector<string> tokens; //the vector containing the tokens of the string
	
	std::size_t new_pos;
	std::size_t prev_pos;
	
	prev_pos = 0;
	new_pos = 0;
	
	string tmp_buf; //temp buffer
	
	while( new_pos!=std::string::npos )
	{
		new_pos = str.find(delimiter,prev_pos);
		tmp_buf = str.substr(prev_pos,new_pos-prev_pos);
		if( tmp_buf!="")
			tokens.push_back(tmp_buf);
		prev_pos = new_pos+1;
	}
	
	return tokens;	
}


vector<string> tokenize( string str, string delimiter)
{
	vector<string> tokens; //the vector containing the tokens of the string
	
	std::size_t new_pos;
	std::size_t prev_pos;
	
	prev_pos = 0;
	new_pos = 0;
	
	string tmp_buf; //temp buffer
	
	while( new_pos!=std::string::npos )
	{ 
		new_pos = str.find(delimiter,prev_pos);
		tmp_buf = str.substr(prev_pos,new_pos-prev_pos);
		if( tmp_buf!="")
			tokens.push_back(tmp_buf);
		prev_pos = new_pos+delimiter.length();
	}
	
	return tokens;	
}
