#include "IntroScreen.h"
#include "Util/Services.h"
#include "Services/StatsManager.h"

IntroScreen::IntroScreen(){
	lv_obj_set_size(*this, 128, 128);

	gif = lv_gif_create(*this);
	lv_gif_set_src(gif, "S:/intro.gif");
	lv_obj_set_pos(gif, 0, 0);

	lv_gif_pause(gif);

	lv_obj_add_event_cb(gif, [](lv_event_t* e){
		//TODO - transition to death/hatch/duckscreen

		/*	auto statsMan = (StatsManager*)Services.get(Service::Stats);
		if(statsMan->isHatched()){
			if(statsMan->hasDied()){
				auto duck = new DeathState(temp);
				duck->start();
			}else{
				auto duck = new DuckScreen(temp);
				duck->start();
			}
		}else{
			auto hatch = new HatchingState(temp);
			hatch->start();
		}*/
	}, LV_EVENT_READY, this);

}

void IntroScreen::onStart(){
	lv_gif_restart(gif);
	lv_gif_resume(gif);
}

void IntroScreen::onStop(){
	lv_gif_pause(gif);
}
