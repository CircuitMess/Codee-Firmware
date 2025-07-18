#include <esp_random.h>
#include "PetScreen.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/DeathScreen.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include "LV_Interface/InputLVGL.h"
#include "Screens/LevelUpScreen.h"
#include "Screens/Settings/SettingsScreen.h"

PetScreen::PetScreen() : evts(6), statsManager((StatsManager*) Services.get(Service::Stats)), battery((Battery*) Services.get(Service::Battery)){
	const auto lvl = std::clamp(statsManager->getLevel()-1, 0, 5);
	lv_obj_set_style_bg_image_src(*this, BgPaths[lvl], 0);

	const auto stats = statsManager->get();

	characterSprite = new Character(*this, statsManager->getLevel(), stats.oilLevel < RustThreshold);
	lv_obj_add_flag(*characterSprite, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*characterSprite, LV_ALIGN_CENTER, 0, 10);

	statsSprite = new StatsSprite(*this, stats.oilLevel, stats.happiness, battery->getPerc(), statsManager->getLevel());
	lv_obj_add_flag(*statsSprite, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*statsSprite, LV_ALIGN_TOP_MID, 0, 0);

	xpSprite = new StatSprite(*this, StatSprite::XP, statsManager->getExpPercentage());
	lv_obj_add_flag(*xpSprite, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*xpSprite, LV_ALIGN_BOTTOM_LEFT, 2, -2);

	menu = new Menu(*this, inputGroup, [this](uint8_t i){ launch(i); });
	lv_obj_add_flag(*menu, LV_OBJ_FLAG_FLOATING);
	lv_obj_align(*menu, LV_ALIGN_CENTER, 0, 30);

	lv_obj_update_layout(*this);
	menu->hideNow();
}

void PetScreen::onStart(){
	lastAlt = millis();
	altCooldown = (esp_random()%8000) + 4000;

	Events::listen(Facility::Stats, &evts);
	Events::listen(Facility::Input, &evts);
}

void PetScreen::onStop(){
	menu->stop();
	Events::unlisten(&evts);
}

void PetScreen::loop(){
	if(stopped) return;

	// Start level up before showing menu

	if(!startShown){
		startShown = true;
		menu->show();
	}

	const bool chrg = battery->isCharging();
	statsSprite->setCharging(chrg);
	if(chrg){
		const auto now = micros();
		static uint64_t lastMicros = 0;
		statsSprite->loop(now - lastMicros);
		lastMicros = now;
	}else{
		statsSprite->setBattery(battery->getPerc());
	}

	if(millis() - lastAlt >= altCooldown){
		lastAlt = millis();
		altCooldown = (esp_random()%8000) + 4000;
		characterSprite->playIdle();
	}

	if(backPress && millis() - backPress >= 4000){
		menu->stop();
		Events::unlisten(&evts);
		stopped = true;

		powerOff();
		return;
	}

	// Here transitions happen, so should be last
	processEvents();
}

void PetScreen::launch(uint8_t i){
	if(i == 0){
		menu->stop();
		Events::unlisten(&evts);
		stopped = true;

		transition([](){ return std::make_unique<SettingsScreen>(); });
		return;
	}

	const auto lvl = statsManager->getLevel();
	if(lvl < i) return;

	menu->stop();
	Events::unlisten(&evts);
	stopped = true;

	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startGame((Games) (i-1));
}

void PetScreen::processEvents(){
	Event evt{};
	if(!evts.get(evt, 0)) return;

	if(evt.facility == Facility::Input){
		const auto data = (Input::Data*) evt.data;
		if(data->btn != Input::D){
			free(evt.data);
			return;
		}

		if(data->action == Input::Data::Press){
			backPress = millis();
		}else{
			backPress = 0;
		}

		free(evt.data);
		return;
	}

	if(evt.facility != Facility::Stats){
		free(evt.data);
		return;
	}

	const auto* data = (StatsManager::Event*) evt.data;
	if(data->action != StatsManager::Event::Updated){
		free(evt.data);
		return;
	}

	statsChanged(statsManager->get(), data->levelup);
}

void PetScreen::statsChanged(const Stats& stats, bool leveledUp){
	if(statsManager->hasDied()){
		menu->stop();
		Events::unlisten(&evts);
		stopped = true;

		transition([](){ return std::make_unique<DeathScreen>(); });

		return;
	}

	if(leveledUp){
		menu->stop();
		Events::unlisten(&evts);
		stopped = true;

		const auto level = statsManager->getLevel();
		transition([level](){ return std::make_unique<LevelUpScreen>(level); }, LV_SCR_LOAD_ANIM_FADE_IN);

		return;
	}

	characterSprite->setRusty(stats.oilLevel < RustThreshold);

	statsSprite->setHappiness(stats.happiness);
	statsSprite->setOil(stats.oilLevel);

	xpSprite->set(statsManager->getExpPercentage());
}

void PetScreen::powerOff(){
	auto bl = (BacklightBrightness*) Services.get(Service::Backlight);
	bl->fadeOut();

	auto input = (Input*) Services.get(Service::Input);
	const auto state = input->getState();

	uint8_t pressed = 0;
	for(const auto& pair : state){
		if(pair.second){
			pressed |= 1 << pair.first;
		}
	}

	Events::unlisten(&evts);
	evts.reset();
	Events::listen(Facility::Input, &evts);

	while(pressed){
		Event evt{};
		if(!evts.get(evt, portMAX_DELAY)) continue;

		if(evt.facility != Facility::Input){
			free(evt.data);
			continue;
		}

		const auto data = (Input::Data*) evt.data;
		if(data->action == Input::Data::Press){
			pressed |= 1 << data->btn;
		}else{
			pressed &= ~(1 << data->btn);
		}

		free(evt.data);
	}

	Events::unlisten(&evts);

	Power::sleepDeep();
}
