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

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>
#include <taglib/tstring.h>
#include <taglib/tstringlist.h>
#include <nlohmann/json.hpp>
#include <picosha2.h>

using json = nlohmann::json;

std::ostream& operator<<(std::ostream& os, TagLib::PropertyMap prop) {
	os << prop.toString();
	return os;
}

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
	std::cout << "CTag v." << version << std::endl;

	//DB of musical works
	std::ifstream fin("byworks.json");
	json jin;

	//Read DB and parse into json
	try {
		jin = json::parse(fin);
	}
	catch (json::parse_error & e)
	{
		std::cerr << e.what() << std::endl;
	}

	//Backup tags
	json tags_backup;
	//Parse backup if exists
	if (std::filesystem::exists("backup.json")) {
		try {
			std::ifstream fin_backup("backup.json");
			tags_backup = json::parse(fin_backup);
		}
		catch (json::parse_error & e) {
			std::cerr << e.what() << std::endl;
		}
	}
	std::ofstream fout_backup("backup.json");

	std::string cmd;
	std::vector<std::string> files;
	//Path to all flacs
	std::string path = "./music/";

	//Walk in path and add all files to files
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		files.push_back(entry.path().u8string());
	}

	//Add only taggable (i.e. music) files to fileList
	TagLib::List<TagLib::FileRef> fileList;
	for (auto filepath : files) {
		TagLib::FileRef f(filepath.c_str());
		if (!f.isNull() && f.tag()) {
			fileList.append(f);
		}
		else {
			std::cout << "File ignored: " << filepath << std::endl;
		}
	}

	//Init flacs
	TagLib::List<TagLib::FileRef>::ConstIterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		//Backup and delete old tags
		json fileTags;
		TagLib::PropertyMap map = (*it).file()->properties();
		for (auto it2 = map.begin(); it2 != map.end(); it2++) {
			std::wstring key = it2->first.toWString();
			for (auto it3 = it2->second.begin(); it3 != it2->second.end(); it3++) {
				std::wstring val = it3->toWString();
				fileTags[std::string(key.begin(), key.end())] = std::string(val.begin(), val.end());
			}
		}
		(*it).file()->setProperties(TagLib::PropertyMap());
		(*it).file()->save();

		//Generate hash
		picosha2::hash256_one_by_one hasher;
		std::vector<unsigned char> hash(picosha2::k_digest_size);
		std::ifstream f(std::wstring((*it).file()->name()), std::ios::binary);
		picosha2::hash256(f, hash.begin(), hash.end());
		std::string hash_str = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
		std::cout << hash_str << std::endl;

		tags_backup[hash_str] = fileTags;

		//Add hash as flac tag
		TagLib::PropertyMap map_hash;
		map_hash.insert("sha256", TagLib::String(hash_str));
		(*it).file()->setProperties(map_hash);
		(*it).file()->save();
	}

	fout_backup << std::setw(4) << tags_backup << std::endl;

	for (auto file : files) {
		TagLib::FileRef musicin(file.c_str());
		std::cout << "original tags: " << musicin.tag()->properties() << std::endl;
		TagLib::PropertyMap myprop;
		std::cout << "assign myprop: " << myprop.insert("SUBTITLE", TagLib::StringList("testva")) << std::endl;
		std::cout << "unassigned parts of myprop: " << musicin.tag()->setProperties(myprop) << std::endl;
		std::cout << "current tags: " << musicin.tag()->properties() << std::endl;
		std::cout << "file save: " << musicin.save() << std::endl;
	}

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