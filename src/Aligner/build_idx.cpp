#include <iostream>
#include <fstream>
#include <sys/sysinfo.h>
#include <errno.h>
#include "HitmapAligner.h"
#include "../Libraries/Misc/ArgReader.h"
#include "../Libraries/Misc/Utilities.h" //to use itoa
using namespace std;

int main(int argc, char *argv[])
{
	string dseq_file;		 // the data sequence file
	string conf_file;		 // the configuration file
	string dseq;			 // the data sequence
	int dlen;				 // the length of data sequence
	clock_t clock_start;	 // the timer start
	clock_t clock_stop;		 // the timer stop
	unsigned long total_ram; // the total RAM memory of the system
	ifstream fin;

	//====================================================================================
	// Create an argument reader - get the argunent values and check that they are all ok.
	//

	string my_options[2];	   // The array containing the options.
	string my_descriptions[2]; // The array containing the descriptions of the options.
	my_options[0] = "-d";
	my_descriptions[0] = "The location of data seq file (required).";
	my_options[1] = "-c";
	my_descriptions[1] = "The location of the index config file (required).";

	// Get the system information...
	struct sysinfo info;
	if (sysinfo(&info) != 0)
	{
		cout << "=> ERROR: Something went wrong during measuring system RAM [more info:'" << strerror(errno) << "']..." << endl;
		return -1;
	}
	total_ram = info.totalram;

	ArgReader my_arg_reader(my_options, my_descriptions, 2);

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

		// Get the location of the data seq file.
		cur_option = my_arg_reader.isActive("-d");
		if (cur_option != "")
		{
			// Get the option value.
			dseq_file = cur_option;

			// === Get the data sequence.
			cout << "=> Loading data seq from '" << dseq_file << "'..." << endl;
			fin.open(dseq_file.c_str());
			if (fin.fail())
			{
				cout << "=> ERROR: Cannot open '" << dseq_file << "' (data file). No such file." << endl
					 << endl;
				return -1;
			}
			fin >> dseq;
			fin.close();
			dlen = dseq.length();

			cout << "=> Done! [d=" << dlen << " (clean)]" << endl;
		}
		else
		{
			cout << "=> ERROR: Determine the location of the data sequence file (option -d)." << endl
				 << endl;
			my_arg_reader.help();
			return -1;
		}

		// Get the location of the configuration file of the experiment.
		cur_option = my_arg_reader.isActive("-c");
		if (cur_option != "")
		{
			// Get the option value.
			conf_file = cur_option;

			fin.open(conf_file.c_str());
			if (fin.fail())
			{
				cout << "=> ERROR: Problem loading configuration file '" << conf_file << "'..." << endl;
				return -1;
			}

			string cur_token;
			fin >> cur_token;
			if (cur_token != "i")
			{
				cout << "=> ERROR: Bad configuration file format. No index type is determined in the first line (parameter 'i')." << endl;
				return -1;
			}
			fin >> cur_token;

			if (cur_token == "hitmap")
			{
				int qlen;		 // The length of the supported queries.
				float err_ratio; // The error ratio of the supported queries.
				int err_thr;
				int fnum;				// The number of fragments.
				unsigned long buck_num; // The number of buckets.
				string ab;				// The alphabet.
				unsigned long block_size;

				err_thr = -1;
				err_ratio = -1;

				cout << "=> Reading configuration file..." << endl;

				// Get qlen.
				fin >> cur_token;
				if (cur_token != "q")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'q' (query length) must be the first idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				qlen = atoi(cur_token.c_str());

				// Get err_ratio.
				fin >> cur_token;
				if (cur_token != "r" && cur_token != "e")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'r' (error ratio) or 'e' (error threshold) must be the second idx parameter." << endl;
					return -1;
				}
				if (cur_token == "r")
				{
					fin >> cur_token;
					err_ratio = atof(cur_token.c_str());
				}
				else if (cur_token == "e")
				{
					fin >> cur_token;
					err_thr = atoi(cur_token.c_str());
				}

				// Ger fnum.
				fin >> cur_token;
				if (cur_token != "f")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'f' (fragments number) must be the third idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				fnum = atoi(cur_token.c_str());

				// Get buck_num.
				fin >> cur_token;
				if (cur_token != "b")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'b' (hash table blocks number) must be the fourth idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				buck_num = strtol(cur_token.c_str(), (char **)NULL, 10);
				if (buck_num == LONG_MAX || buck_num == LONG_MIN)
				{
					cout << "=> ERROR: The number of buckets is way too big (largest number: " << LONG_MAX << ")." << endl;
					return -1;
				}

				// Get block_size
				fin >> cur_token;
				if (cur_token != "s")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 's' (bitset block size) must be the fifth idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				block_size = atoi(cur_token.c_str());

				// Get qlen.
				fin >> cur_token;
				if (cur_token != "a")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'a' (alphabet) must be the sixth idx parameter." << endl;
					return -1;
				}
				fin >> ab;

				// Examine the proper end of file.
				while (!fin.eof())
				{
					fin >> cur_token;
					if (cur_token != "")
					{
						cout << "=> ERROR: Bad end of configuration file." << endl;
						return -1;
					}
				}

				if (err_ratio != -1)
					cout << "=> Done! [q=" << qlen << ", r=" << (double)err_ratio << ", f=" << fnum << ", b=" << buck_num << ", a='" << ab << "']" << endl;
				else
					cout << "=> Done! [q=" << qlen << ", e=" << (double)err_thr << ", f=" << fnum << ", b=" << buck_num << ", a='" << ab << "']" << endl;

				// === Add special characters at the start of data (to avoid problems with alignments at the start)
				for (int j = 0; j <= qlen; j++)
				{
					dseq = "$" + dseq;
				}
				dlen = dseq.length();
				cout << "=> Done! [d=" << dlen << " (real)]" << endl;

				cout << "=> Building HITMAP index..." << endl;

				int qthr;
				if (err_ratio != -1)
					qthr = round(qlen * err_ratio);
				else
					qthr = err_thr;

				HitmapAligner hitmap_aligner(qlen, qthr, fnum, block_size, buck_num, ab);
				clock_start = clock();
				hitmap_aligner.createIdx(dseq, "verbose");
				clock_stop = clock();

				if (err_ratio != -1)
					hitmap_aligner.storeIdx(dseq_file + "_q" + itoa(qlen) + "_r" + dtoa(err_ratio) + "_f" + itoa(fnum) + "_b" + itoa(buck_num) + "_s" + itoa(block_size) + ".hidx", ab, ((double)(clock_stop - clock_start) / (double)CLOCKS_PER_SEC));
				else
					hitmap_aligner.storeIdx(dseq_file + "_q" + itoa(qlen) + "_e" + itoa(err_thr) + "_f" + itoa(fnum) + "_b" + itoa(buck_num) + "_s" + itoa(block_size) + ".hidx", ab, ((double)(clock_stop - clock_start) / (double)CLOCKS_PER_SEC));

				cout << "=> Done! [constr. time: " << ((double)(clock_stop - clock_start) / (double)CLOCKS_PER_SEC) << " secs]" << endl;
			}
			else if (cur_token == "hitmap2")
			{
				int qlen;		 // The length of the supported queries.
				float err_ratio; // The error ratio of the supported queries.
				int err_thr;
				int fnum;  // The number of fragments.
				string ab; // The alphabet.
				unsigned long block_size;
				unsigned long buck_num; // the number of buckets

				err_thr = -1;
				err_ratio = -1;

				cout << "=> Reading configuration file..." << endl;

				// Get qlen.
				fin >> cur_token;
				if (cur_token != "q")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'q' (query length) must be the first idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				qlen = atoi(cur_token.c_str());

				// Get err_ratio.
				fin >> cur_token;
				if (cur_token != "r" && cur_token != "e")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'r' (error ratio) or 'e' (error threshold) must be the second idx parameter." << endl;
					return -1;
				}
				if (cur_token == "r")
				{
					fin >> cur_token;
					err_ratio = atof(cur_token.c_str());
				}
				else if (cur_token == "e")
				{
					fin >> cur_token;
					err_thr = atoi(cur_token.c_str());
				}

				// Ger fnum.
				fin >> cur_token;
				if (cur_token != "f")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'f' (fragments number) must be the third idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				fnum = atoi(cur_token.c_str());

				// Get buck_num.
				fin >> cur_token;
				if (cur_token != "b")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'b' (hash table blocks number) must be the fourth idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				buck_num = atoi(cur_token.c_str());

				// Get block_size
				fin >> cur_token;
				if (cur_token != "s")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 's' (bitset block size) must be the fifth idx parameter." << endl;
					return -1;
				}
				fin >> cur_token;
				block_size = atoi(cur_token.c_str());

				// Get qlen.
				fin >> cur_token;
				if (cur_token != "a")
				{
					cout << "=> ERROR: Bad configuration file format. Parameter 'a' (alphabet) must be the sixth idx parameter." << endl;
					return -1;
				}
				fin >> ab;

				// Examine the proper end of file.
				while (!fin.eof())
				{
					fin >> cur_token;
					if (cur_token != "")
					{
						cout << "=> ERROR: Bad end of configuration file." << endl;
						return -1;
					}
				}

				if (err_ratio != -1)
					cout << "=> Done! [q=" << qlen << ", r=" << (double)err_ratio << ", f=" << fnum << ", s=" << block_size << ", b=" << buck_num << ", a='" << ab << "']" << endl;
				else
					cout << "=> Done! [q=" << qlen << ", e=" << (double)err_thr << ", f=" << fnum << ", s=" << block_size << ", b=" << buck_num << ", a='" << ab << "']" << endl;

				// === Add special characters at the start of data (to avoid problems with alignments at the start)
				for (int j = 0; j <= qlen; j++)
				{
					dseq = "$" + dseq;
				}
				dlen = dseq.length();
				cout << "=> Done! [d=" << dlen << " (real)]" << endl;

				cout << "=> Building HITMAP-LIGHT index..." << endl;

				int qthr;
				if (err_ratio != -1)
					qthr = round(qlen * err_ratio);
				else
					qthr = err_thr;

				HitmapLightAligner hitmap_aligner(qlen, qthr, fnum, block_size, buck_num, ab);
				clock_start = clock();
				hitmap_aligner.createIdx(dseq, "silent");
				clock_stop = clock();

				if (err_ratio != -1)
					hitmap_aligner.storeIdx(dseq_file + "_q" + itoa(qlen) + "_r" + dtoa(err_ratio) + "_f" + itoa(fnum) + "_s" + itoa(block_size) + ".h2idx", ab, ((double)(clock_stop - clock_start) / (double)CLOCKS_PER_SEC));
				else
					hitmap_aligner.storeIdx(dseq_file + "_q" + itoa(qlen) + "_e" + itoa(err_thr) + "_f" + itoa(fnum) + "_s" + itoa(block_size) + ".h2idx", ab, ((double)(clock_stop - clock_start) / (double)CLOCKS_PER_SEC));

				cout << "=> Done! [constr. time: " << ((double)(clock_stop - clock_start) / (double)CLOCKS_PER_SEC) << " secs]" << endl;
			}
			else
			{
				cout << "=> ERROR: '" << cur_token << "' is not a valid index method." << endl;
				return -1;
			}

			fin.close();
		}
		else
		{
			cout << "=> ERROR: Determine the location of the configuration file (option -c)." << endl
				 << endl;
			my_arg_reader.help();
			return -1;
		}
	}

	return 0;
}
