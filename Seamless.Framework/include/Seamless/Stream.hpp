
#pragma once

#include <Seamless/Export.hpp>

#include <vector>
#include <string>

namespace cmls
{
  /** Combination of streamflags, decides how a stream will behave */
  enum StreamFlags : uint8_t
  {
    SF_None = 0b0000'0000, //< Just store
    SF_ByteOrder = 0b0000'0011, //< What byte order the data is written/read in
    SF_LittleEndian = 0b0000'0000, //< Little endian byte order
    SF_BigEndian = 0b0000'0001, //< Big endian byte order
    SF_NetOrder = 0b0000'0001, //< Network byte order
    SF_HostOrder = 0b0000'0010, //< Host byte order

    SF_Default = SF_NetOrder, //< Default flags
  };

  class Stream;

  /** Baseclass for non-POD types that needs to be serializable in a well-defined manner */
  class CMLS_API Serializable
  {
  public:
    virtual bool serialize(Stream & toStream) const = 0;
    virtual bool deserialize(Stream & fromStream) = 0;
  };

  /** A beefy fucker for data serialization. */
  class CMLS_API Stream : public Serializable
  {
  public:

    /** Will create a stream with its own memory, with the specified flags, and optionally reserves the given number of bytes */
    Stream(StreamFlags const & flags = SF_Default, uint64_t const & reserve = 0);

    /** Will create a stream by loading the full contents of the given file */
    Stream(std::string const & filepath, StreamFlags const & flags = SF_Default);

    /** Copy constructor, will always return a new stream with its own copy of the data */
    Stream(Stream const & other);

    /** Assignment operator, will copy the data*/
    Stream & operator=(Stream const & other);

    /** Destructs the stream, will free any data that it owns */
    ~Stream();

    /** Writes flat data to other stream */
    virtual bool serialize(class Stream & toStream) const override;

    /** Writes flat data from other stream */
    virtual bool deserialize(class Stream & fromStream) override;

    /** Reads stream contents from file */
    bool readFile(std::string const & filepath);

    /** Writes stream contents to file */
    bool writeFile(std::string const & filepath);

    /** Flushes data behind the cursor, leaving only unread data left in the stream. */
    void flush();

    /** Returns the cursor position */
    uint64_t tell() const;

    /** Returns the remaining number of bytes */
    uint64_t remaining() const;

    /** True if there is nothing left to read */
    bool eof() const;

    /** Seeks to the given absolute position */
    void seek(uint64_t newCursor);

    /** Advances cursor a number of bytes. 0 skips to end. */
    void skip(int64_t size);

    /** Returns a pointer to data at cursor */
    uint8_t const *current() const;

    /** Returns a pointer to data at beginning of stream */
    uint8_t const *begin() const;

    /** Returns a pointer to data at end of stream */
    uint8_t const *end() const;

    /** Returns the data size, in bytes. */
    uint64_t size() const;

    /** Sets new stream flags */
    void flags(StreamFlags flags);

    /** Returns the current stream flags */
    StreamFlags flags() const;

    /** Sets new data, resets cursor. */
    void data(uint8_t const * data, uint64_t const & size);

    /** Reserves memory. */
    void reserve(uint64_t const & reserve);

    /** Clears the data. */
    void clear();

    /** Read data from this stream. Advances cursor. */
    bool read(std::vector<uint8_t> & data, uint64_t const & size = 0);

    /** Write data to this stream */
    void write(uint8_t const * data, uint64_t const & size);

    /** Efficiently swaps the contents of one stream with the other. This includes the stream flags and cursor unless 'onlyData' is true. */
    void swap(Stream & other, bool onlyData = false);

    /** 
     * Attempts to decompress the data in this stream to new stream by writing decompressed data to it.
     * Does nothing to output stream if operation fails.
     */
    bool decompress(Stream & toStream, uint64_t decompressedSize);

    /**
     * Attempts to compress the data in this stream to new stream by writing compressed data to it.
     * Does nothing to output stream if operation fails.
     */
    bool compress(Stream & toStream, uint64_t *outCompressedSize = nullptr);

    Stream & operator<<(Serializable const & value);
    Stream & operator>>(Serializable & value);

    Stream & operator<<(bool          const & value);
    Stream & operator<<(uint8_t       const & value);
    Stream & operator<<(uint16_t      const & value);
    Stream & operator<<(uint32_t      const & value);
    Stream & operator<<(uint64_t      const & value);
    Stream & operator<<(int8_t        const & value);
    Stream & operator<<(int16_t       const & value);
    Stream & operator<<(int32_t       const & value);
    Stream & operator<<(int64_t       const & value);
    Stream & operator<<(float         const & value);
    Stream & operator<<(double        const & value);
    Stream & operator<<(std::string   const & value);
    Stream & operator<<(std::istream  & value);

    /*
    // If glm ever makes it into this API again, artifact from KIT engine
    Stream & operator<<(glm::vec2     const & value);
    Stream & operator<<(glm::vec3     const & value);
    Stream & operator<<(glm::vec4     const & value);
    Stream & operator<<(glm::uvec2    const & value);
    Stream & operator<<(glm::uvec3    const & value);
    Stream & operator<<(glm::uvec4    const & value);
    Stream & operator<<(glm::ivec2    const & value);
    Stream & operator<<(glm::ivec3    const & value);
    Stream & operator<<(glm::ivec4    const & value);
    Stream & operator<<(glm::mat3     const & value);
    Stream & operator<<(glm::mat4     const & value);
    Stream & operator<<(glm::quat     const & value);
    */

    Stream & operator>>(bool          & value);
    Stream & operator>>(uint8_t       & value);
    Stream & operator>>(uint16_t      & value);
    Stream & operator>>(uint32_t      & value);
    Stream & operator>>(uint64_t      & value);
    Stream & operator>>(int8_t        & value);
    Stream & operator>>(int16_t       & value);
    Stream & operator>>(int32_t       & value);
    Stream & operator>>(int64_t       & value);
    Stream & operator>>(float         & value);
    Stream & operator>>(double        & value);
    Stream & operator>>(std::string   & value);
    Stream & operator>>(std::ostream  & value);

    /*
    // If glm ever makes it into this API again, artifact from KIT engine
    Stream & operator>>(glm::vec2     & value);
    Stream & operator>>(glm::vec3     & value);
    Stream & operator>>(glm::vec4     & value);
    Stream & operator>>(glm::uvec2    & value);
    Stream & operator>>(glm::uvec3    & value);
    Stream & operator>>(glm::uvec4    & value);
    Stream & operator>>(glm::ivec2    & value);
    Stream & operator>>(glm::ivec3    & value);
    Stream & operator>>(glm::ivec4    & value);
    Stream & operator>>(glm::quat     & value);
    Stream & operator>>(glm::mat3     & value);
    Stream & operator>>(glm::mat4     & value);
    */

  private:
    StreamFlags m_flags = SF_Default;

    /** The read cursor. NOTICE: This cursor has nothing to do with writes. Writes are always written to end of stream. */
    uint64_t m_cursor = 0;

    std::vector<uint8_t> m_data;
  };

}