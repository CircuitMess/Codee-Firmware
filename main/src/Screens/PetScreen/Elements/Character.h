#ifndef CIRCUITPET_FIRMWARE_CHARACTERSPRITE_H
#define CIRCUITPET_FIRMWARE_CHARACTERSPRITE_H

#include "LV_Interface/LVObject.h"
#include <optional>
#include <string>

enum class Anim : uint8_t {
	General, Scratch, LookAround, Count // Stretch, Wave, Dance, Knock, TODO - add other anims when available
};

struct CharacterAnim {
	uint8_t charLevel;
	bool rusty;
	Anim anim;
};

class Character : public LVObject {
public:
	Character(lv_obj_t* parentSprite, uint8_t charLevel, bool rusty, Anim currentAnim);
	void setCharLevel(uint8_t charLevel);
	void setRusty(bool rusty);

	void setAnim(Anim anim);

private:
	lv_obj_t* gif;

	uint8_t charLevel; //0-3
	bool rusty; //0-1
	Anim currentAnim;

	std::optional<CharacterAnim> nextAnim;

	void registerNextAnim(); //queues up nextAnim to be started when current anim ends
	void startNextAnim(); //starts queued nextAnim, then clears it
	std::string getAnimPath(uint8_t charLevel, bool rustLevel, Anim anim);

	uint8_t getGIFLevel(uint8_t level);

	int16_t x = 0, y = 0;

	bool canChange = false;
};


#endif //CIRCUITPET_FIRMWARE_CHARACTERSPRITE_H
