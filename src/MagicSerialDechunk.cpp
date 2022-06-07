#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define MAGICCHUNK_DEBUG
#include "MagicSerialDechunk.h"

void serial_dechunk_init(struct SerialDechunk *sp,
	                     uint16_t chunksize,
	                     void (*cb)(struct SerialDechunk *sp)) {
	memset(sp, 0, sizeof *sp);
	sp->chunksize = chunksize;
	sp->b = (uint8_t *)calloc(chunksize, 1);
	sp->_state = CHUNKSTATE_EMPTY;
	sp->add = _serial_dechunk_add;
	sp->reset_state = _serial_dechunk_reset;
	sp->cb = cb;
	sp->_ctr = 0;
}
void _serial_dechunk_reset(struct SerialDechunk *sp) {
	sp->_ctr = 0;
	sp->_state = CHUNKSTATE_EMPTY;
}
void _serial_dechunk_add(struct SerialDechunk *sp, uint8_t c) {
	if (sp->_state == CHUNKSTATE_EMPTY) {
		if (c == SER_STSEQ1) sp->_state = CHUNKSTATE_START1;
		else {
			DSPL("Found data without start sequence");
		}
	} else if (sp->_state == CHUNKSTATE_START1) {
		if (c == SER_STSEQ2) sp->_state = CHUNKSTATE_GATHER;
		else {
			sp->_state = CHUNKSTATE_EMPTY; // Failed to find 2nd char of Start Magic
			DSPL("Missing 2nd byte of start sequence");
		}
	} else if (sp->_state == CHUNKSTATE_GATHER) {
		if (sp->_ctr < sp->chunksize) {
			sp->b[sp->_ctr] = c;
			sp->_ctr++;
		}
		if (sp->_ctr == sp->chunksize) sp->_state = CHUNKSTATE_END1;
	} else if (sp->_state == CHUNKSTATE_END1) {
		if (c == SER_ENSEQ1) sp->_state = CHUNKSTATE_END2;
		else { /* Failed to find magic (chunk overflowed)
				  Since the data could have been corrupt anywhere in the
				  chunk we discard the whole thing */
			DSPL("Exceeded chunk size without end sequence");
			sp->reset_state(sp);
		}
	} else if (sp->_state == CHUNKSTATE_END2) {
		if (c != SER_ENSEQ2) { // fail to get 2nd end magic
			DSPL("Missing 2nd byte of end sequence");
			sp->reset_state(sp);
		} else {                          // SUCCESS!!
			if (sp->cb) (*sp->cb)(sp);
			sp->reset_state(sp);
		}
	}
}


