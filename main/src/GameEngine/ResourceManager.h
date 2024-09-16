#ifndef BIT_FIRMWARE_RESOURCEMANAGER_H
#define BIT_FIRMWARE_RESOURCEMANAGER_H

#include <map>
#include <string>
#include <vector>
#include "FS/File.h"

#define RES_HEART { "c/heart.raw", {}, true }
#define RES_GOBLET { "c/goblet.raw", {}, true }

#define FILE_HEART "/heart.raw"
#define FILE_GOBLET "/goblet.raw"

struct CompressParams {
	uint8_t lookahead = 0;
	uint8_t expansion = 0;

	explicit operator bool() const{
		return lookahead && expansion;
	};
};

struct ResDescriptor {
	std::string path;
	CompressParams compressParams;
	bool inRam;
};

class ResourceManager {
public:
	ResourceManager(const char* root);
	void load(const std::vector<ResDescriptor>& descriptors);

	File getResource(std::string path);

private:
	std::map<std::string, File> resources;
	const char* root;
};


#endif //BIT_FIRMWARE_RESOURCEMANAGER_H
