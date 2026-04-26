#ifndef FILE_READ_HPP
#define FILE_READ_HPP

#include <iostream>
#include <fstream>

std::string read_file(const char* filename){
	std::ifstream file(filename);
	std::string str;
	std::string file_contents;

	while (std::getline(file, str)) {
		file_contents += str;
		file_contents.push_back('\n');
	}

	return file_contents;
}

#endif
