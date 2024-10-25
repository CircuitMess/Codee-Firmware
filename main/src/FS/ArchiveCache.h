#ifndef BIT_FIRMWARE_ARCHIVECACHE_H
#define BIT_FIRMWARE_ARCHIVECACHE_H

#include "FileCache.h"
#include "FileArchive.h"

class ArchiveCache : public FileCache {
public:
	explicit ArchiveCache(const std::vector<std::string>& paths);

	void load() override;
	void unload() override;

	File open(const char* path) override;

private:
	const std::vector<std::string> paths;

	bool loaded = false;
	std::unordered_map<std::string, FileArchive> archives;

};


#endif //BIT_FIRMWARE_ARCHIVECACHE_H
