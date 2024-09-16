#ifndef BIT_FIRMWARE_SERVICES_H
#define BIT_FIRMWARE_SERVICES_H

#include <unordered_map>

enum class Service { Audio, Settings, Battery, Backlight, Input, RobotManager, UI, Display, Robots, NVS, XPSystem, HighScore, Achievements, LED, Twinkle };

class ServiceLocator {
public:
	void set(Service service, void* ptr);
	void* get(Service service);

private:
	std::unordered_map<Service, void*> services;


};

extern ServiceLocator Services;

#endif //BIT_FIRMWARE_SERVICES_H
