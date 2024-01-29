#include <iostream>
#include <fstream>
#include <cstdlib> //to use atoi etc.
#include "../Libraries/Misc/ArgReader.h"
#include "../Libraries/Misc/Utilities.h" //to use itoa
#include "../Libraries/Sequences/SeqGen/SeqGen.h"

using namespace std;

int main(int argc, char *argv[])
{
	string conf; // The location of the configuration file.
	string mode; // The mode of query creation.
	ifstream fin;
	ofstream fout;
	string conf_file;
	string dfolder;
	string dset;
	unsigned long dnum;
	string datafile;

	//====================================================================================
	// Create an argument reader - get the argunent values and check that they are all ok.
	//

	string my_options[2];	   // The array containing the options.
	string my_descriptions[2]; // The array containing the descriptions of the options.
	my_options[0] = "-conf";
	my_options[1] = "-mode";
	my_descriptions[0] = "The location where the configuration file resides (required).";
	my_descriptions[1] = "Two modes. 'datasub' (sequence is taken from the data sequence), 'rand' (random errors in seq)";
	ArgReader my_arg_reader(my_options, my_descriptions, 2);

	if (argc == 1)
	{
		cout << "=> ERROR: Arguments are missing." << endl;
		my_arg_reader.help();
		cout << endl;
		return -1;
	}
	else // Check the arguments given
	{
		// Get the options determined in the command line by the user.
		string cur_option;
		my_arg_reader.read(argc, argv); // Read the user options.

		cur_option = my_arg_reader.isActive("-conf");
		if (cur_option != "")
		{
			// Get the option value.
			conf = cur_option;
		}
		else
		{
			cout << "=> ERROR: Determine the location of the configuration file (option -conf)." << endl;
			my_arg_reader.help();
			return -1;
		}

		// Get the location of the files to be created, otherwise return error.
		cur_option = my_arg_reader.isActive("-mode");
		if (cur_option != "")
		{
			// Get the option value.
			mode = cur_option;
		}
		else
		{
			cout << "=> ERROR: Determine the mode as 'datasub' or 'rand' (option -mode)." << endl;
			my_arg_reader.help();
			return -1;
		}
	}

	// Get the parameter values from the configuration file.
	string cur_token;
	fin.open(conf.c_str());
	if (fin.fail())
	{
		cout << "=> ERROR: Cannot open '" << conf << "' (conf. file). No such file." << endl;
		return -1;
	}
	// the parameters
	string ab_name; // the name of the alphabet
	string dloc;	// the location of the directory containing all the data files
	string subfolder;
	int qnum;	 // the number of query sequences
	int qlen;	 // the length of each of the query sequences
	string qdis; // the distance function used
	int qthr;	 // the query threshold
	int inum;	 // the number of iterations
	bool qrand;
	qrand = true;
	string qmode;

	cout << "\t"
		 << "- Parsing the configuration file..." << endl;
	while (!fin.eof())
	{
		fin >> cur_token;
		cout << "\t\t"
			 << "* cur_token reading '" << cur_token << "'" << endl;

		if (cur_token == "dfile")
		{
			fin >> cur_token;
			datafile = cur_token;
			cout << "\t data file: " << cur_token << endl;
		}
		else if (cur_token == "qnum")
		{
			fin >> cur_token;
			qnum = atoi(cur_token.c_str());
			cout << "\t num of queries: " << qnum << endl;
		}
		else if (cur_token == "qlen")
		{
			fin >> cur_token;
			qlen = atoi(cur_token.c_str());
			cout << "\t length of queries: " << qlen << endl;
		}
		else if (cur_token == "qthr")
		{
			fin >> cur_token;
			qthr = atoi(cur_token.c_str());
			cout << "\t qthr: '" << qthr << "'." << endl;
		}
		else if (cur_token == "meas")
		{
			fin >> qdis;
			cout << "\t distance function: '" << qdis << "'." << endl;
		}
		else if (cur_token == "inum")
		{
			fin >> cur_token;
			inum = atoi(cur_token.c_str());
			cout << "\t num of iterations: " << inum << endl;
		}
	}
	fin.close();

	char is_ok;
	cout << "\t"
		 << "- ATTENTION! Read carefully the above mentioned parameter values. Are all the values what you expect (y/n)? ";
	cin >> is_ok;
	if (is_ok != 'y')
	{
		cout << "Program is going to terminate. Please edit the configuration file and try again." << endl;
		return -1;
	}

	// Creating the file that contains the query sequences.
	string qfile;							   // the filename of the query seq file.
	qfile = conf;							   // initialize the query seq file to have the same name as the conf file.
	qfile.replace(conf.length() - 5, 5, ".q"); // replace the .conf by .q
	cout << "Trying to create the file that contains the queries under the name '" << qfile << "'." << endl;
	fout.open(qfile.c_str());
	if (fout.fail())
	{
		cout << "=> ERROR: Cannot open '" << qfile << "' for writing..." << endl;
		return -1;
	}

	srand(time(NULL)); // Initialize random seed //TODO: Delete it!
	SeqGen generator;

	/*	conf_file = dfolder+"/"+ab_name+"/"+dset+"/data.conf"; //conf file containing the number of data files

		cout<<"\t"<<"- Reading number of data sequences..."<<endl;
		fin.open(conf_file.c_str());
		if( fin.fail() )
		{
			cout<<"=> ERROR: Cannot open '"<<conf_file<<" configuration file."<<endl;
			return -1;
		}
		fin>>dnum;
		fin.close();*/

	/*	if( qmode=="one2one" && dnum!=qnum )
		{
			cout<<"=> ERROR: Different number of data files than qnum="<<qnum<<endl;
			return -1;
		}
		else if( qmode=="many2one" && dnum!=1 )
		{
			cout<<"=> ERROR: In mode 'many2one' there must be only one data sequence."<<endl;
			return -1;
		}*/
	cout << "==> ATTENTION!!!!!! " << endl
		 << "==> ONLY DNA_CAP IS SUPPORTED!!!!!!!!!!!!" << endl
		 << "(and REAL_H is okay)" << endl;
	cout << qnum << endl;

	for (int i = 1; i <= qnum; i++)
	{
		string dataseq;
		string conf_file;
		int dlen;
		int dnum;

		if (mode == "datasub")
		{
			cout << "\t"
				 << "- Trying to read the sequence from '" << datafile << "'" << endl;
			fin.open(datafile.c_str());
			if (fin.fail())
			{
				cout << "ERROR -- [Opening file] Cannot open '" << datafile << "'..." << endl;
				return -1;
			}
			cout << dataseq << endl;
			fin >> dataseq;
			fin.close();
			cout << "\t"
				 << "- Ok!" << endl;
			dlen = dataseq.length();
			int pos_in_data;
			pos_in_data = rand() % (dlen - qlen);

			string fileline;

			while (true)
			{
				fileline = datafile + " " + dataseq.substr(pos_in_data, qlen);

				if (dataseq.substr(pos_in_data, qlen).find('N') == std::string::npos)
				{
					break;
				}
				else
				{
					cout << "\t Retrying because substring contains 'N'..." << endl;
					pos_in_data = rand() % (dlen - qlen);
				}
			}
			
			fout << fileline << endl;
		}
		else if (mode == "rand")
		{
			cout << "\t"
				 << "- Trying to read the sequence from '" << datafile << "'" << endl;
			fin.open(datafile.c_str());
			perror("open");
			if (fin.fail())
			{
				cout << "ERROR -- [Opening file] Cannot open '" << datafile << "'..." << endl;
				return -1;
			}
			fin >> dataseq;
			fin.close();
			cout << "\t"
				 << "- Ok!" << endl;
			dlen = dataseq.length();
			cout<< "Dlen: " << dlen << endl;
			
			int pos_in_data;
			pos_in_data = rand() % (dlen - qlen);
			string fileline;

			while (true)
			{
				string query_seq = dataseq.substr(pos_in_data, qlen);
				if (query_seq.find('N') == std::string::npos)
				{
					cout << "Seq: " << query_seq << endl;
					// including errors
					int err_num;
					int err_pos;
					err_num = rand() % (qthr + 1);
					cout << "\t-Query " << i << " to have " << err_num << " errors..." << endl;

					for (int j = 0; j < err_num; j++)
					{
						err_pos = rand() % qlen;
						if (query_seq[err_pos] == 'T')
							query_seq[err_pos] = 'C';
						else
							query_seq[err_pos] = 'T';
					}
					fileline = datafile + " " + query_seq;

					break;
				}
				else
				{
					
					//cout << "\t Retrying because substring contains 'N'..." << endl;
					pos_in_data = rand() % (dlen - qlen);
					//cout << pos_in_data << endl;

				}
			}
			
			fout << fileline << endl;

			/*			if( ab_name=="real_h" ) //if real human dataset
						{
							alphabet = "ACGT"; //without N
						}

						cout<<"Trying to create random sequence."<<endl;
						string* new_string;
						new_string = new string(qlen,'$');
						generator.createSeq( qlen, alphabet, new_string);
						cout<<"ok!"<<endl;



						string fileline;


						fileline = datafile+" "+(*new_string);




						fout<<fileline<<endl;*/
		}
		else
		{
			cout << "ERROR -- Unknown mode." << endl;
		}
	}
	fout.close();

	return 0;
}
