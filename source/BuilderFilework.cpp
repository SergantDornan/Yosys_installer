#include "BuilderFilework.h"
// Следующая строка заполняется инсталлятором, не менять ее
const std::string root = getHomedir() + "/Yosys";
const std::string cd = cwd();
std::string cwd(){
    char cwd0[PATH_MAX];
    if (getcwd(cwd0, sizeof(cwd0)) != nullptr) {
        return cwd0;
    } 
    else {
        std::cout << "==================== ERROR ====================" << std::endl;
        std::cout << "====== some error in install.cpp : std::string cwd() ======";
        std::cout << std::endl;
        return "";
    }
}
std::string getName(const std::string& path){
    for(int i = path.size() - 1; i >= 0; --i){
        if(path[i] == '/')
            return std::string(path.begin() + i + 1, path.end()); 
    }
    return "";
}
std::string getFolder(const std::string& path){
    for(int i = path.size() - 1; i >= 0; --i){
        if(path[i] == '/')
            return std::string(path.begin(), path.begin() + i);
    }
    return "";
}
std::string getHomedir(){
    const char* homeDir = getenv("HOME");
    if (homeDir) {
        return homeDir;
    } else {
        std::cout << "======================== ERROR ========================" << std::endl;
        std::cout << "==== some error in BuilderFilework.cpp: getHomedir() ====" << std::endl;
        std::cout << std::endl;      
        return "";
    }
}
bool exists(const std::string& path){
    return std::filesystem::exists(path);
}
void appendToFile(const std::string& path, const std::string& s){
    std::ofstream out(path, std::ios::app);
    if(out.is_open()){
        out << s;
        out.close();
    }
    else{
        std::cout << "============================ ERROR ============================" << std::endl;
        std::cout << "======= BuilderFilework.cpp: appendToFile() ======" << std::endl;
        std::cout << "Cannot open file: " << path << std::endl;
        std::cout << std::endl;
    }
}
std::string formatTime(time_t timestamp) {
    std::tm *timeInfo = localtime(&timestamp);
    if (timeInfo == nullptr) {
        std::cout << "===================== ERROR =====================" << std::endl;
        std::cout << "====== BuilderFilework.cpp: formatTime() - some error idn ======" << std::endl;
        std::cout << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
std::string getChangeTime(const std::string& path){
    const char *filename = path.c_str();
    struct stat fileInfo;
    if (stat(filename, &fileInfo) != 0) {
        std::cout << "===================== ERROR =====================" << std::endl;
        std::cout << "====== BuilderFilework.cpp: getChangeTime() ======" << std::endl;
        std::cout << "Error getting file information: " << filename << std::endl;
        std::cout << std::endl;
        return "";
    }
    time_t modificationTime = fileInfo.st_mtime;
    return formatTime(modificationTime);
}
std::string getExt(const std::string& file){
    int index = -1;
    for(int i = file.size()-1; i>=0; --i){
        if(file[i] == '.' && i != 0){
            index = i;
            break;
        }
    }
    if(index != -1)
        return std::string(file.begin() + index + 1,file.end());
    return "-1";
}
std::string getNameNoExt(const std::string& path){
    std::string long_name = path;
    for(int i = path.size() - 1; i >= 0; --i){
        if(path[i] == '/'){
            long_name = std::string(path.begin() + i + 1, path.end()); 
            break;
        }
    }
    int index = -1;
    for(int i = long_name.size()-1; i>=0; --i){
        if(long_name[i] == '.' && i != 0){
            index = i;
            break;
        }
    }
    if(index != -1)
        return std::string(long_name.begin(),long_name.begin() + index);
    else
        return long_name;
}
