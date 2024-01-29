#include <iostream>
#include <fstream>
#include <cstdlib> //to use atoi etc.
#include "../Libraries/Misc/ArgReader.h"
#include "../Libraries/Sequences/SHash/HitmapHash.h"
using namespace std;
using namespace hmp;

int main(int argc, char *argv[])
{
	// Problem parameters
	unsigned int dlen; // The data sequence length.
	unsigned int qlen; // The query sequence length.
	unsigned int qthr; // The alignment threshold.
	string ab;		   // The alphabet.

	// System constants
	double cost_dp;	  // The cost of dynamic programming per cell.
	double cost_bit;  // The cost of bitwise operations per bit.
	double cost_prob; // The cost of probing a hash table.

	// Other variables
	string exp_conf_file;  // the configuration file of the experiment
	string sys_const_file; // file containing the constants of our system
	ofstream fout;
	ifstream fin;
	string ab_name; // the name of the alphabet
	string ab_file; // the full path of the alphabet file
	string dseq;	// the data sequence
	string dset;
	string dfolder;
	string data_file;
	string rand;

	//====================================================================================
	// Create an argument reader - get the argunent values and check that they are all ok.
	//

	string my_options[3];	   // The array containing the options.
	string my_descriptions[3]; // The array containing the descriptions of the options.
	my_options[0] = "-exp";
	my_descriptions[0] = "Determines the location of the configuration file that contains the details of the experiment (required).";
	my_options[1] = "-sys";
	my_descriptions[1] = "The file that contains the system constant values (required).";
	my_options[2] = "-rand";
	my_descriptions[2] = "If random queries 'y'. If part of the data 'n' (required) ATTENTION!! 'n' DOES NOT WORK!! THUS WE ALTER 'y' IN 'n'.";
	ArgReader my_arg_reader(my_options, my_descriptions, 3);
	bool found_all;
	found_all = false;

	if (argc == 1) // Check if no arguments are given
	{
		cout << "=> ERROR: Arguments are missing." << endl;
		my_arg_reader.help();
		return -1;
	}
	else // Check the arguments given
	{
		// Get the options determined in the command line by the user.
		string cur_option;
		my_arg_reader.read(argc, argv); // Read the user options.

		cur_option = my_arg_reader.isActive("-exp");
		if (cur_option != "")
		{
			// Get the option value.
			exp_conf_file = cur_option;
		}
		else
		{
			cout << "=> ERROR: Determine the location of the configuration file of the experiment (option -exp)." << endl;
			my_arg_reader.help();
			return -1;
		}

		cur_option = my_arg_reader.isActive("-sys");
		if (cur_option != "")
		{
			// Get the option value.
			sys_const_file = cur_option;
		}
		else
		{
			cout << "=> ERROR: Determine the location of the file that contains the system constants (option -sys)." << endl;
			my_arg_reader.help();
			return -1;
		}

		cur_option = my_arg_reader.isActive("-rand");
		if (cur_option != "")
		{
			// Get the option value.
			rand = cur_option;
			rand = "y"; // ATTENTION!!! FOUND THAT THIS DOES NOT BRINGS GOOD RESULTS
		}
		else
		{
			cout << "=> ERROR: Determine how the query was selected (option -rand)." << endl;
			my_arg_reader.help();
			return -1;
		}
	}

	//====================================================================================
	// Read the configuration file to get the parameters of the experiment.
	//
	string cur_token; // A buffer used to store the current token
	fin.open(exp_conf_file.c_str());
	if (fin.fail())
	{
		cout << "ERROR -- [Opening configuration file] Cannot open '" << exp_conf_file << "'. No such file." << endl;
		return -1;
	}

	cout << "\t"
		 << "- Parsing configuration file..." << endl;
	string exp_params[10];
	exp_params[0] = "ab";
	exp_params[1] = "dfolder";
	exp_params[2] = "dset";
	exp_params[3] = "qmode";
	exp_params[4] = "qnum";
	exp_params[5] = "qlen";
	exp_params[6] = "qdis";
	exp_params[7] = "qthr";
	exp_params[8] = "inum";
	exp_params[9] = "method";
	int cur_param = 0;

	while (!fin.eof())
	{
		fin >> cur_token; // Get the next token.

		if (cur_token == exp_params[cur_param])
		{
			if (cur_param != 9)
			{
				cur_param++; // Get the next parameter
			}
			else // parameter #8 ("method") can have many entries (one in each line)
			{
				found_all = true;
			}
		}
		else if (cur_param == 1) // handle parameter 'ab'
		{
			ab_name = cur_token;
		}
		else if (cur_param == 2) // handle parameter 'dfolder'
		{
			dfolder = cur_token;
		}
		else if (cur_param == 3) // handle parameter 'dset'
		{
			dset = cur_token;
		}
		else if (cur_param == 6) // handle parameter 'qlen'
		{
			qlen = atoi(cur_token.c_str());
		}
		else if (cur_param == 8) // handle parameter 'qthr'
		{
			qthr = atoi(cur_token.c_str());
		}
	}
	fin.close();

	// === Get the alphabet
	ab_file = dfolder + "/alphabets.conf";
	fin.open(ab_file.c_str());
	if (fin.fail())
	{
		cout << "=> ERROR: Cannot open '" << ab_file << "' (alphabet file). No such file." << endl;
		return -1;
	}
	bool found;
	found = false;
	while (!fin.eof())
	{
		fin >> cur_token;
		if (cur_token == ab_name)
		{
			fin >> cur_token;
			ab = cur_token;
			found = true;
			break;
		}
	}
	if (!found)
	{
		cout << "=> ERROR: Cannot find '" << ab_name << "' or file is in bad format." << endl;
		return -1;
	}
	fin.close();
	cout << "\t\t"
		 << "* ab_name:\t" << ab_name << "\t['" << ab << "', size=" << ab.length() << ")" << endl;

	//=== Get dlen (from the first data seq file). ===
	data_file = dfolder + "/" + ab_name + "/" + dset + "/1.d"; // Open the first data file (there can be many)
	fin.open(data_file.c_str());
	if (fin.fail())
	{
		cout << "=> ERROR: Cannot open '" << data_file << "' (data seq file). No such file." << endl;
		return -1;
	}
	fin >> dseq;
	fin.close();
	dlen = dseq.length() + qlen + 1; // NOTE: We add qlen+1 '$' symbols at the start of dseq to avoid alignments at the start (may cause problems).
	cout << "\t\t"
		 << "* dlen:\t\t" << dlen << "\t[in file '" << data_file << "']" << endl;

	//=== Print the other parameters.
	cout << "\t\t"
		 << "* qlen:\t\t" << qlen << endl;
	cout << "\t\t"
		 << "* qthr:\t\t" << qthr << endl
		 << endl;

	cout << "\t"
		 << "- Parsing the system constants file..." << endl;
	fin.open(sys_const_file.c_str());
	if (fin.fail())
	{
		cout << "=> ERROR: Cannot open '" << sys_const_file << "' (system const. file). No such file." << endl;
		return -1;
	}
	while (!fin.eof())
	{
		fin >> cur_token;
		if (cur_token == "cost_dp")
		{
			fin >> cur_token;
			cost_dp = atof(cur_token.c_str());
		}
		else if (cur_token == "cost_prob")
		{
			fin >> cur_token;
			cost_prob = atof(cur_token.c_str());
		}
		else if (cur_token == "cost_bit")
		{
			fin >> cur_token;
			cost_bit = atof(cur_token.c_str());
		}
		else
		{
			cout << "=> ERROR: System configuration file in bad format." << endl;
			return -1;
		}
	}
	cout << "\t\t"
		 << "* cost_dp:\t" << cost_dp << endl;
	cout << "\t\t"
		 << "* cost_prob:\t" << cost_prob << endl;
	cout << "\t\t"
		 << "* cost_bit:\t" << cost_bit << endl
		 << endl;

	bool random_queries;
	/*	string option;
		cout<<"\t"<<"- Random queries? (y=query is random seq, n=query is subseq of data) ";
		cin>>option;*/
	if (rand == "y")
	{
		random_queries = true;
	}
	else if (rand == "n")
	{
		random_queries = false;
	}
	else
	{
		cout << "=> ERROR: Unkown parameter rand." << endl;
	}

	//=== Compute the minimum phi value ==
	cout << "\t"
		 << "- Estimating cost for various phi values..." << endl;
	unsigned int phi;
	double min_cost = 1000000000000;
	for (phi = (qthr + 1); phi < qlen + 1; phi++)
	{
		double cost_ph1;
		double cost_ph2;
		double cost;
		double areas;

		cost_ph1 = HitmapHash::estimateFiltCost(dlen, qlen, qthr, phi, cost_prob, cost_bit);
		cost_ph2 = HitmapHash::estimateVerCost(dlen, qlen, qthr, phi, ab.length(), cost_dp, random_queries);
		cost = cost_ph1 + cost_ph2;
		areas = HitmapHash::estimateVerAreas(dlen, phi, (floor((double)qlen / (double)phi)), qthr, ab.length());

		cout << "\t\t"
			 << " phi=" << phi << ": ";
		if (min_cost > cost)
		{
			min_cost = cost;
			cout << "M";
		}
		cout << "\t" << cost << " \t[flen=" << (floor((double)qlen / (double)phi)) << "]\t areas=" << areas << ", ph1=" << cost_ph1 << ", ph2=" << cost_ph2 << endl;

		if (floor((double)qlen / (double)phi) == 1)
			break; // no need for more fragments
	}

	return 0;
}