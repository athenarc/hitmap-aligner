#include "Fredr.h"

FredrAligner::FredrAligner(string *alphabet, int gram_len)
{
	this->_alphabet = alphabet;
	this->_alphabet_size = this->_alphabet->length();
	this->_gram_len = gram_len;
	this->_dict = new tr1::unordered_map<long long, int>();
}

void FredrAligner::align(string *d_seq, string *q_seqs, int q_seqs_num, int *q_thrs)
{
	// Store the d_seq and the query paramets.
	this->_d_seq = d_seq;
	this->_q_seqs = q_seqs;
	this->_q_seqs_num = q_seqs_num;
	this->_q_thrs = q_thrs;
	this->queryPreparation();

	// Align based on the stored parameters.
	this->align();
}

void FredrAligner::align(string *d_seq)
{
	// Store the d_seq.
	this->_d_seq = d_seq;

	// Align based on the stored parameters.
	this->align();
}

void FredrAligner::align(string *q_seqs, int q_seqs_num, int *q_thrs)
{
	// Store the query parameters.
	this->_q_seqs = q_seqs;
	this->_q_seqs_num = q_seqs_num;
	this->_q_thrs = q_thrs;
	this->queryPreparation();

	// Align based on the stored parameters.
	this->align();
}

void FredrAligner::queryPreparation(void)
{
	//==============================================================================================
	// Declarations.
	//==============================================================================================
	int i, j; // Auxiliary variables used as counters in loops.

	//==============================================================================================
	// Initializations.
	//==============================================================================================
	this->_dict_size = (int)pow((double)this->_alphabet_size, (double)this->_gram_len); // Compute the size of the D table.
	// Initialize min_q_len.
	this->_min_q_len = this->_q_seqs[0].length();
	for (i = 1; i < this->_q_seqs_num; i++)
	{
		if (this->_q_seqs[i].length() < this->_min_q_len)
		{
			this->_min_q_len = this->_q_seqs[i].length();
		}
	}
	this->_min_q_len = this->_min_q_len - 1; // The one character in the string is the ' ' character.
	// Initialize max_q_thr
	this->_max_q_thr = max(this->_q_thrs, this->_q_seqs_num);

	// Take all the possible l-grams. For each l-gram initialize the corresponding value of the D table.
	char cur_l_gram[8] = {'$', '$', '$', '$', '$', '$', '$', '$'};
	char *cur_l_gram_full; // The current l-gram with a space in front.
	cur_l_gram_full = new char[this->_gram_len + 2];
	cur_l_gram_full[0] = ' ';
	cur_l_gram_full[this->_gram_len + 1] = '\0';
	cur_l_gram[this->_gram_len] = '\0';

	//==============================================================================================
	// Construct the gram dictionary.
	//==============================================================================================
	for (i = 0; i < this->_dict_size; i++) // One iteration per l-gram.
	{
		for (j = 0; j < this->_gram_len; j++) // One iteration per position in l-gram.
		{
			int testing_modulo; // This is a value that is used to test if the corresponding character must been changed.
			testing_modulo = (int)pow((double)this->_alphabet_size, (double)j);

			if (i % testing_modulo == 0) // Check if we must change the character that corresponds to position l-j-1.
			{
				cur_l_gram[this->_gram_len - j - 1] = (*this->_alphabet)[(i / testing_modulo) % this->_alphabet_size]; // Change the character.
				cur_l_gram_full[this->_gram_len - j] = cur_l_gram[this->_gram_len - j - 1];
			}
		}
		// Now compute the D table value for this l-gram.
		int cur_d_value;
		cur_d_value = this->_gram_len;

		for (j = 0; j < this->_q_seqs_num; j++)
		{
			cur_d_value = getMinDifferencesNaiveAlt(this->_q_seqs[j], cur_l_gram_full, cur_d_value);
		}
		// Now we have the current l-gram stored in "cur_l_gram"!!!
		this->_dict->insert(pair<long long, int>(*((long long *)cur_l_gram), cur_d_value)); // Initially the values are set to l (the maximum).
	}
}

void FredrAligner::align(void)
{
	//==============================================================================================
	// Declarations.
	//==============================================================================================
	list<int> all_matches; // A list containing all the matches.
	long data_seq_end;	   // The end of the data seq (precompatation for efficiency reasons)
	long win_end_rel_pos;  // precomputation for efficiency reasons
	long ver_area_start;
	long ver_area_end;
	long window_start; // The start position of the current window.
	long window_end;   // The end position of the current window.

	//==============================================================================================
	// Initializations.
	//==============================================================================================
	data_seq_end = this->_d_seq->length() - 1;				   // I will use it a lot, so it is efficient to pre-compute it.
	win_end_rel_pos = this->_min_q_len - this->_max_q_thr - 1; // = window_length - 1 (precomputed also for efficiency reasons)
	ver_area_start = 1;
	ver_area_end = -1;
	// Initialize the window.
	window_start = 1;
	window_end = window_start + win_end_rel_pos;

	char cur_l_gram[8] = {'$', '$', '$', '$', '$', '$', '$', '$'};
	cur_l_gram[this->_gram_len] = '\0';

	// For each window position read successive l-grams backwards.
	while (window_end <= data_seq_end)
	{
		long gram_start; // The start position of the current gram.
		long gram_end;	 // The end position of the current gram.
		int m_acc = 0;	 // The accumulator M.

		//		cout<<" * Examine window ["<<window_start<<","<<window_end<<"] = "<<this->_d_seq->substr(window_start,window_end-window_start+1)<<endl; //DEBUG

		// Initialize the l-gram.
		gram_start = window_end - this->_gram_len + 1;
		gram_end = gram_start + this->_gram_len - 1;

		bool skip_flag = false; // The value of this flag becomes true if the current window is skipped.

		// Start checking the l-grams of the current window.
		while (gram_start >= window_start)
		{
			this->_d_seq->copy(cur_l_gram, this->_gram_len, gram_start); // ME AYTON TON TROPO KATAFERA NA PESEI TO KATHE ACCESS SE IDIO PERIPOY ME ACCESS DP!
			int cur_m;
			m_acc = m_acc + this->_dict->find(*((long long *)cur_l_gram))->second; // Update the value of the accumulator.

			if (m_acc > this->_max_q_thr)
			{
				// Shift properly the window.
				window_start = gram_start + 1;
				window_end = window_start + win_end_rel_pos;

				// If the end of the new window will go outside
				// of the data sequence, then this is the end.
				// You must verify the current verification area
				// and the fragment of the data sequence that
				// corresponds to the new window.
				if (window_end > data_seq_end)
				{
					// If the current window that I must
					// check begins outside the previous
					// verification area run ASM in the
					// previous verification area and then
					// create a new one.
					if (window_start > ver_area_end + 1)
					{
						// Verify the current verification area.

						// Verification and push the new matches at the end of the list.
						this->verify(ver_area_start, ver_area_end);

						// Create a verification area for the rest of the data sequence.
						ver_area_start = window_start;
						ver_area_end = window_end + 2 * this->_max_q_thr;
						if (ver_area_end > data_seq_end)
						{
							ver_area_end = data_seq_end;
						}

						// Verification and push the new matches at the end of the list.
						this->verify(ver_area_start, ver_area_end);
					}
					else
					{
						// Just update the verification area and run ASM.
						ver_area_end = data_seq_end;

						// Verification and push the new matches at the end of the list.
						this->verify(ver_area_start, ver_area_end);
					}
				}

				skip_flag = true; // Since the window has been skipped, raise the flag!
				break;
			}

			// Take the next successive l-gram.
			gram_end = gram_start - 1;
			gram_start = gram_end - this->_gram_len + 1;
		}

		if (skip_flag == false)
		{
			// If the start of the area that must be tested by DP is
			// inside the previous area, then extend the previous
			// area.

			// If the current window that I must check begins outside
			// the previous verification area run ASM in the previous
			// verification area and then create a new one.
			if (window_start > ver_area_end + 1) // Attention! If window_start == ver_area_end+1, then I must extend the current ver. area.
			{
				// Verify the previous verification area.
				if (ver_area_end > 0)
				{
					// Verification and push the new matches at the end of the list.
					this->verify(ver_area_start, ver_area_end);
				}

				ver_area_start = window_start;
				ver_area_end = window_end + 2 * this->_max_q_thr;
				if (ver_area_end > data_seq_end)
				{
					ver_area_end = data_seq_end;
				}

				// If the current window was the last possible,
				// then you must also verify the fragment of the
				// data that corresponds to this window. There
				// will be no additional iteration, so you must
				// do the verification now.
				if (window_end == data_seq_end)
				{
					// Verification and push the new matches at the end of the list.
					this->verify(ver_area_start, ver_area_end);
				}
			}
			// If the window arrived to the end of the data sequence,
			// then there is no way for the verification area to get
			// increased. You must do the corresponding verification.
			else if (window_end == data_seq_end)
			{
				ver_area_end = data_seq_end;

				// Verification and push the new matches at the end of the list.
				this->verify(ver_area_start, ver_area_end);
			}
			// If the start of the current window is inside the
			// previous verification area, then just extend the
			// verification area. You will do the verification later.
			else
			{
				ver_area_end = window_end + 2 * this->_max_q_thr;
				if (ver_area_end > data_seq_end)
				{
					ver_area_end = data_seq_end;
				}
			}

			// Then shift properly the window.
			window_start = window_start + 1;
			window_end = window_start + win_end_rel_pos;
		}
	}
}

void FredrAligner::verify(long ver_start, long ver_end)
{
	int i;
	list<int> all_area_matches; // All the matches of the current verification area.

	cout << "To verify:" << ver_start << "," << ver_end << " " << this->_d_seq->substr(ver_start, ver_end - ver_start + 1) << endl;

	// For each query sequence run the corresponding ASM query.
	for (i = 0; i < this->_q_seqs_num; i++)
	{
		list<int> matches;
		//		matches = ASMdpUkk( &data_seq, ver_start, ver_end, &query_seqs[i], k_values[i], comp);
		//		dynamicProgramming(	&data_seq,ver_start,ver_end,&query_seqs[i],k_values[i],matches); //this is to return ALL matches (even if the have the same start-end
		all_area_matches.merge(matches);
		all_area_matches.unique(); // Erase duplicates.
	}
	//	all_matches.splice(all_matches.end(), all_area_matches); //Push the new matches at the end of the list of all the matches.
}
