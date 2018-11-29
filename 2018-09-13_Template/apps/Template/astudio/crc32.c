#include "crc32.h"

/**
* @brief reflects a 32-bits data
*
* @param data: data to be reflected.
* @param dataLength: length of the data to be reflected in bits.
* @return data reflected.
*
*/
uint32_t reflect(uint32_t data, uint8_t dataLength)
{
	uint_fast32_t value;
	int_fast16_t loop;

	//
	// Clear our accumulator variable.
	//
	value = 0;

	//
	// Swap bit 0 for bit 7, bit 1 for bit 6, etc.
	//
	for (loop = 1; loop < (dataLength + 1); loop++)
	{
		if (data & 1)
		{
			value |= 1 << (dataLength - loop);
		}
		data >>= 1;
	}

	//
	// Return the reflected value.
	//
	return(value);
}

/**
* @brief Generates a 1024-bytes lookup table to calculate the CRC32.
*
* @param None.
* @return None.
*
*/
void initCRC32Table(void)
{
	uint_fast32_t polynomial;
	int_fast16_t loop, bit;

	//
	// This is the ANSI X 3.66 polynomial as required by the DFU
	// specification.
	//
	polynomial = 0x04C11DB7;

	for (loop = 0; loop <= 0xFF; loop++)
	{
		CRC32Table[loop] = reflect(loop, 8) << 24;
		for (bit = 0; bit < 8; bit++)
		{
			CRC32Table[loop] = ((CRC32Table[loop] << 1) ^
			(CRC32Table[loop] & ((uint32_t)1 << 31) ? polynomial : 0));
		}
		CRC32Table[loop] = reflect(CRC32Table[loop], 32);
	}
}

/**
* @brief Calculate de CRC32 of data.
*
* @param data: buffer on which The CRC32 will be calculated.
* @param dataLength: buffer length.
* @param crc32: start value for the CRC32 calculation.
* @return 4-bytes CRC32 value.
*
*/
uint32_t calculateCRC32(uint8_t* data, uint32_t dataLength, uint32_t crc32)
{
	uint32_t count;
	uint8_t *buffer;
	uint8_t ch;

	//
	// Get a pointer to the start of the data and the number of bytes to
	// process.
	//
	buffer = data;
	count = dataLength;

	//
	// Perform the algorithm on each byte in the supplied buffer using the
	// lookup table values calculated in initCRC32Table().
	//
	while (count--)
	{
		ch = *buffer++;
		crc32 = (crc32 >> 8) ^ CRC32Table[(crc32 & 0xFF) ^ ch];
	}

	//
	// Return the result.
	//
	return(crc32);
}