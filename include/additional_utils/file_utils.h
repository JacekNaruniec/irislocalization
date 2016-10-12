#ifndef FILEUTILS
#define FILEUTILS

#include <string>
#include <vector>

void skipLine(FILE *f);
bool checkFileExistence(const char *filename);
std::vector<std::string> loadAllFileNames(std::string path, std::string extension, bool full_name);
void makeDir(const char *dir);

#endif
