#ifndef CLOCKSTAR_FIRMWARE_FSLVGL_H
#define CLOCKSTAR_FIRMWARE_FSLVGL_H

#include <lvgl.h>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "FS/RawCache.h"
#include "FS/FileArchive.h"

class FSLVGL {
public:
	FSLVGL(char letter);
	virtual ~FSLVGL();

	void loadCache();
	void unloadCache();

private:
	lv_fs_drv_t drv;
	const std::string Root = "/spiffs";

	static FSLVGL* instance;

	static constexpr File* getFile(void* fp){ return (File*) fp; }

	FileArchive* archive = nullptr;

	void* lvOpen(const char* path, lv_fs_mode_t mode);
	lv_fs_res_t lvClose(void* file);
	lv_fs_res_t lvRead(void* fp, void* buf, uint32_t btr, uint32_t* br);
	lv_fs_res_t lvWrite(void* fp, const void* buf, uint32_t btw, uint32_t* bw);
	lv_fs_res_t lvSeek(void* fp, uint32_t pos, lv_fs_whence_t whence);
	lv_fs_res_t lvTell(void* fp, uint32_t* pos);
	void* lvDirOpen(const char* path);
	lv_fs_res_t lvDirRead(void* dir, char* fn, uint32_t fnLen);
	lv_fs_res_t lvDirClose(void* dir);

};


#endif //CLOCKSTAR_FIRMWARE_FSLVGL_H
