#include <esp_random.h>
#include "PetScreen.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/DeathScreen.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include "LV_Interface/InputLVGL.h"
#include "Screens/LevelUpScreen.h"

PetScreen::PetScreen() : statsManager((StatsManager*) Services.get(Service::Stats)), battery((Battery*) Services.get(Service::Battery)), queue(12){
	const auto lvl = std::clamp(statsManager->getLevel()-1, 0, 5);
	lv_obj_set_style_bg_image_src(*this, BgPaths[lvl], 0);

	const auto stats = statsManager->get();
	statsSprite = new StatsSprite(*this, stats.oilLevel, stats.happiness, statsManager->getLevel(), 100);
	lv_obj_add_flag(*statsSprite, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*statsSprite, LV_ALIGN_TOP_MID, 0, 0);

	xpSprite = new StatSprite(*this, StatSprite::XP, statsManager->getExpPercentage());
	lv_obj_add_flag(*xpSprite, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*xpSprite, LV_ALIGN_BOTTOM_LEFT, 2, -2);

	characterSprite = new Character(*this, statsManager->getLevel(), stats.oilLevel < rustThreshold);
	lv_obj_add_flag(*characterSprite, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*characterSprite, LV_ALIGN_CENTER, 0, 10);

	characterSprite->setRusty(stats.oilLevel < rustThreshold);
	characterSprite->setLevel(statsManager->getLevel());

	menu = new Menu(*this, [](uint8_t i){});
	lv_obj_add_flag(*menu, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*menu, LV_ALIGN_CENTER, 0, 30);

	lv_obj_update_layout(*this);
	menu->hideNow();
}

PetScreen::~PetScreen(){

}

void PetScreen::loop(){
	if(stopped) return;

	// Start level up before showing menu

	if(!startShown){
		startShown = true;
		menu->show();
	}

	menu->loop();

	if(dead){
		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<DeathScreen>(); });
	}

	statsSprite->setBattery(battery->getPerc());

	if(millis() - lastAlt >= altCooldown){
		lastAlt = millis();
		altCooldown = (esp_random()%8000) + 4000;
		characterSprite->playIdle();
	}
}

void PetScreen::onStart(){
	if(statsManager->hasDied()){
		dead = true;
		return;
	}

	statsChanged(statsManager->get(), false);
	lastAlt = millis();
	altCooldown = (esp_random()%8000) + 4000;

	Events::listen(Facility::Stats, &queue);
}

void PetScreen::onStop(){
	menu->stop();
	Events::unlisten(&queue);
}

void PetScreen::statsChanged(const Stats& stats, bool leveledUp){
	if(statsManager->hasDied()){
		dead = true;
		return;
	}else{
		dead = false;
	}

	if(leveledUp){
		menu->stop();
		stopped = true;
		const auto level = statsManager->getLevel();
		transition([level](){ return std::make_unique<LevelUpScreen>(level); }, LV_SCR_LOAD_ANIM_FADE_IN);
		return;
	}

	characterSprite->setRusty(stats.oilLevel < rustThreshold);

	statsSprite->setHappiness(stats.happiness);
	statsSprite->setOil(stats.oilLevel);
	statsSprite->setLevel(statsManager->getLevel());

	xpSprite->set(statsManager->getExpPercentage());
}
