#ifndef BIT_FIRMWARE_INTROSCREEN_H
#define BIT_FIRMWARE_INTROSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "LV_Interface/LVGIF.h"

class IntroScreen : public LVScreen {
public:
	IntroScreen();

private:
	LVGIF* gif;

	void onStart() override;
	void onStop() override;

};

#endif //BIT_FIRMWARE_INTROSCREEN_H
