#include "JigHWTest.h"
#include "SPIFFSChecksum.hpp"
#include <Pins.hpp>
#include <soc/efuse_reg.h>
#include <esp_efuse.h>
#include <ctime>
#include <iostream>
#include <esp_mac.h>
#include "Util/Services.h"
#include <driver/ledc.h>
#include "Devices/Input.h"
#include "Util/HWVersion.h"
#include "Periph/PWM.h"
#include "Services/ChirpSystem.h"
#include "Util/Events.h"


JigHWTest* JigHWTest::test = nullptr;
Display* JigHWTest::display = nullptr;
LGFX_Device* JigHWTest::canvas = nullptr;
I2C* JigHWTest::i2c = nullptr;
RTC* JigHWTest::rtc = nullptr;


JigHWTest::JigHWTest(){
	display = new Display();
	canvas = &display->getLGFX();

	i2c = new I2C(I2C_NUM_0, (gpio_num_t) I2C_SDA, (gpio_num_t) I2C_SCL);
	rtc = new RTC(*i2c);

	test = this;

	tests.push_back({ JigHWTest::RTCTest, "RTC", [](){} });
	tests.push_back({ JigHWTest::Time1, "RTC crystal", [](){} });
	tests.push_back({ JigHWTest::Time2, "RTC crystal", [](){} });
	tests.push_back({ JigHWTest::SPIFFSTest, "SPIFFS", [](){} });
	tests.push_back({ JigHWTest::BatteryRef, "Battery reference", [](){}});
	tests.push_back({ JigHWTest::Buttons, "Buttons", [](){}});
	tests.push_back({ JigHWTest::HWVersion, "Hardware version", [](){ esp_efuse_batch_write_cancel(); } });
}

bool JigHWTest::checkJig(){
	char buf[7];
	int wp = 0;

	uint32_t start = millis();
	int c;
	while(millis() - start < CheckTimeout){
		vTaskDelay(1);
		c = getchar();
		if(c == EOF) continue;
		buf[wp] = (char) c;
		wp = (wp + 1) % 7;

		for(int i = 0; i < 7; i++){
			int match = 0;
			static const char* target = "JIGTEST";

			for(int j = 0; j < 7; j++){
				match += buf[(i + j) % 7] == target[j];
			}

			if(match == 7) return true;
		}
	}

	return false;
}


void JigHWTest::start(){
	uint64_t _chipmacid = 0LL;
	esp_efuse_mac_get_default((uint8_t*) (&_chipmacid));
	printf("\nTEST:begin:%llx\n", _chipmacid);

	esp_efuse_batch_write_begin();

	gpio_config_t cfg = {
			.pin_bit_mask = ((uint64_t) 1) << PIN_BL,
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&cfg);
	gpio_set_level((gpio_num_t) PIN_BL, 0);

	canvas->clear(0);
	rgb();

	canvas->clear(TFT_BLACK);
	canvas->setTextColor(TFT_GOLD);
	canvas->setTextWrap(true, true);
	canvas->setTextDatum(textdatum_t::middle_center);

	canvas->setTextFont(0);
	canvas->setTextSize(1);
	canvas->setCursor(0, 6);

	canvas->print("Codee test");
	canvas->setCursor(canvas->width() / 2, 16);
	canvas->println();

	bool pass = true;
	for(const Test& test : tests){
		currentTest = test.name;

		canvas->setTextColor(TFT_WHITE);
		canvas->printf("%s: ", test.name);

		printf("TEST:startTest:%s\n", currentTest);

		bool result = test.test();

		canvas->setTextColor(result ? TFT_GREEN : TFT_RED);
		canvas->printf("%s\n", result ? "PASSED" : "FAILED");

		printf("TEST:endTest:%s\n", result ? "pass" : "fail");

		if(!(pass &= result)){
			if(test.onFail){
				test.onFail();
			}

			break;
		}
	}

	if(!pass){
		printf("TEST:fail:%s\n", currentTest);
		vTaskDelete(nullptr);
	}

	esp_efuse_batch_write_commit();

	printf("TEST:passall\n");

	//------------------------------------------------------

	canvas->print("\n\n");
	canvas->setTextColor(TFT_GREEN);
	canvas->print("All OK!");

	AudioVisualTest();
}

void JigHWTest::rgb(){
	static const char* names[] = { "RED", "GREEN", "BLUE" };
	static const uint16_t colors[] = { TFT_RED, TFT_GREEN, TFT_BLUE };
	for(int i = 0; i < 3; i++){
		canvas->clear(colors[i]);
		canvas->setCursor(20, 40);
		canvas->setTextFont(0);
		canvas->setTextSize(2);
		canvas->print(names[i]);
		vTaskDelay(500);
	}
}

void JigHWTest::log(const char* property, const char* value){
	printf("%s:%s:%s\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, float value){
	printf("%s:%s:%f\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, double value){
	printf("%s:%s:%lf\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, bool value){
	printf("%s:%s:%s\n", currentTest, property, value ? "TRUE" : "FALSE");
}

void JigHWTest::log(const char* property, uint32_t value){
	printf("%s:%s:%lu\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, int32_t value){
	printf("%s:%s:%ld\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, const std::string& value){
	printf("%s:%s:%s\n", currentTest, property, value.c_str());
}

bool JigHWTest::BatteryRef(){
	ADC adc(ADC_UNIT_1);

	const auto config = [&adc](int pin, adc_cali_handle_t& cali, std::unique_ptr<ADCReader>& reader){
		adc_unit_t unit;
		adc_channel_t chan;
		ESP_ERROR_CHECK(adc_oneshot_io_to_channel(pin, &unit, &chan));
		assert(unit == adc.getUnit());

		adc.config(chan, {
				.atten = ADC_ATTEN_DB_0,
				.bitwidth = ADC_BITWIDTH_12
		});

		const adc_cali_curve_fitting_config_t curveCfg = {
				.unit_id = unit,
				.chan = chan,
				.atten = ADC_ATTEN_DB_0,
				.bitwidth = ADC_BITWIDTH_12
		};
		ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&curveCfg, &cali));

		static constexpr float Factor = 4.0f;
		static constexpr float Offset = 0;
		reader = std::make_unique<ADCReader>(adc, chan, cali, Offset, Factor);
	};

	adc_cali_handle_t cali;
	std::unique_ptr<ADCReader> reader;
	config(PIN_BATT, cali, reader);

	static constexpr int CalReads = 10;

	PinOut refSwitch(PIN_VREF);
	refSwitch.on();

	delayMillis(100);
	for(int i = 0; i < CalReads; i++){
		reader->sample();
		delayMillis(10);
	}

	float total = 0;
	for(int i = 0; i < CalReads; i++){
		total += reader->sample();
		delayMillis(10);
	}
	const float reading = total / (float) CalReads;
	const float offset = reading - VoltRef;

	refSwitch.off();

	test->log("reading", reading);
	test->log("offset", (int32_t) offset);

	if(std::abs(offset) > VoltOffsetTolerance){
		test->log("error", "offset too big");
		return false;
	}

	return true;
}

bool JigHWTest::SPIFFSTest(){
	auto ret = esp_vfs_spiffs_register(&spiffsConfig);
	if(ret != ESP_OK){
		test->log("spiffs", false);
		return false;
	}

	for(const auto& f : SPIFFSChecksums){
		auto file = fopen(f.name, "rb");
		if(file == nullptr){
			test->log("missing", f.name);
			return false;
		}

		uint32_t sum = calcChecksum(file);
		fclose(file);

		if(sum != f.sum){
			test->log("file", f.name);
			test->log("expected", (uint32_t) f.sum);
			test->log("got", (uint32_t) sum);

			return false;
		}
	}

	return true;
}

uint32_t JigHWTest::calcChecksum(FILE* file){
	if(file == nullptr) return 0;

#define READ_SIZE 512

	uint32_t sum = 0;
	uint8_t b[READ_SIZE];
	size_t read = 0;
	while((read = fread(b, 1, READ_SIZE, file))){
		for(int i = 0; i < read; i++){
			sum += b[i];
		}
	}

	return sum;
}

void JigHWTest::AudioVisualTest(){
	PinOut led(PIN_LED);
	bool state = false;
	for(;;){
		state = !state;
		led.set(state);
		delayMillis(500);
	}
}

bool JigHWTest::RTCTest(){
	auto ret = i2c->write(0x51, { 0, 0, 0 }, 10);
	if(ret == ESP_OK){
		test->log("begin", true);
	}else{
		test->log("begin", false);
		return false;
	}

	return rtc->init();
}

bool JigHWTest::Time1(){
	auto t = rtc->getTime();
	auto unixt = mktime(&t);

	vTaskDelay(2000 / portTICK_PERIOD_MS);

	auto t2 = rtc->getTime();
	auto unixt2 = mktime(&t2);
	auto diff = std::difftime(unixt2, unixt);

	if(diff != 1 && diff != 2){
		test->log("time passage (expected 1s or 2s)", (uint32_t) diff);
		return false;
	}

	return true;
}

bool JigHWTest::Time2(){
	static constexpr size_t count = 1000;

	time_t lastTime = 0;

	for(uint32_t i = 0; i < count; i++){

		auto t = rtc->getTime();
		auto unixt = mktime(&t);

		if(i == 0){
			lastTime = unixt;
			continue;
		}
		vTaskDelay(1);

		auto diff = abs(difftime(unixt, lastTime));
		if(diff > 1){
			test->log("reading", i);
			test->log("diff", diff);
			JigHWTest::canvas->printf("diff: %.2f", diff);
			return false;
		}

		lastTime = unixt;
	}
	return true;
}

bool JigHWTest::HWVersion(){
	uint16_t version = 1;
	bool result = HWVersion::readVersion(version);

	if(!result){
		test->log("HW version", "couldn't read from efuse");
		return false;
	}

	if(version != 0){
		test->log("Existing HW version", (uint32_t) version);
		if(version == HWVersion::getHardcodedVersion()){
			test->log("Already fused.", (uint32_t) version);
			return true;
		}else{
			test->log("Wrong binary already fused!", (uint32_t) version);
			return false;
		}
	}

	return HWVersion::write();
}

bool JigHWTest::Buttons(){
	PWM buzzPwm(PIN_BUZZ, LEDC_CHANNEL_0, true);
	ChirpSystem audio(buzzPwm);
	const auto buzz = [&audio](){
		audio.play({ Chirp{ 200, 200, 100 }});
	};

	EventQueue evts(12);
	Input input(true);
	input.begin();
	vTaskDelay(200);
	Events::listen(Facility::Input, &evts);

	std::unordered_set<Input::Button> pressed;
	std::unordered_set<Input::Button> released;

	test->instr("Pritisni sve\ngumbe redom.");
	audio.play({
					   Chirp{ 200, 200, 100 },
					   Chirp{ 0, 0, 50 },
					   Chirp{ 200, 200, 100 },
					   Chirp{ 0, 0, 50 },
					   Chirp{ 200, 200, 100 }
			   });

	for(;;){
		Event evt{};
		if(!evts.get(evt, portMAX_DELAY)) continue;

		auto data = (Input::Data*) evt.data;
		if(data->action == Input::Data::Press){
			printf("press\n");
			pressed.insert(data->btn);
			buzz();
		}else{
			printf("release\n");
			released.insert(data->btn);
		}

		free(evt.data);
		if(pressed.size() == 4 && released.size() == 4) break;
	}

	Events::unlisten(&evts);

	input.end();

	vTaskDelay(300);
	audio.play({
					   Chirp{ 200, 200, 100 },
					   Chirp{ 0, 0, 50 },
					   Chirp{ 200, 200, 100 }
			   });
	vTaskDelay(500);

	return true;
}

void JigHWTest::instr(const char* msg){
	canvas->setTextColor(TFT_GOLD);
	canvas->print(msg);
	canvas->print(" ");
}