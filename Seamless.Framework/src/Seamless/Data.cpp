
#include "Seamless/Data.hpp"

#include "Seamless/Error.hpp"

#include <zlib.h>

uint16_t cmls::crc16(uint8_t const *data, uint64_t size)
{
  uint16_t crc = 0x1D0F;
  uint8_t x;

  while (size--)
  {
    x = crc >> 8 ^ *data++;
    x ^= x >> 4;
    crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ (x);
  }

  return (crc &= 0xffff);
}

uint64_t cmls::deflateSize(uint64_t const & dataSize)
{
  // Add 8 bytes, + 8 bytes for each 16 KiB
  return dataSize + ((1 + (dataSize / 16384)) * 8) + 8;
}

bool cmls::inflate(uint8_t const *inData, size_t inSize, uint8_t *outData, size_t & outSize)
{
  uLongf oSize = (uLongf)outSize;
  int result = uncompress(reinterpret_cast<uint8_t *>(outData), &oSize, reinterpret_cast<uint8_t const *>(inData), (uLong)inSize);
  outSize = (size_t)oSize;

  switch (result)
  {
  case Z_OK:
    break;
  case Z_MEM_ERROR:
    LogError("out of memory");
    return false;
  case Z_BUF_ERROR:
    LogError("buffer too small");
    return false;
  case Z_DATA_ERROR:
    LogError("invalid data");
    return false;
  default:
    LogError("unknown error (%i)", result);
    return false;
  }
  return true;
}

bool cmls::deflate(uint8_t const *inData, size_t inSize, uint8_t *outData, size_t & outSize)
{
  uLongf oSize = (uLongf)outSize;
  int result = compress(reinterpret_cast<uint8_t *>(outData), &oSize, reinterpret_cast<uint8_t const *>(inData), (uLong)inSize);
  outSize = (size_t)oSize;

  switch (result)
  {
  case Z_OK:
    break;
  case Z_MEM_ERROR:
    LogError("out of memory");
    return false;
  case Z_BUF_ERROR:
    LogError("buffer too small");
    return false;
  case Z_DATA_ERROR:
    LogError("invalid data");
    return false;
  default:
    LogError("unknown error (%i)", result);
    return false;
  }
  return true;
}
