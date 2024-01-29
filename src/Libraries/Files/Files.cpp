#include "Files.h"

bool dirExists( string dir_name)
{
	struct stat st;
	if( stat(dir_name.c_str(),&st)==0)
	{
		if( st.st_mode & S_IFDIR !=0 )
		{
			return true;
		}
	}
	return false;
}

bool makeDir( string dir_name)
{
	int status;
	status = mkdir(dir_name.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	if( status == -1)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//NOTE: PERMISSIONS CODES AND THEIR MEANING:
//S_IRWXU	00700	mask for file owner permissions
//S_IRUSR	00400	owner has read permission
//S_IWUSR	00200	owner has write permission
//S_IXUSR	00100	owner has execute permission
//S_IRWXG	00070	mask for group permissions
//S_IRGRP	00040	group has read permission
//S_IWGRP	00020	group has write permission
//S_IXGRP	00010	group has execute permission
//S_IRWXO	00007	mask for permissions for others (not in group)
//S_IROTH	00004	others have read permission
//S_IWOTH	00002	others have write permission
//S_IXOTH	00001	others have execute permission

/*
bool removeDir( string dir_name)
{
	int status;
	
	vector<string> dirnames;
	vector<string>::iterator cur_dir;
	vector<string> filenames;
	vector<string>::iterator cur_file;
	
	if( listDir(dir_name,filenames,dirnames) )
	{
		cur_dir = dirnames.begin();
		while( cur_dir!= dirnames.end() )
		{
			if ( !removeDir((*cur_dir)))
			{
				return -1;
			}
			cur_dir++;
		}
		cur_file = filenames.begin();
		while( cur_file!= filenames.end() )
		{
			status = remove((*cur_file).c_str());
			if( status == -1 )
			{
				cout<<"ERROR - [removeDir] Unable to delete file '"<<(*cur_file)<<"'"<<endl;
				return false;
			}
			cur_file++;
		}
	}
	else
	{
		return false;
	}
	
	status = remove(dir_name.c_str());
	
	if( status == -1 )
	{
		return false;
	}
	else
	{
		return true;
	}
}*/

bool listDir( string dir_name, vector<string>& filenames, vector<string>& dirnames)
{
	//Open the directory to get its contents.
	struct dirent* dent;
	DIR* d = opendir(dir_name.c_str());
	if( d == NULL )
	{
		return false;
	}
	while( (dent = readdir(d))!=NULL )
	{
		struct stat st;

		//Ignore "." and ".."
    	if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
    		continue;

    	if (fstatat(dirfd(d), dent->d_name, &st,0) < 0)
    	{
			return false;
    	}

		//If folder
    	if (S_ISDIR(st.st_mode))
		{
			dirnames.push_back(dir_name+"/"+dent->d_name);
		}
		else
		{
			filenames.push_back(dir_name+"/"+dent->d_name);
		}
	}
	return true;
}

