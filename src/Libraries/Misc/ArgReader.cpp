/************************************************************************************************/
/*
 * @file
 *
 *
 *
 * @author Thanasis Vergoulis
 **************************************************************************************************/

#include "ArgReader.h"

ArgReader::ArgReader(string *options, string *descriptions, int opt_num)
{
	this->_options = options;
	this->_descriptions = descriptions;
	this->_opt_num = opt_num;
}

void ArgReader::read(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		for (int j = 0; j < _opt_num; j++)
		{
			if (argv[i] == _options[j])
			{
				_opts_found.push_back(_options[j]);
				_opts_found_values.push_back((string)argv[i + 1]);
			}
		}
	}
}

string ArgReader::check(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		for (int j = 0; j < _opt_num; j++)
		{
			if (argv[i] == _options[j])
			{
				if (i == argc - 1)
				{
					return _options[j];
				}
				if (argv[i + 1][0] == '-')
				{
					return _options[j];
				}
			}
		}
	}
	return "none";
}

string ArgReader::isActive(string opt)
{
	list<string>::iterator cur_opt;
	list<string>::iterator cur_value;
	cur_opt = _opts_found.begin();
	cur_value = _opts_found_values.begin();
	while (cur_opt != _opts_found.end())
	{
		if ((*cur_opt) == opt)
		{
			return (*cur_value);
		}
		cur_opt++;
		cur_value++;
	}
	return "";
}

void ArgReader::help()
{
	cout << "Options:" << endl;
	for (int i = 0; i < this->_opt_num; i++)
	{
		cout << "\t* '" << this->_options[i] << "': " << this->_descriptions[i] << endl;
	}
}