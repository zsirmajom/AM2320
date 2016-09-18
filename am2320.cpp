#include "AM2320.h"
#include <Wire.h>

#define AM2320_address  (0xB8 >> 1) // Since read  and write are separated in the library, we only need the upper 7 bits

typedef enum {
    AM2320_READ     = 0x03,
    AM2320_WRITE    = 0x10
} AM2320CommandCode_t;

unsigned int CRC16(byte *ptr, byte length) 
{ 
    unsigned int crc = 0xFFFF;
    unsigned char i;

    while (length--) {
        crc ^= *ptr++;
        for (i = 0; i < 8; i++) {
            if ((crc & 0x01) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

AM2320::AM2320(): t(0), h(0)
{
	Wire.begin();
}

int AM2320::Read()
{
	byte buf[8] = {0};

    // Wake up sensor (just send address and the hw I2C bus will wait for just enough time)
	Wire.beginTransmission(AM2320_address);
	Wire.endTransmission();
  
    // Send read command
	Wire.beginTransmission(AM2320_address);
	Wire.write(AM2320_READ);
	Wire.write(0x00);           // start address
	Wire.write(0x04);           // read length
    
	if (Wire.endTransmission(1) != 0) {
        return AM2320_TRANSMIT_ERROR;
        
    }
    
    // Wait at least 1.5 ms (datasheet instructions, though it seems to work with no delay)
	delayMicroseconds(1500);
    
	Wire.requestFrom(AM2320_address, 8); 
    
	for (int i = 0; i < 8; i++) {
        buf[i] = Wire.read();
    }

	// CRC check
	unsigned int Rcrc = (buf[7] << 8) + buf[6];
    
	if (Rcrc == CRC16(buf, 6)) {
		t = ((buf[4] & 0x7F) << 8) + buf[5];
		t = ((buf[4] & 0x80) >> 7) == 1 ? t * (-1) : t;

		h = (buf[2] << 8) + buf[3];
		return AM2320_ERROR_NONE;
	}
    
    return AM2320_CRC_CHECK_ERROR;
}
