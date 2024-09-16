#ifndef BIT_FIRMWARE_FILECACHE_H
#define BIT_FIRMWARE_FILECACHE_H


#include <unordered_map>
#include "File.h"
#include "Services/Allocator.h"

class FileCache {
public:
	virtual File open(const char* path) = 0;
	virtual ~FileCache() = default;

	virtual void load(Allocator* alloc = nullptr) = 0;
	virtual void unload() = 0;

};


#endif //BIT_FIRMWARE_FILECACHE_H
