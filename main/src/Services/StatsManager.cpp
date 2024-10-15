#include <cstring>
#include "StatsManager.h"
#include "Util/Services.h"
#include "Services/Time.h"

static const char* tag = "StatsManager";

StatsManager::StatsManager() : Threaded("Stats", 4 * 1024, 5), nvs(*(NVSFlash*) Services.get(Service::NVS)), timeService((Time*) Services.get(Service::Time)),
							   queue(6){
	load();

	//load saved time
	time_t savedTime;
	if(nvs.get(TimeSaveBlobName, savedTime)){
		lastUpdate = *localtime(&savedTime);
	}else{
		lastUpdate = timeService->getTime();
	}
}

void StatsManager::reset(){
	stats.happiness = 100;
	stats.oilLevel = 100;
	stats.experience = 0;
	hoursOnZeroStats = 0;
	hatched = false;
	store();
}

void StatsManager::update(Stats delta){
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

bool StatsManager::hasDied() const{
	return hoursOnZeroStats > GameOverHours; //dies after 24hrs of zero happiness
}


const Stats& StatsManager::get() const{
	return stats;
}

uint8_t StatsManager::getLevel() const{
	return getLevel(stats.experience);
}

uint8_t StatsManager::getLevel(uint16_t exp) {
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

	return (uint8_t) ((float) (exp - prevThreshold) * 100.0 / (LevelupThresholds[getLevel(exp) - 1]));
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

/*	if(!data || !size || size != sizeof(Stats) + sizeof(hoursOnZeroStats) + sizeof(hatched)){
		ESP_LOGW(tag, "Clock data read failed - data invalid");
		ESP_LOGW(tag, "Stats data not found or corrupt! Setting defaults.");
		stats.happiness = 100;
		stats.oilLevel = 100;
		stats.experience = 0;
		hoursOnZeroStats = 0;
		hatched = false;
		return;
	}*/
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

void StatsManager::loop(){
	::Event event{};
	std::tm currentTime{};

	//hourly updated or after RTC update occurs
	if(queue.get(event, 3600000 / portTICK_PERIOD_MS)){
		auto timeEvent = (Time::Event*) event.data;
		if(timeEvent->action == Time::Event::Updated){
			currentTime = timeEvent->updated.time;
		}
		free(event.data);
	}else{
		currentTime = timeService->getTime();
	}

	//if time diff is greater than 1 hour, call timedUpdate
	if(difftime(mktime(&currentTime), mktime(&lastUpdate)) >= 3600){
		timedUpdate();
		lastUpdate = currentTime;
	}

	//save currentTime in case of shutdown/sleep, so it can be resumed
	auto unixtime = mktime(&currentTime);
	if(!nvs.set(TimeSaveBlobName, &unixtime)){
		ESP_LOGW(tag, "Error saving time to NVS");
	}
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

bool StatsManager::isHatched() const{
	return hatched;
}

void StatsManager::setHatched(bool hatched){
	StatsManager::hatched = hatched;
	store();
}
