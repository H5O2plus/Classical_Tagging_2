#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>

#include <taglib/fileref.h>
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

int main() {
	std::cout << "See README.md for more info." << std::endl;

	std::ifstream fin("byworks.json");
	json jin;

	try {
		jin = json::parse(fin);
	}
	catch (json::parse_error & e)
	{
		std::cerr << e.what() << std::endl;
	}

	//std::cout << jin;



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
}