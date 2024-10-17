#include "FSLVGL.h"
#include "FS/SPIFFS.h"
#include "Util/stdafx.h"
#include <dirent.h>
#include <esp_spiffs.h>
#include <string>
#include <unordered_map>
#include <cstring>

std::initializer_list<std::string> GeneralCache = {
		"/Stats/Bar.bin",
		"/Stats/BarLong.bin",
		"/Stats/Batt.bin",
		"/Stats/Happ.bin",
		"/Stats/Oil.bin",
		"/Stats/Xp.bin",

		"/OS/1_0.bin",
		"/OS/10.bin",
		"/OS/9000.bin",
		"/OS/9001.bin",
		"/OS/One.bin",
		"/OS/X.bin",

		"/Menu/Game1.bin",
		"/Menu/Game2.bin",
		"/Menu/Game3.bin",
		"/Menu/Game4.bin",
		"/Menu/Game5.bin",
		"/Menu/Game6.bin",
		"/Menu/Settings.bin"
};

const char* TAG = "FSLVGL";

FSLVGL* FSLVGL::instance = nullptr;

FSLVGL::FSLVGL(char letter){
	lv_fs_drv_init(&drv);

	drv.letter = letter;
	drv.ready_cb = [](lv_fs_drv_t*){ return true; }; // Is the drive ready to use?
	drv.open_cb = [](lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode){ return ((FSLVGL*) drv->user_data)->lvOpen(path, mode);  };
	drv.close_cb = [](lv_fs_drv_t* drv, void* file){ return ((FSLVGL*) drv->user_data)->lvClose(file);  };
	drv.read_cb = [](lv_fs_drv_t* drv, void* file, void* buf, uint32_t btr, uint32_t* br){ return ((FSLVGL*) drv->user_data)->lvRead(file, buf, btr, br); };
	drv.write_cb = [](lv_fs_drv_t* drv, void* file, const void* buf, uint32_t btw, uint32_t* bw){ return ((FSLVGL*) drv->user_data)->lvWrite(file, buf, btw, bw); };
	drv.seek_cb = [](lv_fs_drv_t* drv, void* file, uint32_t pos, lv_fs_whence_t whence){ return ((FSLVGL*) drv->user_data)->lvSeek(file, pos, whence); };
	drv.tell_cb = [](lv_fs_drv_t* drv, void* file, uint32_t* pos){ return ((FSLVGL*) drv->user_data)->lvTell(file, pos); };

	drv.dir_open_cb = [](lv_fs_drv_t* drv, const char* path){ return ((FSLVGL*) drv->user_data)->lvDirOpen(path); };
	drv.dir_read_cb = [](lv_fs_drv_t* drv, void* dir, char* fn, uint32_t fn_len){ return ((FSLVGL*) drv->user_data)->lvDirRead(dir, fn, fn_len); };
	drv.dir_close_cb = [](lv_fs_drv_t* drv, void* dir){ return ((FSLVGL*) drv->user_data)->lvDirClose(dir); };

	drv.user_data = this;

	lv_fs_drv_register(&drv);

	instance = this;
}

FSLVGL::~FSLVGL(){
	esp_vfs_spiffs_unregister("storage");
}

void FSLVGL::loadCache(){
	if(cacheLoaded) return;
	cacheLoaded = true;

	cache.setPaths(getCacheFiles());
	cache.load();
}

void FSLVGL::unloadCache(){
	if(!cacheLoaded) return;
	cacheLoaded = false;

	cache.unload();
}

std::vector<std::string> FSLVGL::getCacheFiles() const{
	std::vector<std::string> paths;
	paths.reserve(GeneralCache.size());

	for(const auto& file : GeneralCache){
		paths.emplace_back(file);
	}

	return paths;
}

void* FSLVGL::lvOpen(const char* path, lv_fs_mode_t mode){
	File file;

	static const auto mkPtr = [](File& file){
		file.seek(0);
		File* filePtr = new File();
		*filePtr = std::move(file);
		return filePtr;
	};

	file = cache.open(path);
	if(file) return mkPtr(file);

	static const std::unordered_map<lv_fs_mode_t, const char*> Map = {
			{ LV_FS_MODE_WR, "w" },
			{ LV_FS_MODE_RD, "r" }
	};

	file = SPIFFS::open(path, Map.at(mode));
	if(file) return mkPtr(file);

	return nullptr;
}

lv_fs_res_t FSLVGL::lvClose(void* file){
	File* filePtr = (File*) file;
	delete filePtr;
	return LV_FS_RES_OK;
}

lv_fs_res_t FSLVGL::lvRead(void* fp, void* buf, uint32_t btr, uint32_t* br){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	*br = file->read((uint8_t*) buf, btr);

	return LV_FS_RES_OK;
}

lv_fs_res_t FSLVGL::lvWrite(void* fp, const void* buf, uint32_t btw, uint32_t* bw){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	*bw = file->write((const uint8_t*) buf, btw);

	return LV_FS_RES_OK;
}

lv_fs_res_t FSLVGL::lvSeek(void* fp, uint32_t pos, lv_fs_whence_t whence){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	static const std::unordered_map<lv_fs_whence_t, SeekMode> Map = {
			{ LV_FS_SEEK_SET, SeekMode::SeekSet },
			{ LV_FS_SEEK_CUR, SeekMode::SeekCur },
			{ LV_FS_SEEK_END, SeekMode::SeekEnd }
	};
	if(!Map.contains(whence)) return LV_FS_RES_INV_PARAM;

	const auto success = file->seek(pos, Map.at(whence));
	if(!success) return LV_FS_RES_INV_PARAM;

	return LV_FS_RES_OK;
}

lv_fs_res_t FSLVGL::lvTell(void* fp, uint32_t* pos){
	File* file = getFile(fp);
	if(!*file) return LV_FS_RES_NOT_EX;

	*pos = file->position();
	return LV_FS_RES_OK;
}

void* FSLVGL::lvDirOpen(const char* path){
	std::string p = Root + std::string(path);
	DIR* dir = opendir(p.c_str());
	return dir;
}

lv_fs_res_t FSLVGL::lvDirRead(void* dir, char* fn, uint32_t fnLen){
	auto entry = readdir((DIR*) dir);
	if(!entry) return LV_FS_RES_NOT_EX;
	strncpy(fn, entry->d_name, fnLen);
	return LV_FS_RES_OK;
}

lv_fs_res_t FSLVGL::lvDirClose(void* dir){
	closedir((DIR*) dir);
	return LV_FS_RES_OK;
}
