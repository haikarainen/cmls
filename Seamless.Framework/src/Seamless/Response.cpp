
#include "Seamless/Response.hpp"

cmls::Response::Response()
{
}

cmls::Response::~Response()
{
}

std::string const &cmls::Response::data() const
{
  return m_data;
}

cmls::Response &cmls::Response::operator<<(Responsive *responsive)
{
  m_data += responsive->respond();
  return *this;
}

cmls::Response &cmls::Response::operator<<(float value)
{
  m_data += std::to_string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(double value)
{
  m_data += std::to_string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(std::string const &value)
{
  m_data += value;
  return *this;
}

cmls::Response &cmls::Response::operator<<(char const *value)
{
  m_data += value;
  return *this;
}

cmls::Response &cmls::Response::operator<<(bool value)
{
  m_data += value ? "true" : "false";
  return *this;
}

cmls::Response &cmls::Response::operator<<(char value)
{
  m_data.push_back(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(int64_t value)
{
  m_data += std::to_string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(uint64_t value)
{
  m_data += std::to_string(value);
  return *this;
}


cmls::Response &cmls::Response::operator<<(int32_t value)
{
  *this << int64_t(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(uint32_t value)
{
  *this << uint64_t(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(int16_t value)
{
  *this << int64_t(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(uint16_t value)
{
  *this << uint64_t(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(int8_t value)
{
  *this << int64_t(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(uint8_t value)
{
  *this << uint64_t(value);
  return *this;
}
