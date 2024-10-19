#include "SettingsScreen.h"
#include "Devices/Input.h"
#include "Util/Services.h"
#include "Screens/PetScreen/PetScreen.h"
#include "BoolElement.h"
#include "SliderElement.h"
#include "LabelElement.h"
#include "DiscreteSliderElement.h"
#include "LV_Interface/LVGL.h"
#include "Services/Time.h"
#include "LV_Interface/InputLVGL.h"
#include "LV_Interface/FSLVGL.h"
#include "Modals/Prompt.h"
#include "nvs_flash.h"
#include "Util/stdafx.h"

SettingsScreen::SettingsScreen() : settings(*(Settings*) Services.get(Service::Settings)), backlight(*(BacklightBrightness*) Services.get(Service::Backlight)),
								   audio(*(ChirpSystem*) Services.get(Service::Audio)), queue(4){
	buildUI();
}

void SettingsScreen::loop(){
	Event evt{};
	if(queue.get(evt, 0)){
		if(evt.facility == Facility::Input){
			auto eventData = (Input::Data*) evt.data;
			if(eventData->btn == Input::D && eventData->action == Input::Data::Press && lv_indev_get_group(InputLVGL::getInstance()->getIndev()) == inputGroup){
				free(evt.data);
				transition([](){ return std::make_unique<PetScreen>(); });
				return;
			}
		}

		free(evt.data);
	}
}

void SettingsScreen::onStop(){
	auto savedSettings = settings.get();
	savedSettings.sound = audioSwitch->getValue();
	savedSettings.screenBrightness = brightnessSlider->getValue();
	savedSettings.sleepTime = sleepSlider->getValue();
	settings.set(savedSettings);
	settings.store();

	backlight.setBrightness(brightnessSlider->getValue());

	Events::unlisten(&queue);
}

void SettingsScreen::onStarting(){
	auto sets = settings.get();
	brightnessSlider->setValue(sets.screenBrightness);
	audioSwitch->setValue(sets.sound);
	sleepSlider->setValue(sets.sleepTime);
}

void SettingsScreen::onStart(){
	Events::listen(Facility::Input, &queue);
}

void SettingsScreen::buildUI(){
	lv_obj_set_size(*this, 128, 128);

	bg = lv_obj_create(*this);
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_size(bg, 128, 128);
	lv_obj_set_pos(bg, 0, 0);
	lv_obj_set_style_bg_color(bg, lv_color_black(), 0);

	auto stats = (StatsManager*) Services.get(Service::Stats);
	const auto lvl = std::clamp((int) stats->getLevel()-1, 0, 5);

	lv_obj_set_style_bg_image_src(bg, BgPaths[lvl], 0);
	lv_obj_set_style_bg_image_opa(bg, LV_OPA_30, 0);
	lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);

	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_gap(*this, 5, 0);
	lv_obj_set_style_pad_hor(*this, 1, 0);
	lv_obj_set_style_pad_bottom(*this, 1, 0);

	auto startingSettings = settings.get();

	auto values = std::vector<const char*>(Settings::SleepText, Settings::SleepText + Settings::SleepSteps);
	printf("settings screen values: %d\n", values.size());

	sleepSlider = new DiscreteSliderElement(*this, "Sleep time", [this](uint8_t value){
		if(value >= Settings::SleepSteps) return;
		auto s = settings.get();
		s.sleepTime = value;
		settings.set(s);
	}, values, startingSettings.sleepTime);
	lv_group_add_obj(inputGroup, *sleepSlider);

	brightnessSlider = new SliderElement(*this, "Brightness", [this](uint8_t value){
		backlight.setBrightness(value);
	}, startingSettings.screenBrightness);
	lv_group_add_obj(inputGroup, *brightnessSlider);

	audioSwitch = new BoolElement(*this, "Sound", [](bool value){
		if(value){
			auto chirp = (ChirpSystem*) Services.get(Service::Audio);
			chirp->play({{ .startFreq = 600, .endFreq = 600, .duration = 50 },
						 { .startFreq = 0, .endFreq = 0, .duration = 100 },
						 { .startFreq = 400, .endFreq = 400, .duration = 50 }});
		}
	}, startingSettings.sound);
	lv_group_add_obj(inputGroup, *audioSwitch);

	factoryReset = new LabelElement(*this, "Factory reset", [this](){
		factoryResetPrompt = new Prompt(this, "Are you sure?\n\nThis will erase ALL data!");
		lv_obj_add_event_cb(*factoryResetPrompt, [](lv_event_t* e){
			auto screen = (SettingsScreen*) lv_event_get_user_data(e);
			screen->performFactoryReset();
		}, EV_PROMPT_YES, this);

		lv_obj_add_event_cb(*factoryResetPrompt, [](lv_event_t* e){
			auto screen = (SettingsScreen*) lv_event_get_user_data(e);
			delete screen->factoryResetPrompt;
			screen->factoryResetPrompt = nullptr;
		}, EV_PROMPT_NO, this);
	}, LV_ALIGN_LEFT_MID);
	lv_group_add_obj(inputGroup, *factoryReset);


	saveAndExit = new LabelElement(*this, "Save and Exit", [this](){
		transition([](){ return std::make_unique<PetScreen>(); });
	}, LV_ALIGN_LEFT_MID);
	lv_group_add_obj(inputGroup, *saveAndExit);

	for(int i = 0; i < lv_obj_get_child_count(*this); ++i){
		lv_obj_add_flag(lv_obj_get_child(*this, i), LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	}

	lv_obj_add_flag(*this, LV_OBJ_FLAG_SCROLLABLE);
}

void SettingsScreen::performFactoryReset(){
	nvs_flash_erase();
	backlight.fadeOut();
	gpio_set_direction((gpio_num_t)PIN_BL, GPIO_MODE_OUTPUT);
	gpio_set_level((gpio_num_t)PIN_BL, 0);
	audio.stop();
	esp_restart();
}
