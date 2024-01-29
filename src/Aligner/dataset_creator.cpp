#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <filesystem>
#include <cstdlib> //to use atoi etc.
#include "../Libraries/Misc/ArgReader.h"
#include "../Libraries/Files/Files.h"
#include "../Libraries/Misc/Utilities.h" //to use itoa
#include "../Libraries/Sequences/SeqGen/SeqGen.h"
using namespace std;
// A COPY OF DATASET CREATOR
// THE IDEA IS TO ADD test.cpp code IN DATSET CREATOR

int main(int argc, char *argv[])
{
	int dlen;		 // The length of the data sequences to be created.
	int dnum;		 // The number of data sequences to be created.
	string ab_name;	 // The name of the alphabet used to create the data sequences.
	string alphabet; // The symbols of the alphabet.
	string loc;		 // The location where the files are going to be created (this is then combined with dlen and ab to form the full path).
	string mode;
	ifstream fin;
	ofstream fout;

	//====================================================================================
	// Create an argument reader - get the argument values and check that they are all ok.
	//
	string my_options[7];	   // The array containing the options.
	string my_descriptions[7]; // The array containing the descriptions of the options.

	my_options[0] = "-mode";
	my_descriptions[0] = "Two modes. 'imp' (dataset will be imported from the device), 'new' (dataset will be generated) (required for both methods).";
	my_options[1] = "-loc";
	my_descriptions[1] = "The location where the data sequence files are going to be created (required for both methods).";
	my_options[2] = "-dlen";
	my_descriptions[2] = "The length of the data sequences to be created (required for 'imp' method).";
	my_options[3] = "-ab";
	my_descriptions[3] = "The name of the alphabet to be used (required for 'imp' method).";
	my_options[4] = "-dnum";
	my_descriptions[4] = "The number of data sequences to be created (required for 'imp' method).";
	my_options[5] = "-df";
	my_descriptions[5] = "Determines the location of the dataset to be imported (required for 'new' method).";
	my_options[6] = "-chr";
	my_descriptions[6] = "The chromosome for the dataset (required for 'new' method).";

	ArgReader my_arg_reader(my_options, my_descriptions, 7);
	string missing_value = my_arg_reader.check(argc, argv);

	if (missing_value != "none")
	{
		cout << "ERROR -- [Reading options] Missing value for parameter " << missing_value << ". Please run again." << endl;
		my_arg_reader.help();
		return -1;
	}

	if (argc == 1)
	{
		cout << "ERROR -- [Reading options] Arguments are required!" << endl;
		my_arg_reader.help();
		return -1;
	}
	else // Check the arguments given
	{

		string cur_option;
		my_arg_reader.read(argc, argv); // Read the user options.
		cur_option = my_arg_reader.isActive("-mode");

		if (cur_option != "")
		{
			// Get the option value.
			if (cur_option != "imp" && cur_option != "new")
			{
				cout << "ERROR -- [Reading options] Incorrect mode. You must determine whether you want to import a dataset ('imp') or generate one ('new')." << endl;
				my_arg_reader.help();
				return -1;
			}
			mode = cur_option;
		}
		else
		{
			cout << "ERROR -- [Reading options] You must determine whether you want to import a dataset ('imp') or generate one ('new')." << endl;
			my_arg_reader.help();
			return -1;
		}

		if (mode == "new")
		{

			string my_options_new[4];	   // The array containing the options.
			string my_descriptions_new[4]; // The array containing the descriptions of the options.

			my_options_new[0] = my_options[1];
			my_descriptions_new[0] = my_descriptions[1];
			my_options_new[1] = my_options[2];
			my_descriptions_new[1] = my_descriptions[2];
			my_options_new[2] = my_options[3];
			my_descriptions_new[2] = my_descriptions[3];
			my_options_new[3] = my_options[4];
			my_descriptions_new[3] = my_descriptions[4];

			ArgReader my_arg_reader_new(my_options_new, my_descriptions_new, 4);
			// Get the length of the sequences to be created, otherwise return error.

			cur_option = my_arg_reader.isActive("-loc");
			if (cur_option != "")
			{
				// Get the option value.
				loc = cur_option;
			}
			else
			{
				cout << "ERROR -- [Reading options] You must determine the location where the data sequence files are going to be created (option -loc)." << endl;
				my_arg_reader_new.help();
				return -1;
			}

			string final_dir;
			final_dir = loc;

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

			cur_option = my_arg_reader.isActive("-dlen");
			if (cur_option != "")
			{
				// Get the option value.
				dlen = atoi(cur_option.c_str());
			}
			else
			{
				cout << "ERROR -- [Reading options] You must determine the length of the sequences to be created (option -dlen)." << endl;
				my_arg_reader_new.help();
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
				my_arg_reader_new.help();
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
				my_arg_reader_new.help();
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
		else if (mode == "imp")
		{
			string dataset_file;
			string my_options_imp[3];	   // The array containing the options.
			string my_descriptions_imp[3]; // The array containing the descriptions of the options.

			my_options_imp[0] = my_options[1];
			my_descriptions_imp[0] = my_descriptions[1];
			my_options_imp[1] = my_options[5];
			my_descriptions_imp[1] = my_descriptions[5];
			my_options_imp[2] = my_options[6];
			my_descriptions_imp[2] = my_descriptions[6];

			ArgReader my_arg_reader_imp(my_options_imp, my_descriptions_imp, 3);

			cur_option = my_arg_reader.isActive("-loc");
			if (cur_option != "")
			{
				// Get the option value.
				loc = cur_option;
			}
			else
			{
				cout << "ERROR -- [Reading options] You must determine the location where the data sequence files are going to be created (option -loc)." << endl;
				my_arg_reader_imp.help();
				return -1;
			}

			string final_dir;
			final_dir = loc;

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

			ifstream fin;

			// Get the options determined in the command line by the user.
			string cur_option;
			my_arg_reader.read(argc, argv); // Read the user options.
			cur_option = my_arg_reader.isActive("-df");
			if (cur_option != "")
			{
				// Get the option value.
				dataset_file = cur_option;

				// Load config file.
				// getting the chromosome
				cout << "=>Trying to locate and open the dataset file." << endl;
				fin.open(dataset_file.c_str());
				if (fin.fail())
				{
					cout << "=> ERROR: Cannot open '" << dataset_file << "' (dataset file). No such file." << endl
						 << endl;
					return -1;
				}
				else
				{
					cout << "\t"
						 << "-Found file." << endl;
				}
			}
			else
			{
				cout << "ERROR -- [Reading options] You must determine the location of the genome file (-df option)." << endl;
				my_arg_reader_imp.help();
				return -1;
			}

			// find dataset file name
			// extract GRC code from it
			size_t found;
			found = dataset_file.find_last_of("/\\");
			string dataset_file_name = dataset_file.substr(found + 1);
			char delimiter = '_';
			stringstream ss(dataset_file_name);
			string temp;
			int i = 0;
			vector<string> tokens;
			while (getline(ss, temp, delimiter))
				tokens.push_back(temp);

			string gencode = tokens.at(tokens.size() - 2);
			string desired_chr = "chromosome";
			string desired_chr_tmp = gencode + " Primary Assembly";
			string space = " ";
			string comma = ",";

			cur_option = my_arg_reader.isActive("-chr");
			if (cur_option != "")
			{
				// All primary reference chromosomes are seperated by this line: [...] chromosome x, GRCm39 reference primary [...]
				desired_chr = desired_chr + space + cur_option.c_str() + comma + space + desired_chr_tmp;
				cout << desired_chr << endl;
			}
			else
			{
				cout << "ERROR -- [Reading options] You must determine the chromosome to become the dataset(-chr option)." << endl;
				my_arg_reader_imp.help();
				return -1;
			}

			ifstream infile(dataset_file);
			if (infile.is_open())
			{
				int eof = 0;
				cout << "\t-Opened the file" << endl;
				string line;
				getline(infile, line);
				cout << "\t"
						"-Searching for chromosome"
						" in the file..."
					 << endl;
				getline(infile, line);
				eof = 1;
				while (!infile.eof())
				{
					if (line[0] != '>')
					{
						getline(infile, line);
						continue;
					}
					if (line.find(desired_chr) != std::string::npos)
					{
						cout << line << endl;
						eof = 0;
						break;
					}
					getline(infile, line);
				}

				if (eof)
				{
					cout << "Couldn't find the desired chromosome" << endl;
					return -1;
				}

				cout << "\t-Found Chromosome" << endl;

				string output_file_name = loc + "/" + "chromosome.txt";
				ofstream output(output_file_name);
				if (output.fail())
				{
					cout << "ERROR -- [Opening file] Cannot open '" << output_file_name << "' for writing..." << endl;
					return -1;
				}
				if (output.is_open())
				{
					string valid_lines;
					getline(infile, valid_lines);
					while (valid_lines[0] != '>')
					{
						output << valid_lines;
						getline(infile, valid_lines);
					}
				}
				else
				{
					cout << "ERROR -- [Opening file] Cannot open '" << output_file_name << "' for writing..." << endl;
					return -1;
				}

				cout << "Ok!" << endl;
				infile.close();
				output.close();
			}
			else
			{
				cout << "ERROR -- [Opening file] Cannot open '" << dataset_file << "' for writing..." << endl;
				return -1;
				;
			}
		}
		else
		{
			cout << "ERROR -- [Reading options] Invalid mode ('imp' or 'new')" << endl;
			my_arg_reader.help();
			return -1;
		}
	}
}
