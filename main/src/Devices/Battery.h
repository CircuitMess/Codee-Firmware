#ifndef BIT_LIBRARY_BATTERY_H
#define BIT_LIBRARY_BATTERY_H

#include <hal/gpio_types.h>
#include <atomic>
#include "Util/Threaded.h"
#include "Periph/ADC.h"
#include "Util/Hysteresis.h"
#include "Periph/Timer.h"
#include <mutex>
#include <esp_efuse.h>

class Battery : private SleepyThreaded {
public:
	Battery();
	void begin();

	enum Level { Critical = 0, VeryLow, Low, Mid, High, VeryHigh, Full, COUNT };

	uint8_t getPerc() const;
	Level getLevel() const;

	struct Event {
		enum {
			LevelChange
		} action;
		union {
			Level level;
		};
	};

	static int16_t getVoltOffset();
	static uint16_t mapRawReading(uint16_t reading);

	bool isShutdown() const;

private:
	static constexpr uint32_t MeasureIntverval = 100;

	ADC adc;
	Hysteresis hysteresis;

	void sleepyLoop() override;

	void sample(bool fresh = false);
	bool shutdown = false;

	void off();

	static constexpr esp_efuse_desc_t adc1_low = { EFUSE_BLK3, 0, 8 };
	static constexpr const esp_efuse_desc_t* efuse_adc1_low[] = { &adc1_low, nullptr };
	static constexpr esp_efuse_desc_t adc1_high = { EFUSE_BLK3, 8, 8 };
	static constexpr const esp_efuse_desc_t* efuse_adc1_high[] = { &adc1_high, nullptr };

};

#endif //BIT_LIBRARY_BATTERY_H
