#include "SingleDigitalLED.h"

SingleDigitalLED::SingleDigitalLED(uint8_t pin) : SingleLED(0xFF), pinout(pin){

}


void SingleDigitalLED::write(uint8_t val){
	pinout.set(val > 0);
}
