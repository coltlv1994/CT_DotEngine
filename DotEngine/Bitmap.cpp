#include "Bitmap.h"
#include <cstring>
#include <stdexcept>

const unsigned int BITS_IN_SEGMENT = 32;

Bitmap::Bitmap(unsigned int p_noOfElements)
{
	m_maxBitIndex = p_noOfElements - 1;
	m_noOfSegments = (m_maxBitIndex + BITS_IN_SEGMENT) / BITS_IN_SEGMENT;
	m_bitmap = new uint32_t[m_noOfSegments];
	memset(m_bitmap, 0, m_noOfSegments);
}

Bitmap::~Bitmap()
{
	delete[] m_bitmap;
}

int Bitmap::ReadBit(uint32_t p_bitIndex)
{
	if (m_maxBitIndex < p_bitIndex)
	{
		// overflow
		throw std::overflow_error("Index overflow");
	}

	size_t segmentPosition = p_bitIndex >> 5; // 2^5 = 32
	uint32_t bitPosition = p_bitIndex & 0x0000001F;

	return (m_bitmap[segmentPosition] >> bitPosition) & 0x1;
}

void Bitmap::SetBit(uint32_t p_bitIndex)
{
	if (m_maxBitIndex < p_bitIndex)
	{
		// overflow
		throw std::overflow_error("Index overflow");
	}

	size_t segmentPosition = p_bitIndex >> 5;
	uint32_t bitPosition = p_bitIndex & 0x0000001F;

	m_bitmap[segmentPosition] |= 0x1 << bitPosition;
}

void Bitmap::ClearBit(uint32_t p_bitIndex)
{
	if (m_maxBitIndex < p_bitIndex)
	{
		// overflow
		throw std::overflow_error("Index overflow");
	}

	size_t segmentPosition = p_bitIndex >> 5;
	uint32_t bitPosition = p_bitIndex & 0x0000001F;

	m_bitmap[segmentPosition] &= ~(0x1 << bitPosition);
}

void Bitmap::SetAllZero()
{
	memset(m_bitmap, 0, m_noOfSegments);
}

void Bitmap::SetAllOne()
{
	memset(m_bitmap, 0xFF, m_noOfSegments);
}

size_t Bitmap::GetNoOfSegments()
{
	return m_noOfSegments;
}

void Bitmap::CopyBitmap(uint32_t* dest)
{
	// This will NOT do any boundary check
	memcpy(dest, m_bitmap, m_noOfSegments * 4); // every uint32_t takes 4 bytes
}