#ifndef CODEE_FIRMWARE_PINS_H
#define CODEE_FIRMWARE_PINS_H

#include <cstdint>
#include <unordered_map>
#include <vector>

enum class Pin : uint8_t {
	BtnA, BtnB, BtnC, BtnD,
	LedBl, LedRed,
	Buzz, BattRead, VrefEn,
	I2cSda, I2cScl,
	TftSck, TftMosi, TftDc, TftRst
};

class Pins {
	typedef std::unordered_map<Pin, int> PinMap;
public:

	static int8_t get(Pin pin);

	static void setLatest();

private:
	Pins();

	PinMap* currentMap = nullptr;

	inline static Pins* instance = nullptr;

	void initPinMaps();

	//For original Bit, Bit 2
	PinMap Revision1;

	//For Bit v3
	PinMap Revision2;

	std::vector<PinMap*> pinMaps = { &Revision1, &Revision2 };
};


#endif //CODEE_FIRMWARE_PINS_H
