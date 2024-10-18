#include "SplashScreen.h"
#include "UIThread.h"
#include "Util/Services.h"

SplashScreen::SplashScreen(Games game) : game(game), queue(1){
	lv_obj_set_style_bg_image_src(*this, GamesSplashImages[(uint8_t) game].splashPath, 0);
}

SplashScreen::~SplashScreen(){
	Events::unlisten(&queue);
	lv_timer_delete(timer);
}

void SplashScreen::onStart(){
	timer = lv_timer_create([](lv_timer_t* t){
		auto screen = (SplashScreen*) lv_timer_get_user_data(t);
		lv_obj_set_style_bg_image_src(*screen, GamesSplashImages[(uint8_t) screen->game].instructionsPath, 0);
		lv_timer_pause(t);
		Events::listen(Facility::Input, &screen->queue);
	}, SplashTime, this);
}

void SplashScreen::loop(){
	Event event{};
	if(!queue.get(event, 0)) return;

	free(event.data);

	if(auto ui = (UIThread*) Services.get(Service::UI)){
		ui->startGame(game);
	}
}
