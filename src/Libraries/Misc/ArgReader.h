/************************************************************************************************/ /*
																									* @file
																									*
																									*
																									*
																									* @author Thanasis Vergoulis
																									**************************************************************************************************/

#ifndef ARGREADER_H
#define ARGREADER_H

#include <iostream>
#include <string>
#include <list>

using namespace std;

/**
 * @brief An object which reads the command line arguments.
 *
 * @var _options An array of all the options provided by the program.
 * @var _descriptions An array of all the descriptions of the options.
 * @var _opt_num The number of options.
 * @var _opts_found The options found at the command line.
 * @var _opts_found_values The values of the found options.
 *
 * @author Thanasis Vergoulis
 */
class ArgReader
{
	string *_options;
	string *_descriptions;
	int _opt_num;
	list<string> _opts_found;
	list<string> _opts_found_values;

public:
	/**
	 * @brief The constructor.
	 *
	 * @author Thanasis Vergoulis
	 */
	ArgReader(string *options, string *descriptions, int opt_num);

	/**
	 * @brief It reads the arguments and set the flags.
	 *
	 * @param argc The 'argc' parameter of command line.
	 * @param argv The 'argv' parameter of command line.
	 *
	 * @author Thanasis Vergoulis
	 */
	void read(int argc, char *argv[]);

	/**
	 * @brief It returns the value of the option if opt is in _opts_found. Otherwise, it returns
	 * the null string.
	 *
	 * @param opt The option in question.
	 *
	 * @author Thanasis Vergoulis
	 */
	string isActive(string opt);

	/**
	 * @brief It prints on standard output a help for the program.
	 *
	 * @author Thanasis Vergoulis
	 */
	void help(void);

	string check(int argc, char *argv[]);
};

#endif