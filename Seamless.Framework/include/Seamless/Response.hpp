
#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/Http.hpp>
#include <Seamless/Stream.hpp>

#include <cstdint>
#include <sstream>

namespace cmls
{
  /** Implement this in a class to allow it to be streamed into a response */
  class CMLS_API Responsive
  {
  public:
    /** Returns this instance as a string in a response */
    virtual std::vector<uint8_t> respond() = 0;
  };

  class CMLS_API Response : public cmls::HttpHeaderOwner
  {
  public:
    Response();
    virtual ~Response();

    std::vector<uint8_t> const &data() const;

    Response &operator<<(Responsive *responsive);

    // Type-native implementations
    Response &operator<<(float value);
    Response &operator<<(double value);
    Response &operator<<(std::string const &value);
    Response &operator<<(char const *value);
    Response &operator<<(bool value);
    Response &operator<<(char value);
    Response &operator<<(int64_t value);
    Response &operator<<(uint64_t value);

    Response &operator<<(cmls::Stream const &value);

    // Aliases towards the type-native implementations
    Response &operator<<(int32_t value); ///< int64_t
    Response &operator<<(uint32_t value); ///< uint64_t
    Response &operator<<(int16_t value); ///< int64_t
    Response &operator<<(uint16_t value); ///< uint64_t
    Response &operator<<(int8_t value); ///< int64_t
    Response &operator<<(uint8_t value); ///< uint64_t

    cmls::HttpStatusCode code() const;
    void code(cmls::HttpStatusCode newCode);

    std::string const &reason() const;
    void reason(std::string const &newReason);

  protected:
    std::vector<uint8_t> m_data;
    cmls::HttpStatusCode m_code = 200;
    std::string m_reason;
    
  };
}