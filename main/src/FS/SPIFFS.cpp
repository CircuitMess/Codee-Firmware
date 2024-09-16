#include "SPIFFS.h"
#include "FSFileImpl.h"
#include <esp_spiffs.h>
#include <esp_log.h>

static const char* TAG = "SPIFFS";

bool SPIFFS::inited = false;

bool SPIFFS::init(){
	if(inited) return true;

	esp_vfs_spiffs_conf_t conf = {
			.base_path = Prefix,
			.partition_label = "storage",
			.max_files = 8,
			.format_if_mount_failed = false
	};

	auto ret = esp_vfs_spiffs_register(&conf);
	if(ret != ESP_OK){
		if(ret == ESP_FAIL){
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		}else if(ret == ESP_ERR_NOT_FOUND){
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		}else{
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		}
		return false;
	}

	inited = true;
	return true;
}

File SPIFFS::open(const char* path, const char* mode){
	std::string p = std::string(SPIFFS::Prefix) + std::string(path);
	return FSFileImpl::open(p.c_str(), mode);
}
