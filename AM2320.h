#ifndef AM2320_H
#define AM2320_H

#include <Arduino.h>

typedef enum {
    AM2320_ERROR_NONE       = 0,
    AM2320_TRANSMIT_ERROR   = 1,
    AM2320_CRC_CHECK_ERROR  = 2
} AM2320ErrorCode_t;

class AM2320
{
	public:
		AM2320();
		float t;
		float h;
		int Read(void);
};

#endif
