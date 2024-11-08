#ifndef CLOCKSTAR_FIRMWARE_JIGHWTEST_H
#define CLOCKSTAR_FIRMWARE_JIGHWTEST_H

#include <vector>
#include "Devices/Display.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include <esp_efuse.h>
#include <esp_spiffs.h>
#include "Periph/I2C.h"
#include "Devices/RTC.h"
#include <Pins.hpp>

struct Test {
	bool (* test)();
	const char* name;
	void (* onFail)();
};

class JigHWTest {
public:
	JigHWTest();
	static bool checkJig();
	void start();

private:
	static Display* display;
	static LGFX_Device* canvas;
	static I2C* i2c;
	static RTC* rtc;
	static JigHWTest* test;
	std::vector<Test> tests;
	const char* currentTest;

	void log(const char* property, const char* value);
	void log(const char* property, float value);
	void log(const char* property, double value);
	void log(const char* property, bool value);
	void log(const char* property, uint32_t value);
	void log(const char* property, int32_t value);
	void log(const char* property, const std::string& value);

	static bool BatteryRef();
	static bool SPIFFSTest();
	static uint32_t calcChecksum(FILE* file);
	static bool RTCTest();
	static bool Time1();
	static bool Time2();
	static bool HWVersion();
	static bool Buttons();

	void instr(const char* msg);
	void AudioVisualTest();
	void rgb();

	static const int16_t VoltRef = 2600;

	static constexpr uint32_t CheckTimeout = 500;

	static constexpr esp_vfs_spiffs_conf_t spiffsConfig = {
			.base_path = "/spiffs",
			.partition_label = "storage",
			.max_files = 8,
			.format_if_mount_failed = false
	};
};

#endif //CLOCKSTAR_FIRMWARE_JIGHWTEST_H
