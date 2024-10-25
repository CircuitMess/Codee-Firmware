#ifndef BIT_FIRMWARE_FILEARCHIVE_H
#define BIT_FIRMWARE_FILEARCHIVE_H

#include <vector>
#include <unordered_map>
#include <string>
#include "FS/File.h"
#include <unordered_set>

class FileArchive {
public:
	FileArchive(File file, const std::unordered_set<std::string>& excluded = {});
	virtual ~FileArchive();

	File get(const char* file, const char* name = nullptr);

private:
	struct Entry {
		const std::string name;
		size_t size;
		size_t offset;
	};

	std::unordered_map<std::string, const Entry> entries;

	bool externalData = false;
	uint8_t* data = nullptr;

};


#endif //BIT_FIRMWARE_FILEARCHIVE_H
