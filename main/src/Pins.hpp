#ifndef CLOCKSTAR_LIBRARY_PINS_HPP
#define CLOCKSTAR_LIBRARY_PINS_HPP

#include "Util/Pins.h"

#define BTN_A Pins::get(Pin::BtnA)
#define BTN_B Pins::get(Pin::BtnB)
#define BTN_C Pins::get(Pin::BtnC)
#define BTN_D Pins::get(Pin::BtnD)

#define PIN_BL Pins::get(Pin::LedBl)
#define PIN_LED Pins::get(Pin::LedRed)
#define PIN_BUZZ Pins::get(Pin::Buzz)
#define PIN_BATT Pins::get(Pin::BattRead)
#define PIN_VREF Pins::get(Pin::VrefEn)

#define I2C_SDA Pins::get(Pin::I2cSda)
#define I2C_SCL Pins::get(Pin::I2cScl)

#define TFT_SCK Pins::get(Pin::TftSck)
#define TFT_MOSI Pins::get(Pin::TftMosi)
#define TFT_DC Pins::get(Pin::TftDc)
#define TFT_RST Pins::get(Pin::TftRst)

#endif //CLOCKSTAR_LIBRARY_PINS_HPP
