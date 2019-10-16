
#include "Seamless/Stream.hpp"
#include "Seamless/Error.hpp"
#include "Seamless/Data.hpp"

cmls::Stream::Stream(StreamFlags const & flags /*= SF_Default*/, uint64_t const & reserve /*= 0*/)
{
  m_flags = flags;

  if (reserve > 0)
  {
    m_data.reserve(reserve);
  }
}

cmls::Stream::Stream(std::string const & filepath, StreamFlags const & flags /*= SF_Default*/)
{
  m_flags = flags;
  if (!readFile(filepath))
  {
    LogError("readFile failed");
  }
}

cmls::Stream::~Stream()
{

}


cmls::Stream::Stream(Stream const & other)
{
  m_data = std::vector<uint8_t>(other.begin(), other.end());
  m_flags = other.flags();
  seek(other.tell());
}

cmls::Stream & cmls::Stream::operator=(Stream const & other)
{
  m_data = std::vector<uint8_t>(other.begin(), other.end());
  m_flags = other.flags();
  seek(other.tell());

  return *this;
}

void cmls::Stream::write(uint8_t const *data, uint64_t const & size)
{
  m_data.insert(m_data.end(), data, data + size);
}

void cmls::Stream::swap(Stream & other, bool onlyData)
{
  m_data.swap(other.m_data);
  
  if (!onlyData)
  {
    auto oflags = m_flags;
    m_flags = other.m_flags;
    other.m_flags = oflags;

    auto ocursor = tell();
    seek(other.tell());
    other.seek(ocursor);
  }
}

void cmls::Stream::data(uint8_t const * data, uint64_t const & size)
{
  m_data = std::vector<uint8_t>(data, data + size);
  seek(0);
}

bool cmls::Stream::serialize(class Stream & toStream) const
{
  toStream.write(begin(), size());

  return true;
}

bool cmls::Stream::deserialize(class Stream & fromStream)
{
  write(fromStream.begin(), fromStream.size());

  return true;
}

bool cmls::Stream::readFile(std::string const & filepath)
{
  std::ifstream handle(filepath, std::ios_base::binary);
  if (!handle)
  {
    LogError("Failed to open file for reading (%s)", filepath.c_str());
    return false;
  }

  *this << handle;

  handle.close();
  return true;
}

bool cmls::Stream::writeFile(std::string const & filepath)
{
  std::ofstream handle(filepath, std::ios_base::binary);
  if (!handle)
  {
    LogError("Failed to open file for writing (%s)", filepath.c_str());
    return false;
  }

  handle.write(reinterpret_cast<char const*>(begin()), size());

  handle.close();
  return true;
}

void cmls::Stream::seek(uint64_t cursor)
{
  if (cursor > size())
  {
    cursor = size();
  }

  m_cursor = cursor;
}

uint8_t const * cmls::Stream::current() const
{
  return begin() + tell();
}

uint8_t const * cmls::Stream::begin() const
{
  return m_data.data();
}

uint8_t const * cmls::Stream::end() const
{
  return begin() + size();
}

uint64_t cmls::Stream::tell() const
{
  return m_cursor;
}

bool cmls::Stream::eof() const
{
  return current() >= end();
}

void cmls::Stream::flags(StreamFlags flags)
{
  m_flags = flags;
}

cmls::StreamFlags cmls::Stream::flags() const
{
  return m_flags;
}

uint64_t cmls::Stream::size() const
{
  return m_data.size();
}


void cmls::Stream::reserve(uint64_t const & reserve)
{
  m_data.reserve(reserve);
}

void cmls::Stream::clear()
{
  m_data.clear();
  seek(0);
}

uint64_t cmls::Stream::remaining() const
{
  return size() - tell();
}

void cmls::Stream::skip(int64_t num)
{
  if (num == 0)
  {
    seek(size());
    return;
  }

  int64_t cursor = int64_t(tell()) + num;
  if (cursor < 0)
  {
    cursor = 0;
  }

  seek(cursor);
}

bool cmls::Stream::read(std::vector<uint8_t> & data, uint64_t const & dataSize /*= 0*/)
{
  uint64_t readSize = dataSize;

  if (readSize == 0)
  {
    readSize = remaining();
  }

  if (readSize > remaining())
  {
    LogError("Not enough data for read");
    return false;
  }

  data.insert(data.end(), current(), current() + readSize);
  skip(readSize);

  return true;
}

bool cmls::Stream::decompress(Stream & toStream, uint64_t decompressedSize)
{
  std::vector<uint8_t> decompressedData(decompressedSize, 0);
  uint64_t outSize = decompressedSize;
  if (!cmls::inflate(current(), remaining(), decompressedData.data(), outSize))
  {
    LogError("cmls::inflate failed");
    return false;
  }

  toStream.write(decompressedData.data(), outSize);

  return true;
}

bool cmls::Stream::compress(Stream & toStream, uint64_t *outCompressedSize)
{
  uint64_t safeSize = cmls::deflateSize(m_data.size());
  std::vector<uint8_t> compressedData(safeSize, 0);
  if (!cmls::deflate(current(), remaining(), compressedData.data(), safeSize))
  {
    LogError("cmls::deflate failed");
    return false;
  }

  toStream.write(compressedData.data(), safeSize);

  if (outCompressedSize)
  {
    *outCompressedSize = safeSize;
  }

  return true;
}

cmls::Stream & cmls::Stream::operator<<(Serializable const & value)
{
  if (!value.serialize(*this))
  {
    LogError("serialize failed");
  }

  return *this;
}
//
//cmls::Stream & cmls::Stream::operator<<(glm::quat const & value)
//{
//  *this << value.x;
//  *this << value.y;
//  *this << value.z;
//  *this << value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::mat4 const & value)
//{
//  for (uint32_t y = 0; y < 4; y++)
//    for(uint32_t x = 0; x < 4; x++)
//      *this << value[x][y];
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::mat3 const & value)
//{
//  for (uint32_t y = 0; y < 3; y++)
//    for (uint32_t x = 0; x < 3; x++)
//      *this << value[x][y];
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::ivec4 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//  *this << value.z;
//  *this << value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::ivec3 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//  *this << value.z;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::ivec2 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::uvec4 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//  *this << value.z;
//  *this << value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::uvec3 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//  *this << value.z;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::uvec2 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::vec4 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//  *this << value.z;
//  *this << value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::vec3 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//  *this << value.z;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator<<(glm::vec2 const & value)
//{
//  *this << value.x;
//  *this << value.y;
//
//  return *this;
//}

cmls::Stream & cmls::Stream::operator<<(std::istream & value)
{
  uint8_t * readBuffer = new uint8_t[1024];

  do
  {
    value.read(reinterpret_cast<char*>(readBuffer), 1024);
    uint64_t bytesRead = value.gcount();
    m_data.insert(m_data.end(), readBuffer, readBuffer + bytesRead);

  } while (value.gcount() == 1024);

  delete[] readBuffer;
  return *this;
}

cmls::Stream & cmls::Stream::operator<<(std::string const & value)
{
  *this << (uint64_t)value.size();
  write(reinterpret_cast<uint8_t const *>(value.c_str()), value.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(double const & value)
{
  cmls::TypeWrapper<double> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());


  return *this;
}

cmls::Stream & cmls::Stream::operator<<(float const & value)
{
  cmls::TypeWrapper<float> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(int64_t const & value)
{
  cmls::TypeWrapper<int64_t> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(int32_t const & value)
{
  cmls::TypeWrapper<int32_t> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(int16_t const & value)
{
  cmls::TypeWrapper<int16_t> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(int8_t const & value)
{
  write(reinterpret_cast<uint8_t const*>(&value), 1);

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(uint64_t const & value)
{
  cmls::TypeWrapper<uint64_t> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(uint32_t const & value)
{
  cmls::TypeWrapper<uint32_t> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(uint16_t const & value)
{
  cmls::TypeWrapper<uint16_t> conv(value);

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.encode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.encode(BO_LittleEndian);
  }

  m_data.insert(m_data.end(), conv.begin(), conv.end());

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(uint8_t const & value)
{
  write(&value, 1);

  return *this;
}

cmls::Stream & cmls::Stream::operator<<(bool const & value)
{
  *this << uint8_t(value ? 1 : 0);
  return *this;
}

cmls::Stream & cmls::Stream::operator>>(Serializable & value)
{
  if (!value.deserialize(*this))
  {
    LogError("deserialize failed");
  }

  return *this;
}
//
//cmls::Stream & cmls::Stream::operator>>(glm::mat4 & value)
//{
//  for (uint32_t y = 0; y < 4; y++)
//    for (uint32_t x = 0; x < 4; x++)
//      *this >> value[x][y];
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::mat3 & value)
//{
//  for (uint32_t y = 0; y < 3; y++)
//    for (uint32_t x = 0; x < 3; x++)
//      *this >> value[x][y];
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::quat & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//  *this >> value.z;
//  *this >> value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::ivec4 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//  *this >> value.z;
//  *this >> value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::ivec3 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//  *this >> value.z;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::ivec2 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::uvec4 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//  *this >> value.z;
//  *this >> value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::uvec3 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//  *this >> value.z;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::uvec2 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::vec4 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//  *this >> value.z;
//  *this >> value.w;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::vec3 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//  *this >> value.z;
//
//  return *this;
//}
//
//cmls::Stream & cmls::Stream::operator>>(glm::vec2 & value)
//{
//  *this >> value.x;
//  *this >> value.y;
//
//  return *this;
//}

cmls::Stream & cmls::Stream::operator>>(std::ostream & value)
{
  value.write(reinterpret_cast<char const*>(begin()), size());
  skip(0);
  return *this;
}

cmls::Stream & cmls::Stream::operator>>(std::string & value)
{
  uint64_t valueSize = 0;
  *this >> valueSize;

  if (valueSize > remaining())
  {
    LogError("Not enough data left to read string value");
    return *this;
  }

  std::string returner(current(), current() + valueSize);
  value.swap(returner);

  skip(valueSize);

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(double & value)
{
  cmls::TypeWrapper<double> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(float & value)
{
  cmls::TypeWrapper<float> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(int64_t & value)
{
  cmls::TypeWrapper<int64_t> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(int32_t & value)
{
  cmls::TypeWrapper<int32_t> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(int16_t & value)
{
  cmls::TypeWrapper<int16_t> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(int8_t & value)
{
  std::vector<uint8_t> buff;
  if (read(buff, 1))
  {
    value = *reinterpret_cast<int8_t*>(&buff[0]);
  }

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(uint64_t & value)
{
  cmls::TypeWrapper<uint64_t> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(uint32_t & value)
{
  cmls::TypeWrapper<uint32_t> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(uint16_t & value)
{
  cmls::TypeWrapper<uint16_t> conv(current());

  if ((m_flags & SF_ByteOrder) == SF_BigEndian)
  {
    conv.decode(BO_BigEndian);
  }
  else if ((m_flags & SF_ByteOrder) == SF_LittleEndian)
  {
    conv.decode(BO_LittleEndian);
  }

  value = conv.value;
  skip(conv.size());

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(uint8_t & value)
{
  std::vector<uint8_t> buff;
  if (read(buff, 1))
  {
    value = buff[0];
  }

  return *this;
}

cmls::Stream & cmls::Stream::operator>>(bool & value)
{
  uint8_t temp = 0;
  *this >> temp;
  value = (temp != 0);

  return *this;
}
