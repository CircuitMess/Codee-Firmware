#include "Pins.h"
#include "Util/EfuseMeta.h"

static const char* TAG = "Pins";

int8_t Pins::get(Pin pin){
	if(instance == nullptr){
		instance = new Pins();

		uint8_t revision = 0;
		EfuseMeta::readRev(revision);

		if(revision == 0){
			instance->currentMap = &instance->Revision1;
		}else if(revision == 1){
			instance->currentMap = &instance->Revision2;
		}else{
			while(true){
				EfuseMeta::log();
				while(true);
			}
		}
	}

	assert(instance != nullptr);

	PinMap* pinMap = instance->currentMap;

	if(pinMap == nullptr){
		ESP_LOGE(TAG, "Pin map is invalid.!\n");
		return -1;
	}

	if(!pinMap->contains(pin)){
		ESP_LOGE(TAG, "Pin %d not mapped!\n", (int)pin);
		return -1;
	}

	return pinMap->at(pin);
}

void Pins::setLatest(){
	if(instance == nullptr){
		instance = new Pins();
	}

	instance->currentMap = instance->pinMaps.back();
}

Pins::Pins(){
	initPinMaps();
}

void Pins::initPinMaps(){
	Revision1 = {
			{ Pin::BtnA,     2 },
			{ Pin::BtnB,     3 },
			{ Pin::BtnC,     4 },
			{ Pin::BtnD,     5 },
			{ Pin::LedBl,    45 },
			{ Pin::LedRed,   38 },
			{ Pin::Buzz,     44 },
			{ Pin::BattRead, 6 },
			{ Pin::VrefEn,   7 },
			{ Pin::I2cSda,   33 },
			{ Pin::I2cScl,   34 },
			{ Pin::TftSck,   39 },
			{ Pin::TftMosi,  40 },
			{ Pin::TftDc,    41 },
			{ Pin::TftRst,   42 },
	};

	Revision2 = {
			{ Pin::BtnA,     16 },
			{ Pin::BtnB,     17 },
			{ Pin::BtnC,     9 },
			{ Pin::BtnD,     8 },
			{ Pin::LedBl,    6 },
			{ Pin::LedRed,   10 },
			{ Pin::Buzz,     7 },
			{ Pin::BattRead, 1 },
			{ Pin::VrefEn,   37 },
			{ Pin::I2cSda,   36 },
			{ Pin::I2cScl,   35 },
			{ Pin::TftSck,   2 },
			{ Pin::TftMosi,  3 },
			{ Pin::TftDc,    4 },
			{ Pin::TftRst,   5 },
	};
}

