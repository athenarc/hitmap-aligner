#include "HitmapAligner.h"

HitmapAligner::HitmapAligner(unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string ab)
{
	this->_qlen = qlen;
	this->_qthr = qthr;
	this->_fnum = fnum;
	//	this->_part_len = part_len;
	//	this->_part_shift = part_shift;
	this->_flen = floor(this->_qlen / this->_fnum);
	this->_buckets_num = buckets_num;
	this->_errno = 0;
	this->_block_size = block_size;
	this->_ab = ab;
}

HitmapAligner::HitmapAligner(void)
{
}

HitmapAligner::~HitmapAligner()
{
	delete this->_hitmap;
}

void HitmapAligner::createIdx(string &dseq, string mode)
{
	this->_dseq = &dseq; // get a pointer to the data sequence
	if (mode == "verbose")
	{
		cout << endl;
		cout << "Building 'Hitmap index':" << endl;
		cout << "\t[begin]" << endl;
	}

	if (this->_errno != 0)
	{
		cout << "ERROR -- [Creating index] An error occurred, cannot build the index..." << endl;
		return;
	}

	//	this->_hitmap = new hmp::HitmapHash(&dseq, this->_part_len, this->_part_shift, this->_fnum, this->_flen, this->_buckets_num);
	this->_hitmap = new hmp::HitmapHash(&dseq, this->_qthr, this->_flen, this->_flen, this->_fnum, this->_block_size, this->_buckets_num, 1);

	this->_hitmap->initCounterBitsets(this->_fnum);

	//	this->_hitmap->print(); //DEBUG

	if (mode == "verbose")
	{
		cout << "\t[end]" << endl;
		cout << endl;
	}
}

void HitmapAligner::createIdx(string &dseq, unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string mode)
{
	this->_qlen = qlen;
	this->_qthr = qthr;
	this->_fnum = fnum;
	//	this->_part_len = part_len;
	//	this->_part_shift = part_shift;
	this->_flen = floor(this->_qlen / this->_fnum);
	this->_buckets_num = buckets_num;
	this->_errno = 0;
	this->_block_size = block_size;

	this->createIdx(dseq, mode);
}

void HitmapAligner::storeIdx(string output_file, string ab, double constr_time)
{
	cout << "=> Saving idx to disk..." << endl;
	this->_hitmap->store(output_file, ab, constr_time);
	cout << "=> Done!" << endl;
}

void HitmapAligner::loadIdx(string input_file, string *dseq, unsigned long qlen)
{
	cout << "=> Loading idx from disk..." << endl;

	this->_hitmap = new hmp::HitmapHash();
	this->_hitmap->load(input_file, dseq);
	this->_dseq = dseq;

	this->_buckets_num = this->_hitmap->getBucketsNum();
	this->_qlen = qlen;
	this->_fnum = this->_hitmap->getFNum();
	this->_qthr = this->_hitmap->getQthr();
	this->_flen = this->_hitmap->getFLen();
	this->_ab = this->_hitmap->getAb();

	cout << "HITMAP initialisation is starting..." << endl;

	this->_hitmap->initCounterBitsets(this->_fnum);

	cout << "HITMAP initialisation is done." << endl;
	cout << "\t"
		 << "buck_num=" << this->_buckets_num << ", qlen=" << this->_qlen << ", fnum=" << this->_fnum << ", flen=" << this->_flen << ", qthr=" << this->_qthr << endl;

	cout << "=> Done!" << endl;
}

void HitmapAligner::alignEditDistance(string &qseq)
{
	this->_wins.clear(); // empty any windows from previous run
	this->_results.clear();
	this->findWindows(qseq); // find the windows
	this->keepUniqueWindows();
	/*	vector<int> xx; xx = this->getWindows(); vector<int>::iterator xxx; xxx = xx.begin();
		while(xxx!= xx.end())
		{ cout<<"window: "<<(*xxx)<<" - "<<(*xxx)+qseq.length()+this->_qthr<<endl; xxx++;}*/
	this->findResultsEditDistance(qseq); // find the results (i.e., the approx. occurrences)
	return;
}

void HitmapAligner::filtering(string &qseq)
{
	this->_wins.clear(); // empty any windows from previous run
	this->_results.clear();
	this->findWindows(qseq); // find the windows
	return;
}

VerificationStatistics HitmapAligner::alignEditDistanceVerbose(string &qseq)
{
	VerificationStatistics stats;
	this->_wins.clear();
	this->_results.clear();
	this->findWindows(qseq);
	this->keepUniqueWindows();
	/*	vector<int> xx; xx = this->getWindows(); vector<int>::iterator xxx; xxx = xx.begin();
		while(xxx!= xx.end())
		{ cout<<"window: "<<(*xxx)<<" - "<<(*xxx)+qseq.length()+this->_qthr<<endl; xxx++;}*/
	this->_hitmap->setVerificationStatistics(qseq, this->_ab);
	stats = this->_hitmap->getVerificationStatistics();
	this->findResultsEditDistance(qseq);
	return stats;
}

void HitmapAligner::findWindows(string &qseq)
{
	this->_hitmap->findWindows(qseq);
}

void HitmapAligner::keepUniqueWindows(void)
{
	this->_hitmap->keepUniqueWindows();
}

void HitmapAligner::findResultsEditDistance(string &qseq)
{
	this->_hitmap->verifyWindows(qseq);
}

vector<int> HitmapAligner::getWindows()
{
	return this->_hitmap->getWindows();
}

vector<int> HitmapAligner::getResults()
{
	this->_hitmap->keepUniqueResults();
	return this->_hitmap->getResults();
}

void HitmapAligner::close()
{
}

HitmapLightAligner::HitmapLightAligner(unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string ab)
{
	this->_qlen = qlen;
	this->_qthr = qthr;
	this->_fnum = fnum;
	this->_flen = floor(this->_qlen / this->_fnum);
	this->_block_size = block_size;
	this->_buckets_num = buckets_num;
	this->_errno = 0;
	this->_ab = ab;
}

HitmapLightAligner::HitmapLightAligner(void)
{
}

HitmapLightAligner::~HitmapLightAligner()
{
	delete this->_hitmap;
}

void HitmapLightAligner::createIdx(string &dseq, string mode)
{
	this->_dseq = &dseq; // get a pointer to the data sequence
	if (mode == "verbose")
	{
		cout << endl;
		cout << "Building 'HitmapLight index':" << endl;
		cout << "\t[begin]" << endl;
	}

	if (this->_errno != 0)
	{
		cout << "ERROR -- [Creating index] An error occurred, cannot build the index..." << endl;
		return;
	}

	this->_hitmap = new hmp::HitmapLightHash(&dseq, this->_fnum, this->_flen, this->_qlen, this->_qthr, this->_block_size, this->_buckets_num);
	this->_hitmap->build();

	if (mode == "verbose")
	{
		cout << "\t[end]" << endl;
		cout << endl;
	}
}

void HitmapLightAligner::createIdx(string &dseq, unsigned long qlen, unsigned long qthr, unsigned long fnum, unsigned long block_size, unsigned long buckets_num, string mode)
{
	this->_qlen = qlen;
	this->_qthr = qthr;
	this->_fnum = fnum;
	this->_flen = floor(this->_qlen / this->_fnum);
	this->_block_size = block_size;
	this->_buckets_num = buckets_num;
	this->_errno = 0;

	this->createIdx(dseq, mode);
}

void HitmapLightAligner::storeIdx(string output_file, string ab, double constr_time)
{
	cout << "=> Saving idx to disk..." << endl;
	this->_hitmap->store(output_file, ab, constr_time);
	cout << "=> Done!" << endl;
}

void HitmapLightAligner::loadIdx(string input_file, string *dseq, unsigned long qlen)
{
	cout << "=> Loading idx from disk..." << endl;

	this->_hitmap = new hmp::HitmapLightHash();
	this->_hitmap->load(input_file, dseq, qlen);
	this->_dseq = dseq;

	this->_buckets_num = this->_hitmap->getBucketsNum();
	this->_qlen = qlen;
	this->_fnum = this->_hitmap->getFNum();
	this->_qthr = this->_hitmap->getQthr();
	this->_flen = this->_hitmap->getFLen();
	this->_ab = this->_hitmap->getAb();

	cout << "\t"
		 << "buck_num=" << this->_buckets_num << ", qlen=" << this->_qlen << ", fnum=" << this->_fnum << ", flen=" << this->_flen << endl;

	cout << "=> Done!" << endl;
}

void HitmapLightAligner::alignEditDistance(string &qseq)
{
	this->_wins.clear(); // empty any windows from previous run
	this->_results.clear();
	this->findWindows(qseq); // find the windows
	this->keepUniqueWindows();
	this->findResultsEditDistance(qseq); // find the results (i.e., the approx. occurrences)
	return;
}

VerificationStatistics HitmapLightAligner::alignEditDistanceVerbose(string &qseq)
{
	VerificationStatistics stats;
	this->_wins.clear();
	this->_results.clear();
	this->findWindows(qseq);
	this->keepUniqueWindows();
	vector<int>::iterator xx;
	xx = this->_hitmap->getWindows().begin();
	while (xx != this->_hitmap->getWindows().end())
	{
		cout << "window: " << (*xx) << " - " << (*xx) + qseq.length() + this->_qthr << endl;
		xx++;
	}
	this->_hitmap->setVerificationStatistics(qseq, this->_ab);
	stats = this->_hitmap->getVerificationStatistics();
	this->findResultsEditDistance(qseq);
	return stats;
}

void HitmapLightAligner::findWindows(string &qseq)
{
	this->_hitmap->findWindows(qseq);
}

void HitmapLightAligner::keepUniqueWindows(void)
{
	this->_hitmap->keepUniqueWindows();
}

void HitmapLightAligner::findResultsEditDistance(string &qseq)
{
	this->_hitmap->verifyWindows(qseq);
}

vector<int> HitmapLightAligner::getWindows()
{
	return this->_hitmap->getWindows();
}

vector<int> HitmapLightAligner::getResults()
{
	this->_hitmap->keepUniqueResults();
	return this->_hitmap->getResults();
}

void HitmapLightAligner::close()
{
}