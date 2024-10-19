#include <algorithm>
#include "LevelUpScreen.h"
#include "Util/stdafx.h"
#include "Screens/PetScreen/PetScreen.h"

LevelUpScreen::LevelUpScreen(uint8_t toLevel){
	toLevel = std::clamp((int) toLevel, 2, 6)-2;
	lv_obj_set_style_bg_image_src(*this, Paths[toLevel], 0);
	lv_obj_set_style_bg_image_opa(*this, LV_OPA_COVER, 0);
}

void LevelUpScreen::onStart(){
	startTime = millis();
}

void LevelUpScreen::loop(){
	if(startTime != 0 && millis() - startTime >= Timeout){
		startTime = 0;
		transition([](){ return std::make_unique<PetScreen>(); }, LV_SCR_LOAD_ANIM_FADE_OUT);
	}
}
