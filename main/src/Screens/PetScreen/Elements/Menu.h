#ifndef CODEE_FIRMWARE_MENU_H
#define CODEE_FIRMWARE_MENU_H


#include "LV_Interface/LVObject.h"
#include "ext/vector_int2.hpp"
#include "misc/lv_timer_private.h"
#include "Util/Events.h"

class Menu : public LVObject {
public:
	Menu(lv_obj_t* parent, std::function<void(uint8_t)> launch);
	~Menu() override;

	void loop();

private:
	static constexpr glm::ivec2 ItemSize = { 32, 32 };

	lv_obj_t* container;

	void hide();
	void show();

	std::function<void(uint8_t)> launch;

	EventQueue evts;
	lv_group_t* grp;

	static constexpr uint32_t HideTimeout = 3000;
	lv_timer_t* hideTimer;
	bool hidden = false;
	bool hiding = false;

	int32_t startPos;
	lv_anim_t hideAnim;

};


#endif //CODEE_FIRMWARE_MENU_H
