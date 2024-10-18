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
	void stop();

	void hideNow();
	void show();

private:
	static constexpr glm::ivec2 ItemSize = { 32, 32 };
	static constexpr glm::ivec2 FrameSize = { 36, 36 };

	void hide();

	void click(int index);
	std::function<void(uint8_t)> launch;

	void shake();
	bool shaking = false;
	lv_anim_t shakeAnim;
	static constexpr uint32_t ShakeAnimDuration = 50;

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
