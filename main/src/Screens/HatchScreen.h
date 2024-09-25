#ifndef CODEE_FIRMWARE_HATCHSCREEN_H
#define CODEE_FIRMWARE_HATCHSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Util/Events.h"

class HatchScreen : public LVScreen {
public:
	HatchScreen();
	~HatchScreen() override;

private:
	void onStop() override;
	void loop() override;
	void onStart() override;

	EventQueue queue;
	lv_obj_t* gif;
	lv_obj_t* modal;
	lv_obj_t* label;

	bool exit = false;
	bool accepted = false;
};


#endif //CODEE_FIRMWARE_HATCHSCREEN_H
