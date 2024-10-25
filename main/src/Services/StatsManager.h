#ifndef CIRCUITPET_FIRMWARE_STATSMANAGER_H
#define CIRCUITPET_FIRMWARE_STATSMANAGER_H

#include <ctime>
#include "Stats.hpp"
#include "Periph/NVSFlash.h"
#include "Util/Threaded.h"
#include "Util/Events.h"

class StatsManager : public SleepyThreaded {
public:
	StatsManager();

	struct Event {
		enum { Updated } action;
		bool levelup;
	};

	void syncTime();

	/**
	 * Resets the stats to starting numbers, used when pet dies or when factory reset occurs.
	 */
	void reset();

	void update(Stats delta);

	bool hasDied() const;

	bool isHatched() const;
	void hatch();

	const Stats& get() const;
	uint8_t getLevel() const;
	static uint8_t getLevel(uint16_t exp);
	uint8_t getExpPercentage() const;
	static uint8_t getExpPercentage(uint16_t exp);

private:
	void store();
	void load();

	static constexpr uint64_t SleepTime = 5 * 60 * 1000; // 5 min
	void sleepyLoop() override;
	void timedUpdate();


	NVSFlash& nvs;
	Stats stats;
	uint8_t hoursOnZeroStats = 0; //number of hours elapsed while happiness is zero
	bool hatched = false;

	static constexpr uint16_t LevelupThresholds[] = { 50, 200, 1000, 4000, 10000 };
	static constexpr uint8_t GameOverHours = 24;
	static constexpr Stats HourlyDecrement = { 5, 3, 0 };

	static constexpr uint32_t UpdateInterval = 3600; //[s], interval when stats decrease

	static constexpr const char* BlobName = "Stats";

	static constexpr const char* TimeSaveBlobName = "StatsTime";

	std::mutex updateMut;
};

#endif //CIRCUITPET_FIRMWARE_STATSMANAGER_H
