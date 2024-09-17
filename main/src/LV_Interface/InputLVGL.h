#ifndef CLOCKSTAR_FIRMWARE_INPUTLVGL_H
#define CLOCKSTAR_FIRMWARE_INPUTLVGL_H

#include <lvgl.h>
#include <map>
#include "Util/Events.h"
#include "Util/Threaded.h"
#include "../Devices/Input.h"

class InputLVGL : private Threaded {
public:
	InputLVGL();

	void read(lv_indev_t* drv, lv_indev_data_t* data);
	static InputLVGL* getInstance();

	[[nodiscard]] lv_indev_t* getIndev() const;

	void setVertNav(bool vertNav);

protected:
	void loop() override;

private:
	static const std::map<Input::Button, lv_key_t> keyMap;
	static const std::map<lv_key_t, lv_key_t> vertRemap;
	lv_indev_t* inputDevice;

	Input::Button lastKey = Input::B;
	Input::Data::Action action = Input::Data::Release;
	bool vertNav = false;

	EventQueue queue;
	static constexpr size_t QueueSize = 5;

	static InputLVGL* instance;
};


#endif //CLOCKSTAR_FIRMWARE_INPUTLVGL_H
