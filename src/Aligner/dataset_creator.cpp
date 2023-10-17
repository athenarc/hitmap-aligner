#include <iostream>
#include <fstream>
#include <cstdlib> //to use atoi etc.
#include "../Libraries/Misc/ArgReader.h"
#include "../Libraries/Files/Files.h"
#include "../Libraries/Misc/Utilities.h" //to use itoa
#include "../Libraries/Sequences/SeqGen/SeqGen.h"
using namespace std;

int main(int argc, char *argv[])
{
	int dlen;		 // The length of the data sequences to be created.
	int dnum;		 // The number of data sequences to be created.
	string ab_name;	 // The name of the alphabet used to create the data sequences.
	string alphabet; // The symbols of the alphabet.
	string loc;		 // The location where the files are going to be created (this is then combined with dlen and ab to form the full path).
	ifstream fin;
	ofstream fout;

	//====================================================================================
	// Create an argument reader - get the argument values and check that they are all ok.
	//

	string my_options[4];	   // The array containing the options.
	string my_descriptions[4]; // The array containing the descriptions of the options.
	my_options[0] = "-loc";
	my_descriptions[0] = "The location where the data sequence files are going to be created (required).";
	my_options[1] = "-dlen";
	my_descriptions[1] = "The length of the data sequences to be created (required).";
	my_options[2] = "-ab";
	my_descriptions[2] = "The name of the alphabet to be used (required).";
	my_options[3] = "-dnum";
	my_descriptions[3] = "The number of data sequences to be created (required).";
	ArgReader my_arg_reader(my_options, my_descriptions, 4);

	if (argc == 1)
	{
		cout << "ERROR -- [Reading options] Arguments are required!" << endl;
		my_arg_reader.help();
		return -1;
	}
	else // Check the arguments given
	{

		// Get the options determined in the command line by the user.
		string cur_option;
		my_arg_reader.read(argc, argv); // Read the user options.

		// Get the location of the files to be created, otherwise return error.
		cur_option = my_arg_reader.isActive("-loc");
		if (cur_option != "")
		{
			// Get the option value.
			loc = cur_option;
		}
		else
		{
			cout << "ERROR -- [Reading options] You must determine the location where the data sequence files are going to be created (option -loc)." << endl;
			my_arg_reader.help();
			return -1;
		}

		// Get the length of the sequences to be created, otherwise return error.
		cur_option = my_arg_reader.isActive("-dlen");
		if (cur_option != "")
		{
			// Get the option value.
			dlen = atoi(cur_option.c_str());
		}
		else
		{
			cout << "ERROR -- [Reading options] You must determine the length of the seuqneces to be created (option -dlen)." << endl;
			my_arg_reader.help();
			return -1;
		}

		// Get the name of the alphabet to be used for the sequences, otherwise return error.
		cur_option = my_arg_reader.isActive("-ab");
		if (cur_option != "")
		{
			// Get the option value.
			ab_name = cur_option;
		}
		else
		{
			cout << "ERROR -- [Reading options] You must determine the name of the alphabet (option -ab)." << endl;
			my_arg_reader.help();
			return -1;
		}

		// Get the number of the sequences to be created, otherwise return error.
		cur_option = my_arg_reader.isActive("-dnum");
		if (cur_option != "")
		{
			// Get the option value.
			dnum = atoi(cur_option.c_str());
		}
		else
		{
			cout << "ERROR -- [Reading options] You must determine the number of the sequences to be created (option -dnum)." << endl;
			my_arg_reader.help();
			return -1;
		}
	}

	// Check if there exist the data directory in the location.
	if (dirExists(loc))
	{
		cout << "ERROR -- [Reading data directory] Folder already exists." << endl;
		return -1;
	}

	// Get the name of the alphabet and its symbols from the configuration file in the folder.
	string ab_file;
	string cur_token;
	//	ab_file = loc+"/alphabets.conf";
	ab_file = "../../Data/Aligner/alphabets.conf";
	fin.open(ab_file.c_str());
	if (fin.fail())
	{
		cout << "ERROR -- [Opening file] Cannot open '" << ab_file << "'. No such file." << endl;
		cout << "A file with this name must be present in the data folder. It contains <ab_name,ab_symbols> pairs, one in each line of the file." << endl;
		return -1;
	}
	bool found;
	found = false;
	while (!fin.eof())
	{
		fin >> cur_token;

		if (cur_token == ab_name)
		{
			cout << "Found alphabet '" << ab_name << "' (";
			fin >> cur_token;
			alphabet = cur_token;
			cout << alphabet << ")" << endl;
			found = true;
			break;
		}
	}
	if (!found)
	{
		cout << "ERROR -- [Parsing file] Could not find this alphabet or alphabet file in bad format." << endl;
		return -1;
	}
	fin.close();

	// Create the alphabet subfolder if there is not present.
	/*	if( !dirExists(loc+"/"+ab_name) )
		{
			if( makeDir(loc+"/"+ab_name) )
			{
				cout<<"Successful creation of directory '"<<(loc+"/"+ab_name)<<"'."<<endl;
			}
			else
			{
				cout<<"ERROR -- [Creating data directory] An error occurred during the creation of the data directory..."<<endl;
				return -1;
			}
		}*/

	// Check if the alphabet and size subfolders are present already
	string final_dir;
	final_dir = loc;
	/*	final_dir = loc+"/"+ab_name+"/"+my_arg_reader.isActive("-dlen");
		if( dirExists(final_dir) )
		{
			char option;
			cout<<"Directory '"<<final_dir<<"' already exists. Do you want to (o)verwrite it (ALL EXISTING FILES WILL BE LOST!) or to (u)pdate it? ";
			cin>>option;
			if( option == 'o' )
			{
				cout<<"Deleting the existing directory and its contents... "<<endl;

				if( removeDir(final_dir) )
				{
					cout<<"OK!"<<endl;
				}
				else
				{
					cout<<"ERROR -- [Creating data directory] Error during the deletion of the old folder."<<endl;
					return -1;
				}

				cout<<"Creating a new (empty) directory in its place..."<<endl;
				if( makeDir(final_dir) )
				{
					cout<<"OK!"<<endl;
				}
				else
				{
					cout<<"ERROR -- [Creating data directory] Error during the creation of the new data directory..."<<endl;
					return -1;
				}
			}
			else if( option == 'u' )
			{
				cout<<"No need to create a folder."<<endl;
			}
			else
			{
				cout<<"ERROR -- [Creating data directory] Invalid option!!!"<<endl;
				return -1;
			}
		}
		else
		{*/
	if (makeDir(final_dir))
	{
		cout << "Successful creation of directory '" << final_dir << "'." << endl;
	}
	else
	{
		cout << "ERROR -- [Creating data directory] An error occurred during the creation of the data directory..." << endl;
		cout << "Director to be created '" << final_dir << endl;
		perror("mkdir");
		return -1;
	}
	//	}

	// Get the number of the largest file in the directory.
	int max_file_id;
	vector<string> filenames;
	vector<string> dirnames;
	if (listDir(final_dir, filenames, dirnames))
	{
		max_file_id = filenames.size();
	}
	else
	{
		cout << "ERROR -- [Reading directory] Error in reading files in directory..." << endl;
		return -1;
	}

	// Create the data files in the final directory
	SeqGen generator;
	for (int i = 0; i < dnum; i++)
	{
		string new_file;
		string *new_string;
		new_string = new string(dlen, '$');
		new_file = final_dir + "/" + itoa(max_file_id + i + 1) + ".d";
		cout << "Creating file " << (i + 1) << " (with name '" << new_file << "')..." << endl;

		// Creating the sequence
		generator.createSeq(dlen, alphabet, new_string);

		// Creating the file
		fout.open(new_file.c_str());
		if (fout.fail())
		{
			cout << "ERROR -- [Opening file] Cannot open '" << new_file << "' for writing..." << endl;
			return -1;
		}
		fout << (*new_string);
		fout.close();

		cout << "Ok!" << endl;
	}

	return 0;
}
