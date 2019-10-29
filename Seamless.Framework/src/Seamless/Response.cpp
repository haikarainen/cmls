
#include "Seamless/Response.hpp"

cmls::Response::Response()
{
}

cmls::Response::~Response()
{
}

std::vector<uint8_t> const &cmls::Response::data() const
{
  return m_data;
}

cmls::Response &cmls::Response::operator<<(Responsive *responsive)
{
  auto newData =responsive->respond();
  m_data.insert(m_data.end(), newData.begin(), newData.end());
  return *this;
}

cmls::Response &cmls::Response::operator<<(float value)
{
  *this << std::to_string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(double value)
{
  *this << std::to_string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(std::string const &value)
{
  auto data = reinterpret_cast<uint8_t const*>(value.c_str());
  m_data.insert(m_data.end(), data, data + value.size());
  m_data.push_back('\0');
  return *this;
}

cmls::Response &cmls::Response::operator<<(char const *value)
{
  *this << std::string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(bool value)
{
  *this << (value ? "true" : "false");
  return *this;
}

cmls::Response &cmls::Response::operator<<(char value)
{
  m_data.push_back(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(int64_t value)
{
  *this << std::to_string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(uint64_t value)
{
  *this << std::to_string(value);
  return *this;
}

cmls::Response &cmls::Response::operator<<(cmls::Stream const& value)
{
  m_data.insert(m_data.end(), value.begin(), value.end());
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

cmls::HttpStatusCode cmls::Response::code() const
{
  return m_code;
}

void cmls::Response::code(cmls::HttpStatusCode newCode)
{
  m_code = newCode;
}

std::string const& cmls::Response::reason() const
{
  return m_reason;
}

void cmls::Response::reason(std::string const& newReason)
{
  m_reason = newReason;
}
