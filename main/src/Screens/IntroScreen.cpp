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
	lv_obj_set_pos(*gif, 0, 0);

	gif->setLoopCallback([](){
		auto statsMan = (StatsManager*)Services.get(Service::Stats);
		auto ui = (UIThread*)Services.get(Service::UI);

		if(statsMan->isHatched()){
			if(statsMan->hasDied()){
				ui->startScreen([](){ return std::make_unique<DeathScreen>(); });
			}else{
				ui->startScreen([](){ return std::make_unique<PetScreen>(); });
			}
		}else{
			ui->startScreen([](){ return std::make_unique<HatchScreen>(); });
		}
	});
}

void IntroScreen::onStart(){
	gif->start();
}

void IntroScreen::onStop(){
	gif->stop();
}
