#ifndef BIT_FIRMWARE_NVSFLASH_H
#define BIT_FIRMWARE_NVSFLASH_H

#include <nvs.h>
#include <string>
#include <esp_log.h>
#include <array>
#include <unordered_map>
#include <functional>

class NVSUpgrade{
public:
	virtual ~NVSUpgrade() = default;
	virtual constexpr uint32_t getTargetVersion() const = 0;
	virtual void execute() const {}
};

class NVSFlash {
public:
	NVSFlash();
	virtual ~NVSFlash();

	template<typename T>
	bool get(const std::string& blob, T& data) const{
		size_t out_size = sizeof(T);
		auto err = nvs_get_blob(handle, blob.c_str(), &data, &out_size);

		if(err != ESP_OK){
			ESP_LOGI(TAG, "Blob %s not found", blob.c_str());
			return false;
		}

		return true;
	}

	template<typename T>
	bool set(const std::string& blob, const T& data) const{
		esp_err_t err = nvs_set_blob(handle, blob.c_str(), &data, sizeof(T));

		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s store error: %d", blob.c_str(), err);
			return false;
		}

		err = nvs_commit(handle);
		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s commit error: %d", blob.c_str(), err);
			return false;
		}

		return true;
	}

	template<typename T, size_t S>
	bool get(const std::string& blob, std::array<T, S>& dataArray) const requires(S > 0){
		size_t out_size = sizeof(T) * S;
		auto err = nvs_get_blob(handle, blob.c_str(), dataArray.data(), &out_size);

		if(err != ESP_OK){
			ESP_LOGI(TAG, "Blob %s not found", blob.c_str());
			return false;
		}

		return true;
	}

	template<typename T, size_t S>
	bool set(const std::string& blob, const std::array<T, S>& dataArray) const requires(S > 0){
		esp_err_t err = nvs_set_blob(handle, blob.c_str(), dataArray.data(), sizeof(T) * S);

		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s store error: %d", blob.c_str(), err);
			return false;
		}

		err = nvs_commit(handle);
		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s commit error: %d", blob.c_str(), err);
			return false;
		}

		return true;
	}

private:
	static constexpr const char* NVSNamespace = "Codee";
	static constexpr const char* TAG = "NVS";
	nvs_handle_t handle{};
};

#endif //BIT_FIRMWARE_NVSFLASH_H