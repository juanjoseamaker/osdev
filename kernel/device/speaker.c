#include "../kernel.h"
#include "speaker.h"
#include "pit.h"
#include "port.h"

void speaker_play(uint32_t freq) {
 	uint32_t div;
 	uint8_t tmp;
 
     // Change PIT 2 frequency
 	div = 1193180 / freq;
 	port_byte_out(0x43, 0xb6);
 	port_byte_out(0x42, (uint8_t) (div) );
 	port_byte_out(0x42, (uint8_t) (div >> 8));
 
    // Play
 	tmp = port_byte_in(0x61);
  	if (tmp != (tmp | 3))
 		port_byte_out(0x61, tmp | 3);
}

void speaker_stop() {
	uint8_t tmp = port_byte_in(0x61) & 0xFC;
    port_byte_out(0x61, tmp);
}

void song() {
    speaker_play(264); sleep(250);
    speaker_stop(); sleep(500);
    speaker_play(264); sleep(250);
    speaker_stop(); sleep(250);
    speaker_play(297); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(264); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(352); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(330); sleep(2000);
    speaker_stop(); sleep(500);
    speaker_play(264); sleep(250);
    speaker_stop(); sleep(500);
    speaker_play(264); sleep(250);
    speaker_stop(); sleep(250);
    speaker_play(297); sleep(1000);;
    speaker_stop(); sleep(250);
    speaker_play(264); sleep(1000);;
    speaker_stop(); sleep(250);
    speaker_play(396); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(352); sleep(2000);
    speaker_stop(); sleep(500);
    speaker_play(264); sleep(250);
    speaker_stop(); sleep(500);
    speaker_play(264); sleep(250);
    speaker_stop(); sleep(250);
    speaker_play(264); sleep(1000);;
    speaker_stop(); sleep(250);
    speaker_play(440); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(352); sleep(500);
    speaker_stop(); sleep(250);
    speaker_play(352); sleep(250);
    speaker_stop(); sleep(250);
    speaker_play(330); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(297); sleep(2000);
    speaker_stop(); sleep(500);
    speaker_play(466); sleep(250);
    speaker_stop(); sleep(500);
    speaker_play(466); sleep(250);
    speaker_stop(); sleep(250);
    speaker_play(440); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(352); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(396); sleep(1000);
    speaker_stop(); sleep(250);
    speaker_play(352); sleep(2000);
    speaker_stop(); sleep(250);
}