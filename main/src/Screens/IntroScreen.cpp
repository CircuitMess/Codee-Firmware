#include "IntroScreen.h"
#include "Util/Services.h"
#include "Services/StatsManager.h"
#include "UIThread.h"
#include "DeathScreen.h"
#include "HatchScreen.h"
#include "PetScreen/PetScreen.h"

IntroScreen::IntroScreen(){
	lv_obj_set_size(*this, 128, 128);

	gif = new LVGIF(*this, "S:/Anim/Intro");
	gif->setLooping(LVGIF::LoopType::Single);
	lv_obj_set_pos(*gif, 0, 0);

	gif->setLoopCallback([this](){
		lv_obj_add_flag(*gif, LV_OBJ_FLAG_HIDDEN);

		auto statsMan = (StatsManager*)Services.get(Service::Stats);
		if(statsMan->isHatched()){
			if(statsMan->hasDied()){
				transition([](){ return std::make_unique<DeathScreen>(); }, LV_SCR_LOAD_ANIM_FADE_IN);
			}else{
				transition([](){ return std::make_unique<PetScreen>(); }, LV_SCR_LOAD_ANIM_FADE_IN);
			}
		}else{
			transition([](){ return std::make_unique<HatchScreen>(); }, LV_SCR_LOAD_ANIM_FADE_IN);
		}
	});
}

void IntroScreen::onStart(){
	gif->start();
}

void IntroScreen::onStop(){
	gif->stop();
}
