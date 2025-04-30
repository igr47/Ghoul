#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
using json=nlohmann::json;

inline void writeToFile(const std::string& filename,const json& data){
	std::ofstream  file(filename);
	if(file.is_open()){
		file<<data.dump(4);
		file.close();
	}else{
	std::cerr<<"Error opening file: "<<filename<<"\n";
	}
}
