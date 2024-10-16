#include <esp_random.h>
#include "PetScreen.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/DeathScreen.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include "LV_Interface/InputLVGL.h"

PetScreen::PetScreen() : statsManager((StatsManager*) Services.get(Service::Stats)), battery((Battery*) Services.get(Service::Battery)), queue(12){
	sprintf(bgPath, "S:/Bg/Level%d.bin", statsManager->getLevel());
	lv_obj_set_style_bg_image_src(*this, bgPath, 0);

	lv_obj_set_style_pad_all(*this, 1, 0);

	topBar = lv_obj_create(*this);
	lv_obj_set_layout(topBar, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(topBar, LV_FLEX_FLOW_ROW_WRAP);
	lv_obj_set_flex_align(topBar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
	lv_obj_set_style_pad_row(topBar, 2, 0);

	statsSprite = new StatsSprite(topBar, statsManager->get().oilLevel, statsManager->get().happiness, 100);
	osSprite = new OSIcon(topBar, statsManager->getLevel());

	lv_obj_set_size(topBar, lv_pct(100), LV_SIZE_CONTENT);


	characterSprite = new Character(*this, statsManager->getLevel(), statsManager->get().oilLevel < rustThreshold, Anim::General);
	lv_obj_set_pos(*characterSprite, characterX, characterY);

	characterSprite->setRusty(statsManager->get().oilLevel < rustThreshold);
	characterSprite->setCharLevel(statsManager->getLevel());
	characterSprite->setAnim(Anim::General);

	menu = new Menu(*this, inputGroup);
	lv_obj_align(*menu, LV_ALIGN_BOTTOM_MID, 0, 50);

	hideGroup = lv_group_create();
	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), hideGroup);

	//used for switching inputGroups after menu hiding is over
	lv_obj_t* unhideElement = lv_obj_create(*this);
	lv_group_set_editing(hideGroup, true);
	lv_obj_add_flag(unhideElement, LV_OBJ_FLAG_FLOATING);
	lv_group_add_obj(hideGroup, unhideElement);
	lv_obj_add_event_cb(unhideElement, [](lv_event_t* e){
		auto screen = (PetScreen*) lv_event_get_user_data(e);
		lv_indev_set_group(InputLVGL::getInstance()->getIndev(), screen->inputGroup);
	}, LV_EVENT_KEY, this);

	levelupGroup = lv_group_create();
}

PetScreen::~PetScreen(){
	lv_obj_t* menuObj = *menu;
	lv_anim_delete(menuObj, nullptr);

	lv_group_delete(hideGroup);
	lv_group_delete(levelupGroup);
}

void PetScreen::loop(){
	if(levelupInProgress) return;

	if(dead){
		auto ui = (UIThread*) Services.get(Service::UI);
		ui->startScreen([](){ return std::make_unique<DeathScreen>(); });
	}

	Event event{};
	if(queue.get(event, 0)){
		if(event.facility == Facility::Stats){
			auto e = (StatsManager::Event*) event.data;
			statsChanged(statsManager->get(), e->levelup);
		}

		if(event.facility == Facility::Input){
			if(menuHidden){
				unhideMenu();
			}else{
				hideMillis = millis();
			}
		}

		free(event.data);
	}

	if(!menuHidden && millis() - hideMillis >= HideTimeout){
		hideMenu();
	}

	statsSprite->setBattery(battery->getPerc());

	//playing random duck animations while idling
	if(micros() - randCounter >= randInterval){
		randCounter = micros();
		Anim anim;
		if(!specialAnimPlaying){
			specialAnimPlaying = true;
			randInterval = 1000000;
			int num = 1 + esp_random() % ((uint8_t) Anim::Count - 1);
			anim = (Anim) (num);
		}else{
			specialAnimPlaying = false;
			randInterval = esp_random() % 4000000 + 2000000;
			anim = Anim::General;
		}

		characterSprite->setAnim(anim);
	}
}

void PetScreen::onStart(){
	if(statsManager->hasDied()){
		dead = true;
		return;
	}

	statsChanged(statsManager->get(), false);
	randInterval = esp_random() % 4000000 + 2000000;
	randCounter = micros();

	Events::listen(Facility::Stats, &queue);
	Events::listen(Facility::Input, &queue);

	if(!levelupInProgress){
		unhideMenu();
	}
}

void PetScreen::onStop(){
	Events::unlisten(&queue);
}

void PetScreen::statsChanged(const Stats& stats, bool leveledUp){
	if(statsManager->hasDied()){
		dead = true;
		return;
	}else{
		dead = false;
	}

	if(leveledUp && !levelupInProgress){
		startLevelupAnim();
	}

	if(characterSprite) characterSprite->setRusty(stats.oilLevel < rustThreshold);

	statsSprite->setHappiness(stats.happiness);
	statsSprite->setOilLevel(stats.oilLevel);
	statsSprite->setXPLevel();
}

void PetScreen::hideMenu(){
	if(menuHidden) return;
	menuHidden = true;

	lv_obj_t* menuObj = *menu;
	lv_anim_init(&hiderAnimation);
	lv_anim_set_var(&hiderAnimation, menuObj);
	lv_anim_set_values(&hiderAnimation, MenuY, 50);
	lv_anim_set_duration(&hiderAnimation, HideAnimDuration);
	lv_anim_set_exec_cb(&hiderAnimation, [](void* var, int32_t v){ lv_obj_set_y((lv_obj_t*) var, v); });
	lv_anim_set_path_cb(&hiderAnimation, lv_anim_path_ease_in);
	lv_anim_start(&hiderAnimation);

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), hideGroup);
}

void PetScreen::unhideMenu(){
	if(!menuHidden) return;

	hideMillis = millis();
	menuHidden = false;

	lv_obj_t* menuObj = *menu;
	lv_anim_init(&hiderAnimation);
	lv_anim_set_var(&hiderAnimation, menuObj);
	lv_anim_set_values(&hiderAnimation, 50, MenuY);
	lv_anim_set_duration(&hiderAnimation, HideAnimDuration);
	lv_anim_set_exec_cb(&hiderAnimation, [](void* var, int32_t v){ lv_obj_set_y((lv_obj_t*) var, v); });
	lv_anim_set_path_cb(&hiderAnimation, lv_anim_path_ease_out);
	lv_anim_start(&hiderAnimation);
}

void PetScreen::startLevelupAnim(){
	levelupImg = lv_image_create(*this);
	lv_obj_add_flag(levelupImg, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_style_opa(levelupImg, LV_OPA_TRANSP, 0);
	std::string path = "S:/LevelUp/" + std::to_string(statsManager->getLevel()) + ".bin";
	lv_image_set_src(levelupImg, path.c_str());


	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), levelupGroup);
	levelupInProgress = true;

	lv_anim_init(&levelupAnim);
	lv_anim_set_var(&levelupAnim, levelupImg);
	lv_anim_set_user_data(&levelupAnim, this);
	lv_anim_set_values(&levelupAnim, LV_OPA_TRANSP, LV_OPA_COVER);
	lv_anim_set_duration(&levelupAnim, LevelupFadeTime);
	lv_anim_set_exec_cb(&levelupAnim, [](void* var, int32_t v){ lv_obj_set_style_opa((lv_obj_t*) var, v, 0); });
	lv_anim_set_path_cb(&levelupAnim, lv_anim_path_linear);

	lv_anim_set_completed_cb(&levelupAnim, [](lv_anim_t* anim){
		auto screen = (PetScreen*) lv_anim_get_user_data(anim);
		screen->levelupTimer = lv_timer_create([](lv_timer_t* t){
			auto screen = (PetScreen*) lv_timer_get_user_data(t);
			screen->stopLevelupAnim();
		}, LevelupShowTime, screen);
	});
	lv_anim_start(&levelupAnim);
}

void PetScreen::stopLevelupAnim(){
	if(!levelupInProgress) return;

	lv_timer_delete(levelupTimer);

	lv_anim_init(&levelupAnim);
	lv_anim_set_var(&levelupAnim, levelupImg);
	lv_anim_set_user_data(&levelupAnim, this);
	lv_anim_set_values(&levelupAnim, LV_OPA_COVER, LV_OPA_TRANSP);
	lv_anim_set_duration(&levelupAnim, LevelupFadeTime);
	lv_anim_set_exec_cb(&levelupAnim, [](void* var, int32_t v){ lv_obj_set_style_opa((lv_obj_t*) var, v, 0); });
	lv_anim_set_path_cb(&levelupAnim, lv_anim_path_linear);

	lv_anim_set_completed_cb(&levelupAnim, [](lv_anim_t* anim){
		auto screen = (PetScreen*) lv_anim_get_user_data(anim);
		lv_obj_delete(screen->levelupImg);
		screen->levelupImg = nullptr;
		screen->levelupInProgress = false;
		if(screen->menuHidden){
			screen->unhideMenu();
		}else{
			lv_indev_set_group(InputLVGL::getInstance()->getIndev(), screen->inputGroup);
		}
	});
	lv_anim_start(&levelupAnim);
}
