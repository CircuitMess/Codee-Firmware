#include "esp_attr.h"
#include "hal/gpio_hal.h"
#include "hal/efuse_ll.h"
#include "esp_log.h"

static const char* TAG = "hook";

/** Function used to tell the linker to include this file with all its symbols. */
void bootloader_hooks_include(void){}

void readEfuse(uint16_t* PID, uint8_t* Revision){
	/**
	 * Reading is done using raw register reads.
	 * Using the 'efuse' component results in crashes (TG0WDT_SYS_RST) !!!
	 *
	 * Block 3 = User block
	 * 00 00 XX YY | ZZ 00 00 00
	 * XX - lower byte of PID
	 * YY - upper byte of PID
	 * ZZ - Revision byte
	 */

	//Byteswapping is applied to have the same appearance as in 'espefuse summary' (v4.8.1)
	uint32_t reg = REG_READ(EFUSE_RD_USR_DATA0_REG);
	uint32_t reversedReg = (((reg) & 0xff000000) >> 24) | (((reg) & 0x00ff0000) >> 8) | (((reg) & 0x0000ff00) << 8) | (((reg) & 0x000000ff) << 24);
	uint16_t readPID = ((reversedReg & 0xFF00) >> 8 )| ((reversedReg & 0xFF) << 8);

	reg =  REG_READ(EFUSE_RD_USR_DATA1_REG);
	reversedReg = (((reg) & 0xff000000) >> 24) | (((reg) & 0x00ff0000) >> 8) | (((reg) & 0x0000ff00) << 8) | (((reg) & 0x000000ff) << 24);
	uint8_t readRevision = ((reversedReg & 0xFF000000) >> 24);

	ESP_LOGI(TAG, "PID: %d, rev: %d", readPID, readRevision);

	*PID = readPID;
	*Revision = readRevision;
}

void IRAM_ATTR bootloader_before_init(void){
	uint16_t pid = 0;
	uint8_t rev = 0;
	readEfuse(&pid, &rev);

	uint32_t bl = GPIO_NUM_45;
	if(rev == 1){
		bl = GPIO_NUM_6;
	}

	gpio_ll_output_enable(&GPIO, bl);
	gpio_ll_set_level(&GPIO, bl, 1);
}

void bootloader_after_init(void){
	// ESP_LOGI("HOOK", "This hook is called AFTER bootloader initialization");
}
