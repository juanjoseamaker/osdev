#include "pit.h"
#include "../int/isr.h"
#include "port.h"
#include "../kernel.h"

#include <stdint.h>

static uint32_t pit_ticks = 0; // 1 millisecond per tick

void sleep(uint32_t ticks) {
	uint32_t wait_ticks = pit_ticks + ticks;
	if(wait_ticks < pit_ticks)
		panic("PIT sleep failed\n");
	
	while(pit_ticks < wait_ticks);
}

static inline void pit_send_cmd(uint8_t cmd) {
	port_byte_out(PIT_REG_COMMAND, cmd);
}

static inline void pit_send_data(uint16_t data, uint8_t counter) {
	uint8_t	port = (counter == PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
		((counter == PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);

	port_byte_out(port, (uint8_t)data);
}

static void pit_start_counter(uint32_t freq, uint8_t counter, uint8_t mode) {
	if(freq == 0)
		return;

	// 1193181 / divisor = freq -> divisor = 1193181 / freq
	uint16_t divisor = (uint16_t)(1193181/(uint16_t)freq);
	
	// send operational command words
	uint8_t ocw = 0;
	ocw = (ocw & ~PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~PIT_OCW_MASK_RL) | PIT_OCW_RL_DATA;
	ocw = (ocw & ~PIT_OCW_MASK_COUNTER) | counter;
	pit_send_cmd(ocw);
	
	// set frequency rate
	pit_send_data(divisor & 0xff, 0);
	pit_send_data((divisor >> 8) & 0xff, 0);
}

static void pit_callback(registers_t *regs) {
	pit_ticks++;

	if(pit_ticks == 0xffffffff)
		pit_ticks = 0;
}

void pit_init() {
	register_interrupt_handler(IRQ0, pit_callback);
	pit_start_counter(1000, PIT_OCW_COUNTER_0, PIT_OCW_MODE_SQUAREWAVEGEN); // 1 millisecond
}
