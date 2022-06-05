#ifndef _MAGICSERIALDECHUNK_H
#define _MAGICSERIALDECHUNK_H

#ifdef ARDUINO
	#include <Arduino.h>
	#ifdef MAGICCHUNK_DEBUG
		#define DSP(v) Serial.print(v)
		#define DSPL(v) Serial.println(v)
	#else
		#define DSP(v)
		#define DSPL(v)
	#endif
#else
	#ifdef MAGICCHUNK_DEBUG
		#include <iostream>
		#define DSP(v)  do { std::cout << v; } while (0)
		#define DSPL(v) do { std::cout << v << std::endl; } while (0)
	#else
		#define DSP(v)
		#define DSPL(v)
	#endif
#endif

#if (!defined(SER_STSEQ1) || !defined(SER_STSEQ2) \
		|| !defined(SER_ENSEQ1) || !defined(SER_ENSEQ2))
	#define SER_STSEQ1 0x02
	#define SER_STSEQ2 0xFE
	#define SER_ENSEQ1 0x03
	#define SER_ENSEQ2 0xFD
#endif

enum chunkenum_dechunk {
	CHUNKSTATE_EMPTY=0,
	CHUNKSTATE_START1,
	CHUNKSTATE_GATHER,
	CHUNKSTATE_END1,
	CHUNKSTATE_END2
};
struct SerialDechunk;
struct SerialDechunk {
	uint8_t *b; // buffer
	uint16_t chunksize;
	uint16_t ctr;
	void (*add)(struct SerialDechunk *sp, uint8_t c);
	void (*cb)(struct SerialDechunk *sp);
	void (*reset_state)(struct SerialDechunk *sp);
	int _state;
};

/* Call to initialize existing struct */
void serial_dechunk_init(struct SerialDechunk *sp,
	                     uint16_t chunksize,
	                     void (*cb)(struct SerialDechunk *sp));

void _serial_dechunk_reset(struct SerialDechunk *sp);
void _serial_dechunk_add(struct SerialDechunk *sp, uint8_t c);
#endif // _MAGICSERIALDECHUNK_H
