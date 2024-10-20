#ifndef CLOCKSTAR_FIRMWARE_LVGL_H
#define CLOCKSTAR_FIRMWARE_LVGL_H

#include <lvgl.h>
#include "LVScreen.h"
#include "Devices/Display.h"
#include "Util/Threaded.h"

class LVGL{
public:
	LVGL(Display& display);

	void startScreen(std::function<std::unique_ptr<LVScreen>()> create, lv_screen_load_anim_t anim = LV_SCR_LOAD_ANIM_NONE);

	/** startScreen should be called immediately after this function. */
	void stopScreen();

	void loop();
	void resume();

private:
	Display& display;

	lv_disp_t* lvDisplay;

	static void flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

	std::unique_ptr<LVScreen> currentScreen;

};


#endif //CLOCKSTAR_FIRMWARE_LVGL_H
