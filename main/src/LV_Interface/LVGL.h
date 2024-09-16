#ifndef CLOCKSTAR_FIRMWARE_LVGL_H
#define CLOCKSTAR_FIRMWARE_LVGL_H

#include <lvgl.h>
#include "LVScreen.h"
#include "Devices/Display.h"
#include "Util/Threaded.h"

class LVGL{
public:
	LVGL(Display& display);

	void startScreen(std::function<std::unique_ptr<LVScreen>()> create);

	/** startScreen should be called immediately after this function. */
	void stopScreen();

	void loop();
	void resume();

private:
	Display& display;

	static constexpr uint8_t Rows = 64;
	uint8_t drawBuffer[2*128*Rows];

	lv_disp_draw_buf_t lvDrawBuf;
	lv_disp_drv_t lvDispDrv;
	lv_disp_t* lvDisplay;

	static void flush(lv_disp_drv_t* dispDrv, const lv_area_t* area, lv_color_t* pixels);

	std::unique_ptr<LVScreen> currentScreen;

};


#endif //CLOCKSTAR_FIRMWARE_LVGL_H
