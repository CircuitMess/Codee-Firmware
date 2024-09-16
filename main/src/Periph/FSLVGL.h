#ifndef CLOCKSTAR_FIRMWARE_FSLVGL_H
#define CLOCKSTAR_FIRMWARE_FSLVGL_H

#include <lvgl.h>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "FS/RawCache.h"
#include "FS/ArchiveCache.h"
#include "Services/Allocator.h"

class FSLVGL {
public:
	FSLVGL(char letter, Allocator* alloc = nullptr);
	virtual ~FSLVGL();

	void loadCache();
	void unloadCache();

	static void themeChange();
	static void menuChange();

	static void reloadMenu();

private:
	lv_fs_drv_t drv;
	const std::string Root = "/spiffs";

	Allocator* alloc = nullptr;

	RawCache cache;

	std::vector<std::string> getCacheFiles() const;
	bool cacheLoaded = false;

	static FSLVGL* instance;
	static bool themeChanged;
	static bool menuChanged;

	struct SubArchive {
		FileArchive* archive = nullptr;
		Allocator* alloc = nullptr;
		void* allocBuf = nullptr;
		operator bool(){ return archive != nullptr || alloc != nullptr; }
	};

	static constexpr size_t MenuSize = 42000; // Biggest icon * num of icons
	static constexpr size_t ThemeSize = 24000; // Size of Theme1 (biggest theme)
	static constexpr size_t AchisSize = 12000; // Size of Theme1 (biggest theme)

	struct {
		SubArchive menu;
		SubArchive theme;
		SubArchive achis;
	} archives = { };

	void loadMenu();
	void unloadMenu();

	void loadTheme();
	void unloadTheme();

	void loadAchis();
	void unloadAchis();

	static constexpr File* getFile(void* fp){ return (File*) fp; }

	void* lvOpen(const char* path, lv_fs_mode_t mode);
	lv_fs_res_t lvClose(void* file);
	lv_fs_res_t lvRead(void* fp, void* buf, uint32_t btr, uint32_t* br);
	lv_fs_res_t lvWrite(void* fp, const void* buf, uint32_t btw, uint32_t* bw);
	lv_fs_res_t lvSeek(void* fp, uint32_t pos, lv_fs_whence_t whence);
	lv_fs_res_t lvTell(void* fp, uint32_t* pos);
	void* lvDirOpen(const char* path);
	lv_fs_res_t lvDirRead(void* dir, char* fn);
	lv_fs_res_t lvDirClose(void* dir);

};


#endif //CLOCKSTAR_FIRMWARE_FSLVGL_H
