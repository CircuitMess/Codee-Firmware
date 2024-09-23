#ifndef BIT_FIRMWARE_INTROSCREEN_H
#define BIT_FIRMWARE_INTROSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"

class IntroScreen : public LVScreen {
public:
	IntroScreen();

private:
	lv_obj_t* gif;

	void onStart() override;
	void onStop() override;

};

#endif //BIT_FIRMWARE_INTROSCREEN_H
