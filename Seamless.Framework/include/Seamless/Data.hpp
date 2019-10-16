
#pragma once 

#include <Seamless/Export.hpp>

#include <cstdint>
#include <cstring>

#define NOMINMAX
#include <winsock2.h>
#define htobe16(x) htons(x)
#define htole16(x) (x)
#define be16toh(x) ntohs(x)
#define le16toh(x) (x)
#define htobe32(x) htonl(x)
#define htole32(x) (x)
#define be32toh(x) ntohl(x)
#define le32toh(x) (x)
#define htobe64(x) htonll(x)
#define htole64(x) (x)
#define be64toh(x) ntohll(x)
#define le64toh(x) (x)

namespace cmls
{

  enum ByteOrder : uint8_t
  {
    BO_BigEndian = 0,
    BO_LittleEndian = 1,
    BO_Network = BO_BigEndian
  };


  template<typename T>
  T toLittleEndian64(T & in)
  {
    uint64_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 8);
    a = htole64(a);
    memcpy((void*)&b, (void*)&a, 8);
    return b;
  }

  template<typename T>
  T fromLittleEndian64(T & in)
  {
    uint64_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 8);
    a = le64toh(a);
    memcpy((void*)&b, (void*)&a, 8);
    return b;
  }

  template<typename T>
  T toLittleEndian32(T& in)
  {
    uint32_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 4);
    a = htole32(a);
    memcpy((void*)&b, (void*)&a, 4);
    return b;
  }

  template<typename T>
  T fromLittleEndian32(T& in)
  {
    uint32_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 4);
    a = le32toh(a);
    memcpy((void*)&b, (void*)&a, 4);
    return b;
  }

  template<typename T>
  T toLittleEndian16(T& in)
  {
    uint16_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 2);
    a = htole16(a);
    memcpy((void*)&b, (void*)&a, 2);
    return b;
  }

  template<typename T>
  T fromLittleEndian16(T& in)
  {
    uint16_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 2);
    a = le16toh(a);
    memcpy((void*)&b, (void*)&a, 2);
    return b;
  }

  template<typename T>
  T toBigEndian64(T & in)
  {
    uint64_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 8);
    a = htobe64(a);
    memcpy((void*)&b, (void*)&a, 8);
    return b;
  }

  template<typename T>
  T fromBigEndian64(T & in)
  {
    uint64_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 8);
    a = be64toh(a);
    memcpy((void*)&b, (void*)&a, 8);
    return b;
  }

  template<typename T>
  T toBigEndian32(T& in)
  {
    uint32_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 4);
    a = htobe32(a);
    memcpy((void*)&b, (void*)&a, 4);
    return b;
  }

  template<typename T>
  T fromBigEndian32(T& in)
  {
    uint32_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 4);
    a = be32toh(a);
    memcpy((void*)&b, (void*)&a, 4);
    return b;
  }

  template<typename T>
  T toBigEndian16(T& in)
  {
    uint16_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 2);
    a = htobe16(a);
    memcpy((void*)&b, (void*)&a, 2);
    return b;
  }

  template<typename T>
  T fromBigEndian16(T& in)
  {
    uint16_t a = 0;
    T b;
    memcpy((void*)&a, (void*)&in, 2);
    a = be16toh(a);
    memcpy((void*)&b, (void*)&a, 2);
    return b;
  }

  template<typename T>
  T fromBigEndian(T & in)
  {
    if (sizeof(T) == 2)
    {
      return fromBigEndian16(in);
    }
    else if (sizeof(T) == 4)
    {
      return fromBigEndian32(in);
    }
    else if (sizeof(T) == 8)
    {
      return fromBigEndian64(in);
    }

    return in;
  }

  template<typename T>
  T toBigEndian(T & in)
  {
    if (sizeof(T) == 2)
    {
      return toBigEndian16(in);
    }
    else if (sizeof(T) == 4)
    {
      return toBigEndian32(in);
    }
    else if (sizeof(T) == 8)
    {
      return toBigEndian64(in);
    }

    return in;
  }

  template<typename T>
  T fromLittleEndian(T & in)
  {
    if (sizeof(T) == 2)
    {
      return fromLittleEndian16(in);
    }
    else if (sizeof(T) == 4)
    {
      return fromLittleEndian32(in);
    }
    else if (sizeof(T) == 8)
    {
      return fromLittleEndian64(in);
    }

    return in;
  }

  template<typename T>
  T toLittleEndian(T & in)
  {
    if (sizeof(T) == 2)
    {
      return toLittleEndian16(in);
    }
    else if (sizeof(T) == 4)
    {
      return toLittleEndian32(in);
    }
    else if (sizeof(T) == 8)
    {
      return toLittleEndian64(in);
    }

    return in;
  }

  template <typename Type>
  struct TypeWrapper
  {
    TypeWrapper(Type inValue)
    {
      value = inValue;
    }

    TypeWrapper(uint8_t const * data)
    {
      memcpy(begin(), data, size());
    }

    void encode(ByteOrder toByteOrder)
    {
      if (toByteOrder == BO_BigEndian)
      {
        value = toBigEndian(value);
      }
      else if (toByteOrder == BO_LittleEndian)
      {
        value = toLittleEndian(value);
      }
    }

    void decode(ByteOrder fromByteOrder)
    {
      if (fromByteOrder == BO_BigEndian)
      {
        value = fromBigEndian(value);
      }
      else if (fromByteOrder == BO_LittleEndian)
      {
        value = fromLittleEndian(value);
      }
    }

    uint8_t *begin()
    {
      return reinterpret_cast<uint8_t*>(&value);
    }
    uint8_t *end()
    {
      return reinterpret_cast<uint8_t*>(&value) + size();
    }

    static uint64_t size()
    {
      return sizeof(Type);
    }

    Type value;
  };

  // CRC-16-CCITT (poly 0x1021)
  CMLS_API uint16_t crc16(uint8_t const *data, uint64_t size);

  // zlib safe minimum  deflate buffer size, based on input size
  CMLS_API uint64_t deflateSize(uint64_t const & dataSize);

  // zlib decompress data
  CMLS_API bool inflate(uint8_t const *inData, size_t inSize, uint8_t *outData, size_t & outSize);

  // zlib compress data
  CMLS_API bool deflate(uint8_t const *inData, size_t inSize, uint8_t *outData, size_t & outSize);

}