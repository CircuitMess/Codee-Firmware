#ifndef BIT_FIRMWARE_SINGLEDIGITALLED_H
#define BIT_FIRMWARE_SINGLEDIGITALLED_H

#include "SingleLED.h"
#include "Periph/PinOut.h"

class SingleDigitalLED : public SingleLED {
public:
	SingleDigitalLED(uint8_t pin);
	virtual ~SingleDigitalLED() override = default;

protected:
	void write(uint8_t val) override;

private:
	PinOut pinout;

};


#endif //BIT_FIRMWARE_SINGLEDIGITALLED_H
