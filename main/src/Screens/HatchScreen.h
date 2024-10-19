#ifndef CODEE_FIRMWARE_HATCHSCREEN_H
#define CODEE_FIRMWARE_HATCHSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Util/Events.h"
#include "LV_Interface/LVGIF.h"

class HatchScreen : public LVScreen {
public:
	HatchScreen();

private:
	void onStop() override;
	void loop() override;
	void onStart() override;

	EventQueue queue;
	LVGIF* gif;
	lv_obj_t* modal;
	lv_obj_t* label;

};


#endif //CODEE_FIRMWARE_HATCHSCREEN_H
