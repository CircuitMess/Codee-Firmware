#include "DeathScreen.h"
#include "Services/StatsManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "HatchScreen.h"

DeathScreen::DeathScreen() : stats((StatsManager*) Services.get(Service::Stats)){
	sprintf(bgPath, "S:/Bg/Level%d.bin", stats->getLevel());
	lv_obj_set_style_bg_image_src(*this, bgPath, 0);

	duck = lv_gif_create(*this);
	lv_gif_set_src(duck, getPath().c_str());
	lv_gif_pause(duck); //only 1st frame of gif shown
	lv_obj_set_pos(duck, 50, 36);


	explosion = lv_gif_create(*this);
	lv_gif_set_src(explosion, "S:/death.gif");
	lv_gif_pause(explosion);


	lv_obj_add_event_cb(explosion, [](lv_event_t* e){
		auto screen = (DeathScreen*) lv_event_get_user_data(e);
		screen->exit = true;
		lv_timer_set_period(screen->timer, exitPause);
		lv_timer_reset(screen->timer);
		lv_timer_resume(screen->timer);
	}, LV_EVENT_READY, this);

}

void DeathScreen::onStart(){
	timer = lv_timer_create([](lv_timer_t* t){
		auto screen = (DeathScreen*) lv_timer_get_user_data(t);
		if(!screen->exploded){
			lv_gif_resume(screen->explosion);
			lv_timer_set_period(t, duckVisibleMillis);
			lv_timer_reset(t);
			screen->exploded = true;
		}else{
			if(screen->exit){
				auto ui = (UIThread*) Services.get(Service::UI);
				ui->startScreen([](){ return std::make_unique<HatchScreen>(); });
			}else{
				lv_obj_add_flag(screen->duck, LV_OBJ_FLAG_HIDDEN);
				lv_timer_pause(t);
			}
		}
	}, explosionStart, this);
}

void DeathScreen::onStop(){
	stats->setHatched(false);
}

DeathScreen::~DeathScreen(){
	lv_timer_delete(timer);
}

std::string DeathScreen::getPath(){
	std::string path = "S:/Home/rusty/0";
	if(stats->getLevel() == 6){
		path += "6_general.gif";
	}else if(stats->getLevel() >= 4){
		path += "4_general.gif";
	}else{
		path += "1_general.gif";
	}
	return path;
}
