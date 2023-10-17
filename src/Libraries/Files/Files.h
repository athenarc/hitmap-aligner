#ifndef FILES_H
#define FILES_H

#include <iostream>
#include <sys/types.h> //For filesystem operations
#include <sys/stat.h> //For filesystem operations
#include <dirent.h>
#include <vector>
#include <cstring>
using namespace std;

/**
 * It returns true if the specified directory exists and false otherwise.
 *
 * @param dir_name The name of the directory.
 * @return True if the specified directory exists, false otherwise.
 *
 * @author Thanasis Vergoulis
 */
bool dirExists( string dir_name);

/**
 * It creates a folder with default mask permissions for all types of users.
 *
 * @param dir_name The name of the directory to be created.
 * @return True in case of successful directory creation, false otherwise.
 *
 * @author Thanasis Vergoulis
 */
bool makeDir( string dir_name);

/**
 * It removes an existing directory.
 *
 * @param dir_name The name of the directory to be deleted.
 * @return True in case of successful directory deletion, false otherwise.
 *
 * @author Thanasis Vergoulis
 */
bool removeDir( string dir_name);

/**
 * It gets the files and the directories residing in a given directory.
 *
 * @param dir_name The name (full path) of the given directory.
 * @param filenames Reference to a vector containing all the files residing in the given directory.
 * @param dirnames Reference to a vector containing all the directories residing in the given directory.
 * @return True in case of successful execution, false otherwise.
 *
 * @author Thanasis Vergoulis
 */
bool listDir( string dir_name, vector<string>& filenames, vector<string>& dirnames);

#endif