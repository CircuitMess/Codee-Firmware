#ifndef PERSE_ROVER_LEDSERVICE_H
#define PERSE_ROVER_LEDSERVICE_H

#include <cstdint>
#include <map>
#include <tuple>
#include <driver/ledc.h>
#include <memory>
#include <mutex>
#include "Util/Threaded.h"
#include "Util/Queue.h"

enum class LED : uint8_t {
	Red, COUNT
};

class LEDService : private Threaded {
public:
	explicit LEDService();

	virtual ~LEDService();

	void begin();
	void end();

	void on(LED led);

	void off(LED led);

	void blink(LED led, uint32_t count = 1, uint32_t period = 1000);

	void breathe(LED led, uint32_t period = 1000);

	void set(LED led, float percent);

	void breatheTo(LED led, float targetPercent, uint32_t duration = 250);

protected:
	virtual void loop() override;

private:
	struct PwnMappingInfo {
		gpio_num_t pin = GPIO_NUM_NC;
		ledc_channel_t channel = LEDC_CHANNEL_0;
		uint8_t limit = 100;
	};

	static const std::map<LED, PwnMappingInfo> PwmMappings;

private:
	enum LEDInstruction {
		On,
		Off,
		Blink,
		Breathe,
		Set,
		BreatheTo
	};

	struct LEDInstructionInfo {
		LED led;
		LEDInstruction instruction;
		uint32_t count;
		uint32_t period;
		float targetPercent;
	};

	std::map<LED, class SingleLED*> ledDevices;
	std::map<LED, std::unique_ptr<class LEDFunction>> ledFunctions;
	Queue<LEDInstructionInfo> instructionQueue;

private:
	void onInternal(LED led);

	void offInternal(LED led);

	void blinkInternal(LED led, uint32_t count, uint32_t period);

	void breatheInternal(LED led, uint32_t period);

	void setInternal(LED led, float percent);

	void breatheToInternal(LED led, float targetPercent, uint32_t duration);
};

#endif //PERSE_ROVER_LEDSERVICE_H