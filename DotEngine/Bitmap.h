#pragma once
#include <cstdint>
// A customized bitmap realization;
// C++20 has <bit> 

// Keep it align with m_bitmap's
extern const unsigned int BITS_IN_SEGMENT;

class Bitmap
{
public:
	Bitmap(unsigned int p_noOfElements);
	~Bitmap();

	int ReadBit(uint32_t p_bitIndex);
	void SetBit(uint32_t p_bitIndex);
	void ClearBit(uint32_t p_bitIndex);

	void SetAllZero();
	void SetAllOne();

	size_t GetNoOfSegments();

	void CopyBitmap(uint32_t* dest);

private:
	uint32_t* m_bitmap; // use fixed length integer represent
	size_t m_noOfSegments;
	unsigned int m_maxBitIndex;
};

