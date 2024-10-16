#ifndef CODEE_FIRMWARE_OSICON_H
#define CODEE_FIRMWARE_OSICON_H

#include "LV_Interface/LVObject.h"

class OSIcon : public LVObject {
public:
	OSIcon(lv_obj_t* parent, uint8_t level);

	void setLevel(uint8_t level);

private:
	lv_obj_t* logo;
	lv_obj_t* version;
};


#endif //CODEE_FIRMWARE_OSICON_H
