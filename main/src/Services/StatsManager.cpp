#include <cstring>
#include <cmath>
#include "StatsManager.h"
#include "Util/Services.h"
#include "Services/Time.h"

static const char* tag = "StatsManager";

StatsManager::StatsManager() : SleepyThreaded(SleepTime, "Stats", 4 * 1024, 5, 1), nvs(*(NVSFlash*) Services.get(Service::NVS)){
	load();
	syncTime();
	start();
}

void StatsManager::syncTime(){
	std::lock_guard lock(updateMut);

	const auto time = (Time*) Services.get(Service::Time);

	time_t lastUpdate;
	if(nvs.get(TimeSaveBlobName, lastUpdate)){
		const auto currentTime = time->getUnixTime();
		const auto timeDiff = currentTime - lastUpdate;

		if(timeDiff < 0){
			lastUpdate = currentTime;
		}else{
			for(int i = 0; i < timeDiff / UpdateInterval; i++){
				timedUpdate();
				lastUpdate += UpdateInterval;
			}
		}
	}else{
		lastUpdate = time->getUnixTime();
	}

	if(!nvs.set(TimeSaveBlobName, lastUpdate)){
		ESP_LOGW(tag, "Error saving time to NVS");
	}
}

void StatsManager::sleepyLoop(){
	syncTime();
}

void StatsManager::timedUpdate(){
	if(hasDied() || !hatched) return;

	stats -= HourlyDecrement;

	if(stats.oilLevel == 0 && hoursOnZeroStats <= GameOverHours){
		hoursOnZeroStats++;
	}else if(stats.oilLevel > 0){
		hoursOnZeroStats = 0;
	}

	Event e{ Event::Updated, false };
	Events::post(Facility::Stats, e);

	store();
}

void StatsManager::reset(){
	std::lock_guard lock(updateMut);

	stats.happiness = 100;
	stats.oilLevel = 100;
	stats.experience = 0;
	hoursOnZeroStats = 0;
	hatched = false;
	store();
}

void StatsManager::update(Stats delta){
	std::lock_guard lock(updateMut);

	if(!hatched) return;

	uint8_t oldLevel = getLevel();
	stats += delta;

	if(stats.oilLevel > 0){
		hoursOnZeroStats = 0;
	}

	ESP_LOGI(tag, "%d, %d, %d\n", stats.happiness, stats.experience, stats.oilLevel);

	bool levelUp = (oldLevel != getLevel());

	Event e{ Event::Updated, levelUp };
	Events::post(Facility::Stats, e);

	store();
}

void StatsManager::hatch(){
	std::lock_guard lock(updateMut);
	if(hatched || hasDied()) return;

	hatched = true;
	store();

	const auto time = (Time*) Services.get(Service::Time);
	const auto now = time->getUnixTime();
	nvs.set(TimeSaveBlobName, now);
}

const Stats& StatsManager::get() const{
	return stats;
}

bool StatsManager::hasDied() const{
	return hoursOnZeroStats > GameOverHours; //dies after 24hrs of zero happiness
}

bool StatsManager::isHatched() const{
	return hatched;
}

uint8_t StatsManager::getLevel() const{
	return getLevel(stats.experience);
}

uint8_t StatsManager::getLevel(uint16_t exp){
	const uint8_t levelupsNum = sizeof(LevelupThresholds) / sizeof(uint16_t);
	uint16_t requiredXP = 0;
	for(uint8_t i = 0; i < levelupsNum; i++){
		requiredXP += LevelupThresholds[i];
		if(exp < requiredXP){
			return i + 1;
		}
	}
	return levelupsNum + 1;
}

uint8_t StatsManager::getExpPercentage() const{
	return getExpPercentage(stats.experience);
}

uint8_t StatsManager::getExpPercentage(uint16_t exp){
	if(getLevel(exp) == 1){
		return (uint8_t) (((float) exp * 100) / LevelupThresholds[0]);
	}

	if(getLevel(exp) == 6){
		return 100;
	}


	uint16_t prevThreshold = 0;
	for(uint8_t i = 0; i < getLevel(exp) - 1; i++){
		prevThreshold += LevelupThresholds[i];
	}

	return (uint8_t) std::round(((float) (exp - prevThreshold) * 100.0 / (LevelupThresholds[getLevel(exp) - 1])));
}

void StatsManager::store(){
	const auto size = sizeof(Stats) + sizeof(hoursOnZeroStats) + sizeof(hatched);
	uint8_t data[size];

	memcpy(data, (uint8_t*) &stats, sizeof(Stats));
	memcpy(data + sizeof(Stats), &hoursOnZeroStats, 1);
	memcpy(data + sizeof(Stats) + sizeof(hoursOnZeroStats), &hatched, 1);

	if(!nvs.set(BlobName, data)){
		ESP_LOGW(tag, "Error saving stats to NVS");
	}
}

void StatsManager::load(){
	const auto size = sizeof(Stats) + sizeof(hoursOnZeroStats) + sizeof(hatched);
	uint8_t data[size];

	if(!nvs.get(BlobName, data)){
		ESP_LOGW(tag, "Stats data not found or corrupt! Setting defaults.");
		stats.happiness = 100;
		stats.oilLevel = 100;
		stats.experience = 0;
		hoursOnZeroStats = 0;
		hatched = false;
		return;
	}

	memcpy(&stats, data, sizeof(Stats));

	if(stats.happiness > 100 || stats.oilLevel > 100){

		ESP_LOGW(tag, "Stats file not found or corrupt! Setting defaults.");
		stats.happiness = 100;
		stats.oilLevel = 100;
		stats.experience = 0;
		hoursOnZeroStats = 0;
		hatched = false;
		return;
	}

	memcpy(&hoursOnZeroStats, data + sizeof(Stats), 1);
	memcpy(&hatched, data + sizeof(Stats) + sizeof(hoursOnZeroStats), 1);
}
