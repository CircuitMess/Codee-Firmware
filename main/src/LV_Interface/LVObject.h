#ifndef BATCONTROLLER_FIRMWARE_LVOBJECT_H
#define BATCONTROLLER_FIRMWARE_LVOBJECT_H

#include <lvgl.h>

class LVObject {
public:
	LVObject(const LVObject&) = delete;
	LVObject& operator=(const LVObject&) = delete;

	LVObject(lv_obj_t* parent);
	virtual ~LVObject();

	operator lv_obj_t*();

protected:
	lv_obj_t* obj = nullptr;

private:
	bool deleting = false;
};


#endif //BATCONTROLLER_FIRMWARE_LVOBJECT_H
