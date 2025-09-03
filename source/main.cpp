#include "BuilderFilework.h"
#include "alias.h"
#include <thread>

const size_t numThreads = std::thread::hardware_concurrency();

// REQS: g++, gcc, make, cmake
// build-essential clang lld bison flex 
// libreadline-dev gawk tcl-dev libffi-dev git 
// graphviz xdot pkg-config python3 libboost-system-dev 
// libboost-python-dev libboost-filesystem-dev zlib1g-dev

bool checkPrograms(const std::vector<std::string>& v){
	std::vector<std::string> missing;
	for(int i = 0; i < v.size(); ++i)
		if(!checkProgram(v[i])) missing.push_back(v[i]);
	if(missing.size() > 0){
		std::cout << "Cannot install yosys, missing:" << std::endl;
		for(int i = 0; i < missing.size(); ++i)
			std::cout << "\t" << missing[i] << std::endl;
		std::cout << "Install all of this and try again" << std::endl;
		std::cout << std::endl;
		return false;
	}
	return true;
}
bool checkLibs(const std::vector<std::string>& v){
	char y = 'n';
	std::cout << "Libraries required to install yosys:" << std::endl;
	for(int i = 0; i < v.size(); ++i)
		std::cout << "\t" << v[i] << std::endl;
	std::cout << "Continue? [y/n]: ";
	std::cin >> y;
	std::cout << std::endl;
	if(y != 'y') return false;
	return true;
}
bool checkLibreadlineDev() {
	int result = std::system("pkg-config --exists readline");
    return (result == 0);
}

bool checkTclDev() {
  int result = std::system("pkg-config --exists tcl");
  return (result == 0);
}

bool checklibffidev(){
	int result = std::system("pkg-config --exists libffi-dev");
	return (result == 0);
}

void eraseKernel(const std::string& path){
	std::ifstream in(path);
	std::string line;
	std::vector<std::string> lines;
	while(std::getline(in, line))
		lines.push_back(line);
	in.close();
	std::ofstream out(path);
	for(int i = 0; i < lines.size(); ++i){
		if(lines[i].find("kernel/") != std::string::npos && 
			lines[i].find("include") != std::string::npos)
		{
			int pos = lines[i].find("kernel/");
			lines[i] = std::string(lines[i].begin(), lines[i].begin() + pos) + std::string(lines[i].begin() + pos + 7, lines[i].end());
		}
		out << lines[i] << std::endl;
	}
	out.close();
}

int main(int argc, char* argv[]){
	std::vector<std::string> args;
	for(int i = 1; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	if(args.size() != 0 && args[0] == "uninstall"){
		std::string cmd = "rm -rf " + root;
		system(cmd.c_str());
		removeAlias("yosys", root + "/yosys/yosys");
		std::cout << "Yosys uninstalled" << std::endl;
		return 0;
	}
	if(exists(root)){
		if(args.size() == 0 || args[0] != "force"){
			std::cout << root << " already exists" << std::endl;
			std::cout << "Remove " << root << " to continue installation" << std::endl;
			std::cout << "Or run make install-force" << std::endl;
			std::cout << "make install-force will not remove " << root << " folder" << std::endl;
			std::cout << "If you want to completely reinstall the program remove " << root << " manually" << std::endl;
			std::cout << std::endl;
			return 0;
		}
	}
	std::cout << "====================== WARNING ======================" << std::endl;
	std::cout << "Yosys installation is configured for the gcc compiler" << std::endl;
	std::cout << "If you use another compiler you will have to deal with the configs yourself or install gcc" << std::endl;
	std::cout << std::endl;
	if(!checkLibs({"libreadline-dev", "tcl-dev", "libffi-dev"})) return 0;
	if(!checkPrograms({"make", "git", "bison", "flex"})) return 0;
	std::string cmd;
	if(!exists(root)){
		cmd = "mkdir " + root;
		system(cmd.c_str());
	}
	if(!exists(root + "/yosys")){
		// working commit 819b963
		cmd = "git clone --recurse-submodules https://github.com/YosysHQ/yosys.git " + root + "/yosys";
		system(cmd.c_str());
	}
	if(!exists(root + "/yosys-slang")){
		// working commit c962a6e
		cmd = "git clone --recursive https://github.com/povik/yosys-slang " + root + "/yosys-slang";
		system(cmd.c_str());
	}
	auto dirs_slang = getDirs(root + "/yosys-slang/src");
	auto dirs_yosys = getDirs(root + "/yosys/kernel");
	for(int i = 1; i < dirs_slang.size(); ++i)
		dirs_slang[i] = getName(dirs_slang[i]);
	for(int i = 1; i < dirs_yosys.size(); ++i){
		if(find(dirs_slang, getName(dirs_yosys[i])) == -1){
			std::string cmd = "cp " + dirs_yosys[i] + " " + root + "/yosys-slang/src";
			system(cmd.c_str());
			std::cout << "Copied: " << dirs_yosys[i] << std::endl;
		}
	}
	dirs_slang = getDirs(root + "/yosys-slang/src");
	for(int i = 1; i < dirs_slang.size(); ++i){
		std::cout << "Altering " << getName(dirs_slang[i]) << std::endl;
		eraseKernel(dirs_slang[i]);
	}
	std::string line;
	std::vector<std::string> lines;
	std::ifstream in(root + "/yosys-slang/src/CMakeLists.txt");
	if(!in.is_open()){
		std::cout << "================== ERROR ==================" << std::endl;
		std::cout << "Cannot open " << root << "/yosys-slang/src/CMakeLists.txt" << std::endl;
		std::cout << std::endl;
		return 0;
	}
	while(std::getline(in, line)) lines.push_back(line);
	in.close();
	if(lines[0] != "set(YOSYS_CONFIG \"" + root + "/yosys/yosys-config\")"){
		std::ofstream out(root + "/yosys-slang/src/CMakeLists.txt");
		out << "set(YOSYS_CONFIG \"" << root << "/yosys/yosys-config\")" << std::endl;
		for(int i = 0; i < lines.size(); ++i) out << lines[i] << std::endl;
		out.close();
	}
	
	// -------------------- COMPILATION ----------------  //
	
	// yosys
	if(!exists(root + "/yosys/yosys")){
		cmd = "make config-gcc -C " + root + "/yosys";
		system(cmd.c_str());
		cmd = "make -C " + root + "/yosys -j " + std::to_string(numThreads); 
		system(cmd.c_str());
	}
	// slang
	if(!exists(root + "/yosys-slang/build/slang.so")){
		cmd = "make -C " + root + "/yosys-slang -j " + std::to_string(numThreads); 
		system(cmd.c_str());
	}
	// -------------------------------------------------- //
	
	cmd = "mkdir " + root + "/yosys/share/plugins";
	system(cmd.c_str());
	cmd = "cp " + root + "/yosys-slang/build/slang.so " + root + "/yosys/share/plugins";
	system(cmd.c_str());
	addAlias("yosys", root + "/yosys/yosys");

	std::cout << std::endl;
	if(exists(root + "/yosys/yosys")) std::cout << "SUCCES" << std::endl;
	return 0;
}