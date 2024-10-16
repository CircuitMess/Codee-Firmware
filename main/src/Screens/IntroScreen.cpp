#include "IntroScreen.h"
#include "Util/Services.h"
#include "Services/StatsManager.h"
#include "UIThread.h"
#include "DeathScreen.h"
#include "HatchScreen.h"
#include "PetScreen/PetScreen.h"

IntroScreen::IntroScreen(){
	lv_obj_set_size(*this, 128, 128);

	gif = lv_gif_create(*this);
	lv_gif_set_src(gif, "S:/intro.gif");
	lv_obj_set_pos(gif, 0, 0);

	lv_gif_pause(gif);

	lv_obj_add_event_cb(gif, [](lv_event_t* e){
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
	}, LV_EVENT_READY, this);

}

void IntroScreen::onStart(){
	lv_gif_restart(gif);
	lv_gif_resume(gif);
}

void IntroScreen::onStop(){
	lv_gif_pause(gif);
}
