#ifndef CODEE_FIRMWARE_MENU_H
#define CODEE_FIRMWARE_MENU_H

#include "LV_Interface/LVObject.h"
#include "GrayscaleImageElement.h"
#include "GameEngine/Game.h"


struct MenuItem {
	uint8_t levelRequired;
	const char* unlockedPath;
	const char* lockedPath;
	Games game;
};

class Menu : public LVObject{
public:
	Menu(lv_obj_t* parent, lv_group_t* inputGroup);

private:
	void launch(Games game);

	std::vector<GrayscaleImageElement*> items;
	lv_obj_t* border;

	static constexpr MenuItem GameItems[(uint32_t)Games::COUNT] = {
			{ 1, "S:/MenuIcons/Icon1.bin", "S:/MenuIcons/Icon1.bin", Games::Oily},
			{ 2, "S:/MenuIcons/Icon2.bin","S:/MenuIcons/Locked2.bin", Games::PolarSwim },
			{ 3,"S:/MenuIcons/Icon3.bin","S:/MenuIcons/Locked3.bin",Games::PingoSnack},
			{ 4, "S:/MenuIcons/Icon4.bin", "S:/MenuIcons/Locked4.bin", Games::PolarJump },
			{ 5,"S:/MenuIcons/Icon5.bin", "S:/MenuIcons/Locked5.bin", Games::Dance },
			{ 6,"S:/MenuIcons/Icon6.bin", "S:/MenuIcons/Locked6.bin",Games::IceBlast},
	};
};


#endif //CODEE_FIRMWARE_MENU_H
