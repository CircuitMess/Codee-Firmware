#ifndef BIT_FIRMWARE_RAMFILE_H
#define BIT_FIRMWARE_RAMFILE_H

#include <cstdint>
#include <vector>
#include <cstddef>
#include <string>
#include "FileImpl.h"
#include "File.h"

class RamFile : public FileImpl {
public:
	RamFile(File file, bool use32bAligned = false);
	RamFile(uint8_t* data, size_t size, const char* name);
	RamFile(const char* path, bool use32bAligned = false);
	~RamFile() override;

	operator bool() override;

	static File open(const File& file, bool use32bAligned = false);
	void close() override;

	size_t size() const override;
	const char* name() const override;
	std::string path();


	size_t read(uint8_t* dest, size_t len) override;
	size_t write(const uint8_t* buf, size_t size) override;
	void flush() override;

	bool seek(int pos, int whence = SEEK_SET) override;
	size_t pos() const override;

private:
	uint8_t* data = nullptr;
	size_t cursor = 0;
	std::string filePath;
	size_t fileSize = 0;
	bool borrowed = false;

};


#endif //BIT_FIRMWARE_RAMFILE_H
