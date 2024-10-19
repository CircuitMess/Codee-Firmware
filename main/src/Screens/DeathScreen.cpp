#include "DeathScreen.h"
#include "Services/StatsManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "HatchScreen.h"
#include "Util/stdafx.h"

DeathScreen::DeathScreen() : statsManager((StatsManager*) Services.get(Service::Stats)){
	const auto lvl = std::clamp(statsManager->getLevel()-1, 0, 5);
	lv_obj_set_style_bg_image_src(*this, BgPaths[lvl], 0);

	const auto stats = statsManager->get();

	character = new Character(*this, statsManager->getLevel(), stats.oilLevel < RustThreshold);
	lv_obj_add_flag(*character, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*character, LV_ALIGN_CENTER, 0, 10);
}

void DeathScreen::onStart(){
	startTime = millis();
}

void DeathScreen::loop(){
	if(phase == Start && millis() - startTime >= 1000){
		phase = Explo1;
		startTime = millis();

		auto gif = new LVGIF(*this, "S:/Anim/Death");
		gif->setLooping(LVGIF::LoopType::Single);
		gif->setLoopCallback([this, gif](){
			if(character){
				lv_obj_delete(*character);
				character = nullptr;
			}

			lv_obj_add_flag(*gif, LV_OBJ_FLAG_HIDDEN);
			phase = Stop;
			startTime = millis();
		});
		lv_obj_align(*gif, LV_ALIGN_CENTER, 0, 10);
		gif->start();
	}else if(phase == Explo1 && millis() - startTime >= 270){
		phase = Explo2;
		startTime = millis();

		if(character){
			lv_obj_delete(*character);
			character = nullptr;
		}
	}else if(phase == Stop && millis() - startTime >= 2000){
		phase = Done;
		startTime = millis();
		statsManager->reset();
		transition([](){ return std::make_unique<HatchScreen>(); });
	}
}
