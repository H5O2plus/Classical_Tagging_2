#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <filesystem>
#include <stdexcept>

#include <FLAC++/all.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::ostream& operator<<(std::ostream& os, json::value_t type) {
	os << "json::value_t::";
	if (type == json::value_t::null) {
		os << "null";
	}
	else if (type == json::value_t::boolean) {
		os << "boolean";
	}
	else if (type == json::value_t::number_integer) {
		os << "number_integer";
	}
	else if (type == json::value_t::number_unsigned) {
		os << "number_unsigned";
	}
	else if (type == json::value_t::number_float) {
		os << "number_float";
	}
	else if (type == json::value_t::object) {
		os << "object";
	}
	else if (type == json::value_t::array) {
		os << "array";
	}
	else if (type == json::value_t::string) {
		os << "string";
	}
	else {
		os << "__unknown__";
	}

	return os;
}

std::vector<std::string> strToVec(std::string str) {
	std::stringstream strs(str);
	std::istream_iterator<std::string> begin(strs);
	std::istream_iterator<std::string> end;
	return std::vector<std::string>(begin, end);
}

int main(int argc, char** argv) {
	std::cout << "See README.md for more info." << std::endl;
	std::string version = "0.1";
	std::string cmd;
	std::vector<std::string> files;
	std::string path = "./music/";


	std::ifstream fin("byworks.json");
	json jin;

	try {
		jin = json::parse(fin);
	}
	catch (json::parse_error & e)
	{
		std::cerr << e.what() << std::endl;
	}

	std::cout << "CTag v." << version << std::endl;


	do {
		std::cout << "> ";
		std::getline(std::cin, cmd);
		std::vector<std::string> cmd_parsed(strToVec(cmd));

		try {
			if (cmd_parsed[0] == "init") {
				if (cmd_parsed[1] == "tags") {
					files.clear();
					
					for (const auto& entry : std::filesystem::directory_iterator(path)) {
						files.push_back(entry.path().u8string());
					}
				}
				else {
					throw std::invalid_argument("No such option.");
				}
			}
		}
		catch (const std::invalid_argument& e) {
			std::cerr << "Invalid argument: " << e.what() << std::endl;
		}
		

	} while (cmd != "exit");



	//Last names of composers, constructed by iterating works
	std::vector<std::string> composers;
	for (auto it = jin["works"].begin(); it != jin["works"].end(); it++) {
		composers.push_back((*it)["composer"].get<std::string>());
	}
	std::sort(composers.begin(), composers.end());
	composers.erase(std::unique(composers.begin(), composers.end()), composers.end());
	std::cout << "List of composers in the database:" << std::endl;
	for (auto it = composers.begin(); it != composers.end(); it++) {
		std::cout << *it << std::endl;
	}

	std::string query;

	std::getline(std::cin, query);

	/*
	std::vector<std::string> composers;
	for (auto it = music_data["works"].begin(); it != music_data["works"].end(); it++) {
		composers.push_back((*it)["composer"].get<std::string>());
	}
	std::sort(composers.begin(), composers.end());
	composers.erase(std::unique(composers.begin(), composers.end()), composers.end());
	return composers;
	*/
}