#include "LVGL.h"
#include "InputLVGL.h"
#include "Theme/theme.h"
#include <esp_log.h>
#include <core/lv_global.h>
#include <draw/lv_image_decoder.h>
#include <draw/lv_image_decoder_private.h>
#include <draw/sw/blend/lv_draw_sw_blend_to_rgb565.h>
#include <draw/sw/blend/lv_draw_sw_blend_private.h>

LVGL::LVGL(Display& display) : display(display){
	lv_init();

	lv_tick_set_cb(xTaskGetTickCount);

	lvDisplay = lv_display_create(128, 128);
	lv_display_set_color_format(lvDisplay, LV_COLOR_FORMAT_RGB565);
	lv_display_set_user_data(lvDisplay, this);
	lv_display_set_flush_cb(lvDisplay, flush);

	Sprite& canvas = display.getCanvas();
	lv_display_set_buffers(lvDisplay, canvas.getBuffer(), nullptr, canvas.width()*canvas.height()*2, LV_DISPLAY_RENDER_MODE_PARTIAL);

	auto theme = theme_init(lvDisplay);
	lv_display_set_theme(lvDisplay, theme);
}

void LVGL::flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map){
	auto lvgl = static_cast<LVGL*>(lv_display_get_user_data(disp));
	auto lgfx = lvgl->display.getLGFX();

	auto x = area->x1;
	auto y = area->y1;
	auto w = (area->x2 - area->x1 + 1);
	auto h = (area->y2 - area->y1 + 1);
	auto data = (uint16_t*) px_map;

	lgfx.pushImage(x, y, w, h, data);

	lv_display_flush_ready(disp);
}

void LVGL::loop(){
	if(currentScreen && currentScreen->loaded){
		currentScreen->loop();
	}
	if(!currentScreen) return;

	auto ttn = lv_timer_handler();

	if(ttn <= 0 || ttn > LV_DEF_REFR_PERIOD) ttn = 1;
	vTaskDelay(ttn);
}

void LVGL::resume(){
	if(currentScreen){
		lv_obj_invalidate(*currentScreen);
	}
}

void LVGL::startScreen(std::function<std::unique_ptr<LVScreen>()> create, lv_screen_load_anim_t anim){
	if(anim == LV_SCR_LOAD_ANIM_NONE){
		stopScreen();
	}else{
		currentScreen->stop();
		currentScreen.release(); // Will get auto deleted by LVGL
	}

	currentScreen = create();
	currentScreen->start(this);
	lv_screen_load_anim(*currentScreen, anim, anim == LV_SCR_LOAD_ANIM_NONE ? 0 : 500, 0, true);
}

void LVGL::stopScreen(){
	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), nullptr);

	if(currentScreen){
		currentScreen->stop();

		// and switch to a temp screen. if currentScreen isn't set, that means a temp screen is already running
		lv_obj_t* tmp = lv_obj_create(nullptr);
		lv_scr_load_anim(tmp, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
	}

	currentScreen.reset();
}

void LVGL::drawImage(const char* src){
	const auto lvDisplay = LV_GLOBAL_DEFAULT()->disp_default;
	const auto lvgl = (LVGL*) lv_display_get_user_data(lvDisplay);
	const Sprite& canvas = lvgl->display.getCanvas();

	lv_image_decoder_dsc_t decoder;
	const lv_image_decoder_args_t args = { .no_cache = true };
	lv_result_t res = lv_image_decoder_open(&decoder, src, &args);
	if(res != LV_RESULT_OK){
		ESP_LOGE("LVGL::drawImage", "Failed opening image file %s", src);
		return;
	}

	const auto render = [&decoder, &canvas](const lv_area_t& area){
		lv_draw_sw_blend_image_dsc_t dsc = {
				.dest_buf = (uint8_t*) canvas.getBuffer() + canvas.width() * 2 * area.y1,
				.dest_w = area.x2 - area.x1 + 1,
				.dest_h = area.y2 - area.y1 + 1,
				.dest_stride = canvas.width() * 2,
				.src_buf = decoder.decoded->data,
				.src_stride = (int32_t) decoder.decoded->header.stride,
				.src_color_format = (lv_color_format_t) decoder.decoded->header.cf,
				.opa = LV_OPA_COVER,
				.blend_mode = LV_BLEND_MODE_NORMAL
		};
		lv_draw_sw_blend_image_to_rgb565(&dsc);
	};

	const lv_area_t area_full = { 0, 0, canvas.width() - 1, canvas.height() - 1 };

	if(decoder.decoded){
		render(area_full);
	}else{
		lv_area_t area_decoded = { LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MIN };
		while(res == LV_RESULT_OK){
			res = lv_image_decoder_get_area(&decoder, &area_full, &area_decoded);
			if(res == LV_RESULT_OK){
				render(area_decoded);
			}
		}
	}

	lv_image_decoder_close(&decoder);

	lvgl->flush(lvDisplay, &area_full, (uint8_t*) canvas.getBuffer());
}

void LVGL::resetDisplayRefreshTimer(){
	auto timer = lv_display_get_refr_timer(lvDisplay);
	lv_timer_reset(timer);
}
