#include "ShutdownScreen.h"
#include "Util/stdafx.h"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Services/Power.h"

ShutdownScreen::ShutdownScreen(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	auto img = lv_img_create(*this);
	lv_img_set_src(img, "S:/bg.bin");
	lv_obj_add_flag(img, LV_OBJ_FLAG_FLOATING);

	img = lv_img_create(*this);
	lv_img_set_src(img, "S:/Battery/EmptyBig.bin");
	lv_obj_align(img, LV_ALIGN_CENTER, 0, -10);

	label = lv_label_create(*this);
	lv_label_set_text(label, "Battery critical,\nshutting down");
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);
}

void ShutdownScreen::onStart(){
	blinkTime = millis();
	startTime = millis();
}

void ShutdownScreen::loop(){
	if(millis() - startTime >= ShutdownTime){
		off();
	}
}

void ShutdownScreen::off(){
	Power::powerOff();
}
