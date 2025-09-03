#ifndef BUILDERFILEWORK
#define BUILDERFILEWORK
#include "filework.h"
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <iomanip>
#include <sstream>
extern const std::string root;
std::string cwd();
std::string getExt(const std::string&);
std::string getName(const std::string&);
std::string getFolder(const std::string&);
std::string getHomedir();
bool exists(const std::string&);
void appendToFile(const std::string&, const std::string&);
std::string formatTime(time_t timestamp);
std::string getChangeTime(const std::string&);
std::string getNameNoExt(const std::string&);
#endif