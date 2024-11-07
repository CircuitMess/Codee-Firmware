#include "esp_attr.h"
#include "hal/gpio_hal.h"

/** Function used to tell the linker to include this file with all its symbols. */
void bootloader_hooks_include(void){}

static const gpio_num_t BL = GPIO_NUM_45;

void IRAM_ATTR bootloader_before_init(void){
	gpio_ll_output_enable(&GPIO, BL);
	gpio_ll_set_level(&GPIO, BL, 1);

}

void bootloader_after_init(void){
	// ESP_LOGI("HOOK", "This hook is called AFTER bootloader initialization");
}
