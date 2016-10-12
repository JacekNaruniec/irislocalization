#include "file_utils.h"
#include <Windows.h>
#include <vector>
#include "dirent.h"

using namespace std; 

void skipLine(FILE *f)
{
	int c = -1;

	while (c!='\n')
		c = getc(f);
}

bool checkFileExistence(const char *filename)
{
	FILE *f;

	if (fopen_s(&f, filename, "rt")!=0)
		return false;

	fclose(f);
	return true;
}

vector<string> loadAllFileNames(string path, string extension, bool full_name)
{
	DIR *dir;
	struct dirent *ent;
	vector<string> out;
	size_t ext_length = extension.length();
	if ((dir = opendir(path.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			string current; 
			current = ent->d_name;
			if (current.length() < ext_length)
				continue;
			if (current.substr(max(0, current.length() - ext_length), current.length()) == extension)
				out.push_back(current);		
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		return out;
	}

	return out;
}

void makeDir(const char *dir)
{
	CreateDirectoryA(dir, 0);
}
