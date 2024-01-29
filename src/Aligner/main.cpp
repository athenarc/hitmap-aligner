/**************************************************************************************/ /*
																						  * @file main.cpp
																						  *
																						  * @author Thanasis Vergoulis
																						  ****************************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib> //to use atoi etc.
#include <cmath>   //to use floor etc.
#include <vector>
#include "../Libraries/Misc/ArgReader.h"
#include "HitmapAligner.h"
#include "Validator.h"
#include "../Libraries/Sequences/SHash/HitmapHash.h"
#include "../Libraries/Misc/MemObserver.h"
#include "../Libraries/Misc/Utilities.h" //to use itoa
#include "../Libraries/Files/Files.h"
using namespace std;

/**
 * It represents a run of an alignment method on the given data.
 *
 * @var _method_name The name of the alignment method.
 * @var _method_parameters A string containing the values of the parameters of the method separated by spaces.
 * @var _avg_time_elapsed The average time elapsed for all iterations of the run.
 * @var _total_stats The verification statistics.
 *
 * @author Thanasis Vergoulis
 */
struct MethodRun
{
	string _method_name;
	string _method_parameters;
	double _avg_time_elapsed;
	VerificationStatistics _total_stats;
};

int main(int argc, char *argv[])
{
	// Experiment parameters.
	string dseq;  // The data sequence.
	string qseq;  // The query sequence.
	string qmode; //'one2one'=one query for each data, 'many2one'=all queries for one data

	// Files & filepaths.
	string exp_conf_file; // The location of the configuration file of the experiment.
	string query_file;	  // File containing the query sequences.
	string data_file;	  // File containing the data sequence
	string data_folder;	  // Folder containing the data file.
	string idx_file;
	idx_file = "";

	// Auxiliary variables
	string cur_token; // A buffer used to store the current token
	//	vector<MethodRun>::iterator cur_run;

	ifstream fin, fin2;
	ofstream fout;
	clock_t clock_start;
	clock_t clock_stop;
	string mode; // the mode of the runs

	// The parameters of the experiment
	int qnum;	 // the number of query sequences
	int qlen;	 // the length of each of the query sequences
	string qdis; // the distance function used
	int qthr;	 // the query threshold
	qthr = -1;
	int inum; // the number of iterations
	MethodRun method_run;
	bool all_dfiles; // If true, all the files in the folder are considered.
	bool qrand;		 // If true, the queries are taken from random locations of the data sequences.
	string tmp_buf;	 // buffer

	qrand = true;
	all_dfiles = true;

	string my_options[4];	   // The array containing the options.
	string my_descriptions[4]; // The array containing the descriptions of the options.
	my_options[0] = "-exp";
	my_descriptions[0] = "Determines the location of the configuration file that contains the details of the experiment (required).";
	my_options[1] = "-mode";
	// my_descriptions[1] = "The mode of the runs: time_measurement, filtering_time, time_measurement_verbose, debug (required).";
	my_descriptions[1] = "The mode of the runs: time_measurement, validation, time_measurement_verbose (required).";
	my_options[2] = "-qthr";
	my_descriptions[2] = "The query threshold (optional).";
	my_options[3] = "-idx";
	my_descriptions[3] = "The index file (optional).";
	ArgReader my_arg_reader(my_options, my_descriptions, 4);

	if (argc == 1) // Check if no arguments are given
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

		// Get the location of the configuration file of the experiment.
		cur_option = my_arg_reader.isActive("-exp");
		if (cur_option != "")
		{
			// Get the option value.
			exp_conf_file = cur_option;

			// Load config file.
			cout << "\t"
				 << "- Loading configuration file..." << endl;
			fin.open(exp_conf_file.c_str());
			if (fin.fail())
			{
				cout << "=> ERROR: Cannot open '" << exp_conf_file << "' (experiment conf. file). No such file." << endl
					 << endl;
				return -1;
			}

			// Read config file.
			cout << "\t"
				 << "- Parsing configuration file..." << endl;

			while (!fin.eof())
			{
				fin >> cur_token; // Get the next token.

				if (cur_token == "dfile")
				{
					fin >> cur_token;
					data_file = cur_token;
				}
				else if (cur_token == "qnum")
				{
					fin >> cur_token;
					qnum = atoi(cur_token.c_str());
				}
				else if (cur_token == "qlen")
				{
					fin >> cur_token;
					qlen = atoi(cur_token.c_str());
				}
				else if (cur_token == "meas")
				{
					fin >> cur_token;
					qdis = cur_token;
				}
				else if (cur_token == "inum")
				{
					fin >> cur_token;
					inum = atoi(cur_token.c_str());
				}
			}

			method_run._method_name = "N/A";
			method_run._method_parameters = "N/A";
			method_run._total_stats._ver_size = 0;
			method_run._total_stats._data_size = 0;
			method_run._total_stats._ver_areas_num = 0;
			method_run._total_stats._non_merged_areas = 0;
			method_run._total_stats._estimated_candidates = 0;

			fin.close();
		}
		else
		{
			cout << "=> ERROR: Determine the location of the configuration file of the experiment (option -exp)." << endl
				 << endl;
			my_arg_reader.help();
			return -1;
		}

		// Get the mode of execution.
		cur_option = my_arg_reader.isActive("-mode");
		if (cur_option != "")
		{
			string modes[4];
			modes[0] = "time_measurement";
			modes[1] = "filtering_time";
			modes[2] = "time_measurement_verbose";
			modes[3] = "debug";

			// Get the option value.
			mode = cur_option;

			if (mode != modes[0] && mode != modes[1] && mode != modes[2] && mode != modes[3])
			{
				cout << "=> ERROR: Specified mode '" << mode << "' does not exist." << endl
					 << endl;
				return -1;
			}

			cout << endl;
			cout << "\t"
				 << "- Exec. mode: '" << mode << "'" << endl;
		}
		else
		{
			cout << "=> ERROR: Determine the execution mode." << endl
				 << endl;
			my_arg_reader.help();
			return -1;
		}

		// Get the qthr (if given)
		cur_option = my_arg_reader.isActive("-qthr");
		if (cur_option != "")
		{
			qthr = atoi(cur_option.c_str());
		}

		// Get the idx file (if given)
		cur_option = my_arg_reader.isActive("-idx");
		if (cur_option != "")
		{
			idx_file = cur_option;
		}
	}
	cout << "\t"
		 << "- Finalising configurations..." << endl;

	// === Load the queries file
	query_file = exp_conf_file;

	vector<string> tokens;
	tokens = tokenize(query_file, ".conf");
	query_file = tokens[0] + ".q";

	cout << "\t"
		 << "- Query file opened..." << endl;

	fin2.open(query_file.c_str());
	if (fin2.fail())
	{
		cout << "=> ERROR: Cannot open '" << query_file << "' (queries file). No such file." << endl
			 << endl;
		return -1;
	}

	cout << "\t\t"
		 << "* qlen:\t\t" << qlen << "\t[from '" << query_file << "']" << endl;
	cout << "\t\t"
		 << "* qnum:\t\t" << qnum << endl;
	cout << "\t\t"
		 << "* iterations:\t" << inum << endl;

	// if (true)
	//{
	int dlen_clean; // length without special symbols (to be added below)
	int dlen_real;	// length with special symbols

	// === Get the data sequence.
	cout << "\t"
		 << "- Begin loading data sequence... [from '" << data_file << "']" << endl;
	fin.open(data_file.c_str());
	if (fin.fail())
	{
		cout << "=> ERROR: Cannot open '" << data_file << "' (data file). No such file." << endl
			 << endl;
		return -1;
	}
	fin >> dseq;
	fin.close();
	dlen_clean = dseq.length();

	// === Add special characters at the start of data (to avoid problems with alignments at the start)
	for (int j = 0; j <= qlen; j++)
	{
		dseq = "$" + dseq;
	}
	dlen_real = dseq.length();
	cout << "\t"
		 << "- Successful data loading [length: " << dlen_clean << " (clean), " << dlen_real << " (real)." << endl;

	// If qthr was not given in the command line, request it!
	if (qthr == -1)
	{
		cout << "\t"
			 << "- Give desired error threshold: ";
		cin >> qthr;
	}
	// If the idx_file was not given in the command line, request it!
	if (idx_file == "")
	{
		int id;
		int last_slash_pos;
		last_slash_pos = data_file.find_last_of("/\\");
		// data_folder = data_file.substr(0, last_slash_pos);
		data_folder = "../bin";
		// Check if there is an index file. If not, create an index (in memory).
		vector<string> filenames;
		vector<string> dirnames;
		cout << "Data Folder: " << data_folder << endl;

		int res = listDir(data_folder, filenames, dirnames);
		if (res)
		{
			cout << "\t"
				 << "- Reading data folder for existing idx files... " << endl;
			cout << "\t"
				 << "- Found " << filenames.size() << " files, " << dirnames.size() << " folders..." << endl;

			vector<string>::iterator cur_file;
			vector<string> idx_files;
			cur_file = filenames.begin();

			while (cur_file != filenames.end())
			{
				if /* (cur_file->find(data_file) != std::string::npos
					||*/
					((cur_file->find(".widx") != std::string::npos || cur_file->find(".w2idx") != std::string::npos || cur_file->find(".hidx") != std::string::npos || cur_file->find(".h2idx") != std::string::npos))
				{
					ifstream fin3;
					fin3.open(cur_file->c_str());
					string tok;
					bool proper_idx;
					proper_idx = true;

					while (!fin3.eof())
					{
						fin3 >> tok;
						if (tok == "@err_threshold:")
						{
							fin3 >> tok;
							if (atoi(tok.c_str()) != qthr)
							{
								proper_idx = false;
							}
						}
					}
					fin3.close();
					if (proper_idx)
						idx_files.push_back(*cur_file);
				}

				cur_file++;
			}

			if (idx_files.size() != 0)
			{
				cout << "\t"
					 << "- Found " << idx_files.size() << " possible idx files." << endl;
				cur_file = idx_files.begin();
				id = 0;
				while (cur_file != idx_files.end())
				{
					cout << "\t\t" << id << ": " << (*cur_file) << endl;
					id++;
					cur_file++;
				}
				cout << "\t"
					 << "- Use: ";
				cin >> id;
			}
			else
			{
				cout << "\t"
					 << "- No proper idx files found!" << endl;
				return -1;
				// MAYBE HERE PROVIDE THE OPPORTUNITY TO CREATE THE INDEX
			}
			idx_file = idx_files[id];
		}
		else
		{
			perror("List_Dir");
		}
	}

	if (idx_file.find(".hidx") != std::string::npos) // HITMAP index was selected
	{
		cout << "\t"
			 << "- HITMAP idx used." << endl;
		HitmapAligner hitmap_aligner;
		method_run._method_name = "hitmap";
		hitmap_aligner.loadIdx(idx_file, &dseq, qlen);

		// Align queries.
		for (int i = 1; i <= qnum; i++)
		{
			// Get the query sequence.
			fin2 >> tmp_buf;
			fin2 >> qseq;
			qlen = qseq.length();

			if (qdis == "edit")
			{
				VerificationStatistics tmp_stats;
				tmp_stats = hitmap_aligner.alignEditDistanceVerbose(qseq);
				method_run._total_stats._ver_size += tmp_stats._ver_size;
				method_run._total_stats._data_size += tmp_stats._data_size;
				method_run._total_stats._ver_areas_num += tmp_stats._ver_areas_num;
				method_run._total_stats._non_merged_areas += tmp_stats._non_merged_areas;
				method_run._total_stats._estimated_candidates += tmp_stats._estimated_candidates;

				vector<int> results;
				results = hitmap_aligner.getResults();
				Validator my_validator(&dseq, &qseq, qthr);
				if (!my_validator.validate(qdis, results))
				{
					vector<int> results;
					cout << "\t\t\t"
						 << "ERROR - There are some lost results." << endl;

					cout << endl
						 << "=== DEBUG ===" << endl;
					cout << "qseq: '" << qseq << "'" << endl;
					vector<int> ver_wins;
					ver_wins = hitmap_aligner.getWindows();
					cout << "\t\t\t"
						 << "Found " << ver_wins.size() << " windows." << endl;
					vector<int>::iterator cur_win;
					cur_win = ver_wins.begin();
					cout << "\t\t\t";
					while (cur_win != ver_wins.end())
					{
						cout << "[" << (*cur_win) << "-" << ((*cur_win) + qseq.length() + qthr) << " (real:" << ((*cur_win) - qlen - 1) << ")] ";
						// real position: if we do not consider the extra '$' symbols that we add at the begining of the data sequence
						cur_win++;
					}
					cout << endl;

					// print results found
					results = hitmap_aligner.getResults();
					cout << "\t\t\t"
						 << "Found " << results.size() << " results." << endl;

					// print results found by validator
					my_validator.printResults(qlen + 1, 3);
					return -1;
				}
				my_validator.printResults(qlen + 1, 3);
				// DEBUG
				/*vector<int> windows;
				vector<int>::iterator cur_win;
				windows = hitmap_aligner.getWindows();
				cout<<"Found "<<windows.size()<<" windows:"<<endl;
				cur_win = windows.begin();
				while( cur_win != windows.end() )
				{
					cout<<"\t ["<<(*cur_win)<<" ("<<((*cur_win)-qlen-1)<<")]"<<endl;
					cur_win++;
				}*/

				if (mode == "time_measurement_verbose")
				{
					vector<int>::iterator cur_res;
					cur_res = results.begin();
					cout << "\t\t\t";
					while (cur_res != results.end())
					{
						cout << "[" << (*cur_res) << " (real:" << ((*cur_res) - qlen - 1) << ")] ";
						// real position: if we do not consider the extra '$' symbols that we add at the begining of the data sequence
						cur_res++;
					}
					cout << endl;
					clock_start = clock();
					for (int f = 0; f < inum; f++)
					{
						hitmap_aligner.alignEditDistance(qseq);
					}
					clock_stop = clock();
					method_run._avg_time_elapsed += (((double)(clock_stop - clock_start) / (double)inum) / (double)CLOCKS_PER_SEC / (double)qnum);
				}
				else if (mode == "filtering_time")
				{
					clock_start = clock();
					for (int f = 0; f < inum; f++)
					{
						hitmap_aligner.filtering(qseq);
					}
					clock_stop = clock();
					method_run._avg_time_elapsed += (((double)(clock_stop - clock_start) / (double)inum) / (double)CLOCKS_PER_SEC / (double)qnum);
				}
			}
		}
	}
	else if (idx_file.find(".h2idx") != std::string::npos) // HITMAP light index was selected
	{
		cout << "\t"
			 << "- HITMAP Light idx used." << endl;
		HitmapLightAligner hitmap_aligner;
		method_run._method_name = "hitmap2";
		hitmap_aligner.loadIdx(idx_file, &dseq, qlen);

		// Align queries.
		for (int i = 1; i <= qnum; i++)
		{
			cout << "\t\t"
				 << "[QUERY -" << i << "-]" << endl;

			// Get the query sequence.
			fin2 >> tmp_buf;
			fin2 >> qseq;
			qlen = qseq.length();
			cout << "\t\t\t"
				 << "Successful query retrieval." << endl;

			cout << "\t\t\t"
				 << "Performing query alignment [simil='" << qdis << "',mode='" << mode << "']..." << endl;
			if (qdis == "edit")
			{
				if (mode == "time_measurement")
				{
					cout << "=> ERROR: This mode is not supported in this version." << endl;
					return -1;
				}
				else if (mode == "validation")
				{
					cout << "=> ERROR: This mode is not supported in this version." << endl;
					return -1;
				}
				else if (mode == "time_measurement_verbose")
				{
					VerificationStatistics tmp_stats;
					tmp_stats = hitmap_aligner.alignEditDistanceVerbose(qseq);
					method_run._total_stats._ver_size += tmp_stats._ver_size;
					method_run._total_stats._data_size += tmp_stats._data_size;
					method_run._total_stats._ver_areas_num += tmp_stats._ver_areas_num;
					method_run._total_stats._non_merged_areas += tmp_stats._non_merged_areas;
					method_run._total_stats._estimated_candidates += tmp_stats._estimated_candidates;

					cout << "\t\t\t"
						 << "Validate that the result set is correct." << endl;
					vector<int> results;
					results = hitmap_aligner.getResults();
					Validator my_validator(&dseq, &qseq, qthr);
					if (!my_validator.validate(qdis, results))
					{
						vector<int> results;
						cout << "\t\t\t"
							 << "ERROR - There are some lost results." << endl;

						cout << endl
							 << "=== DEBUG ===" << endl;
						cout << "qseq: '" << qseq << "'" << endl;
						vector<int> ver_wins;
						ver_wins = hitmap_aligner.getWindows();
						cout << "\t\t\t"
							 << "Found " << ver_wins.size() << " windows." << endl;
						vector<int>::iterator cur_win;
						cur_win = ver_wins.begin();
						cout << "\t\t\t";
						while (cur_win != ver_wins.end())
						{
							cout << "[" << (*cur_win) << "-" << ((*cur_win) + qseq.length() + qthr) << " (real:" << ((*cur_win) - qlen - 1) << ")] ";
							// real position: if we do not consider the extra '$' symbols that we add at the begining of the data sequence
							cur_win++;
						}
						cout << endl;

						// print results found
						results = hitmap_aligner.getResults();
						cout << "\t\t\t"
							 << "Found " << results.size() << " results." << endl;
						vector<int>::iterator cur_res;
						cur_res = results.begin();
						cout << "\t\t\t";
						while (cur_res != results.end())
						{
							cout << "[" << (*cur_res) << " (real:" << ((*cur_res) - qlen - 1) << ")] ";
							// real position: if we do not consider the extra '$' symbols that we add at the begining of the data sequence
							cur_res++;
						}
						cout << endl;
						// print results found by validator
						my_validator.printResults(qlen + 1, 3);

						return -1;
					}
					else
					{
						cout << "\t\t\t"
							 << "Results are okay!" << endl;
					}

					cout << "\t\t\t"
						 << "Measure execution time." << endl;
					clock_start = clock();
					for (int f = 0; f < inum; f++)
					{
						hitmap_aligner.alignEditDistance(qseq);
					}
					clock_stop = clock();
					method_run._avg_time_elapsed += (((double)(clock_stop - clock_start) / (double)inum) / (double)CLOCKS_PER_SEC / (double)qnum);
					cout << "\t\t\t"
						 << "Measurements are done." << endl;
				}
				else
				{
					cout << "=> ERROR: Unknown mode '" << mode << "'." << endl;
					return -1;
				}
			}
			else if (qdis == "hamming")
			{
				cout << "THIS MODE IS OUT-DATED..." << endl;
				return -1;
			}
			cout << "\t\t"
				 << "Successful run!" << endl;
			cout << "\t\t\t"
				 << "+ Cleaning unit vector block pool." << endl;
			// SparseBitsetC::cleanBlockPool();
		}
	}
	else
	{
		cout << "WARNING -- [Running alignment methods] Uknown idx type '" << method_run._method_name << "'." << endl;
	}
	cout << "[Run: '" << method_run._method_name << "' with params: {" << method_run._method_parameters << "}]" << endl;
	cout << "\t"
		 << "Avg. time elapsed: " << method_run._avg_time_elapsed << endl;
	if (mode == "time_measurement_verbose" || mode == "filtering_time")
	{
		method_run._total_stats._ver_size_percentage = (double)method_run._total_stats._ver_size / (double)method_run._total_stats._data_size;
		method_run._total_stats.print(1);
	}

#ifndef COLUMN_STORED_BUCKETS
	hmp::HitmapBucket::freeZeros();
#endif
	return 0;
}

/**
 * @brief A fragment of the data sequence.
 *
 * @var _begin The beginning position in the sequence.
 * @var _end The ending position in the sequence.
 * @var _type The type of fragment.
 *
 * @author Thanasis Vergoulis
 */
struct Fragment
{
	unsigned long _begin;
	unsigned long _end;
	unsigned long _type;
};

/**
 * @brief Struct containing pointers to all the index structures used by mDFilter method.
 *
 * @details Here the index structure selected is hash table.
 *
 * @var _idxs Array containing pointers to the hash tables.
 * @var _idxs_num The number of indexes.
 * @var _q_len The query length for which the indexes where built.
 * @var _q_thr The query threshold for which the indexes where built.
 *
 * @author Thanasis Vergoulis
 */
/*struct mDFHash
{
	SHash** _idxs;
	unsigned long _idxs_num;
	unsigned long _q_len;
	unsigned long _q_thr;
};*/

/**
 * @brief Naive Hamming distance calculator.
 *
 * @param s1 The first sequence (reference).
 * @param s2 The second sequence (reference).
 *
 * @author Thanasis Vergoulis
 */
int nHamming(string &s1, string &s2)
{
	unsigned long s_len;
	s_len = s1.length();
	unsigned long ham_dist;
	ham_dist = 0;

	for (unsigned long i = 0; i < s_len; i++)
	{
		if (s1[i] != s2[i])
		{
			ham_dist++;
		}
	}

	return ham_dist;
}

int nSearchHamming(string &d_seq, string &q_seq, unsigned long q_thr)
{
	unsigned long d_len;
	d_len = d_seq.length();
	unsigned long q_len;
	q_len = q_seq.length();

	for (unsigned long i = 0; i < (d_len - q_len + 1); i++)
	{
		string tmp_str;
		tmp_str = d_seq.substr(i, q_len);
		if (nHamming(tmp_str, q_seq) <= q_thr)
		{
			cout << "[nSearchHamming:] Found [" << i << "," << (i + q_len - 1) << "]" << endl;
		}
	}

	return 0;
}