#ifndef CODEE_FIRMWARE_LEVELUPSCREEN_H
#define CODEE_FIRMWARE_LEVELUPSCREEN_H

#include "LV_Interface/LVScreen.h"

class LevelUpScreen : public LVScreen {
public:
	explicit LevelUpScreen(uint8_t toLevel);

private:
	uint32_t startTime = 0;
	static constexpr uint32_t Timeout = 2000;

	void onStart() override;
	void loop() override;

	static constexpr const char* Paths[] = {
			"S:/LevelUp/Level2.bin",
			"S:/LevelUp/Level3.bin",
			"S:/LevelUp/Level4.bin",
			"S:/LevelUp/Level5.bin",
			"S:/LevelUp/Level6.bin"
	};

};


#endif //CODEE_FIRMWARE_LEVELUPSCREEN_H
