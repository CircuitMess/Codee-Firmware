#ifndef CLOCKSTAR_FIRMWARE_SETTINGSSCREEN_H
#define CLOCKSTAR_FIRMWARE_SETTINGSSCREEN_H

#include "LV_Interface/LVScreen.h"
#include "Settings/Settings.h"
#include "Util/Events.h"
#include "Services/BacklightBrightness.h"
#include "Services/ChirpSystem.h"
#include "LV_Interface/LVModal.h"

class SettingsScreen : public LVScreen {
public:
	SettingsScreen();

private:
	virtual void onStarting() override;
	virtual void onStart() override;
	virtual void onStop() override;
	virtual void loop() override;

	void buildUI();

	void performFactoryReset();

	Settings& settings;
	BacklightBrightness& backlight;
	ChirpSystem& audio;

	lv_obj_t* bg = nullptr;
	class DiscreteSliderElement* sleepSlider = nullptr;
	class SliderElement* brightnessSlider = nullptr;
	class BoolElement* audioSwitch = nullptr;
	class LabelElement* factoryReset = nullptr;
	class LabelElement* saveAndExit = nullptr;

	class Prompt* factoryResetPrompt = nullptr;

	char bgPath[20] = { 0 };

	EventQueue queue;
};


#endif //CLOCKSTAR_FIRMWARE_SETTINGSSCREEN_H
