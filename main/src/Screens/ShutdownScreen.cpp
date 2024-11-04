#include "ShutdownScreen.h"
#include "Util/stdafx.h"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Services/Power.h"
#include "Services/StatsManager.h"
#include "Services/BacklightBrightness.h"

ShutdownScreen::ShutdownScreen(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	auto stats = (StatsManager*) Services.get(Service::Stats);
	const auto lvl = std::clamp((int) stats->getLevel() - 1, 0, 5);
	lv_obj_set_style_bg_image_src(*this, BgPaths[lvl], 0);
	lv_obj_set_style_bg_image_opa(*this, LV_OPA_COVER, 0);

	auto modal = lv_image_create(*this);
	lv_image_set_src(modal, "S:/Modal.bin");
	lv_obj_add_flag(modal, LV_OBJ_FLAG_FLOATING);
	lv_obj_center(modal);

	auto label = lv_label_create(*this);
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_obj_set_style_text_line_space(label, 8, 0);
	lv_label_set_text(label, "Battery critical,\nshutting down.");
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_add_flag(label, LV_OBJ_FLAG_FLOATING);
	lv_obj_center(label);
}

void ShutdownScreen::onStart(){
	startTime = millis();
}

void ShutdownScreen::loop(){
	if(millis() - startTime >= ShutdownTime){
		off();
	}
}

void ShutdownScreen::off(){
	auto bl = (BacklightBrightness*) Services.get(Service::Backlight);
	bl->fadeOut();
	Power::powerOff();
}
