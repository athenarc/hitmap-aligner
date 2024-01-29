#include "Validator.h"

Validator::Validator(string *dseq, string *qseq, unsigned int qthr)
{
	this->_dseq = dseq;
	this->_qseq = qseq;
	this->_dlen = this->_dseq->length();
	this->_qlen = this->_qseq->length();
	this->_qthr = qthr;
}

Validator::~Validator(void)
{
}

bool Validator::validate(string distance, vector<int> &test_results)
{
	if (distance == "edit")
	{
		// find the results based on dynamic programming
		this->_results.empty();
		dynamicProgramming(this->_dseq, 0, this->_dlen - 1, this->_qseq, this->_qthr, &this->_results);

		vector<int>::iterator tmp_it, tmp_it2;

		// ensure that there are no duplicates and that the results are sorted
		sort(this->_results.begin(), this->_results.end());
		tmp_it = unique(this->_results.begin(), this->_results.end());
		this->_results.resize(std::distance(this->_results.begin(), tmp_it));
		sort(test_results.begin(), test_results.end());
		tmp_it = unique(test_results.begin(), test_results.end());
		test_results.resize(std::distance(test_results.begin(), tmp_it));

		cout<<"\t"<<"- Validator: DP results: "<<this->_results.size()<<endl;
		cout<<"\t"<<"- Validator: method results: "<<test_results.size()<<endl;

		if (test_results.size() == 0) // method no results
		{
			tmp_it = this->_results.begin();
			while (tmp_it != this->_results.end())
			{
				cout << "PROBLEM! Tested aligner returns no results!" << endl;
				cout << "\t - Validator result =" << (*tmp_it) << endl;
				cout << this->_dseq->substr((*tmp_it) - this->_qlen - 2 * this->_qthr + 1, this->_qlen + 2 * this->_qthr) << endl;
				tmp_it++;
			}
			return false;
		}
		else
		{
			tmp_it = this->_results.begin();
			tmp_it2 = test_results.begin();
			unsigned int idx = 0;
			while (tmp_it != this->_results.end())
			{
				if ((*tmp_it) != (*tmp_it2))
				{
					cout << "PROBLEM! Tested aligner's results do not much with validator's! (V:" << this->_results.size() << ", A:" << test_results.size() << ")" << endl;
					cout << "\t - [" << idx << "] Validator result =" << (*tmp_it) << ", Aligner result =" << (*tmp_it2) << endl;
					cout << this->_dseq->substr((*tmp_it) - this->_qlen - 2 * this->_qthr + 1, this->_qlen + 2 * this->_qthr) << endl;
					return false;
				}
				tmp_it++;
				tmp_it2++;
				idx++;
			}
		}

		// check if the results are okay

		return true;
	}
	else if (distance == "hamming")
	{
		cout << "ERROR -- [Validating output] Validator for hamming distance is not ready yet." << endl;
		return false;
	}
	else
	{
		cout << "ERROR -- [Validating output] Unknown distance function given to the validator." << endl;
		return false;
	}
}

void Validator::printResults(int offset, int tabs)
{
	vector<int>::iterator cur_result;
	for (int i = 0; i < tabs; i++)
	{
		cout << "\t";
	}
	cout << "[Validator:] Results found (" << this->_results.size() << "): " << endl;
	//	cout<<"offset="<<offset<<endl;
	cur_result = this->_results.begin();
	for (int i = 0; i < tabs; i++)
	{
		cout << "\t";
	}
	while (cur_result != this->_results.end())
	{
		cout << "[" << (*cur_result) << " (real:" << ((*cur_result) - offset) << ")] ";
		cur_result++;
	}
	cout << endl;
}