
#pragma once

#include <Seamless/Export.hpp>

#include <functional>
#include <string>
#include <cstdint>

namespace cmls
{
  class CMLS_API FilesystemEntry
  {
  public:
    FilesystemEntry(std::string const & path);
    virtual ~FilesystemEntry();

    std::string name();

    std::string fullPath();

    std::string path();

    bool valid() const
    {
      return m_valid;
    }

    std::string relativePath(std::string relative);

  protected:
    std::string m_path;
    bool m_valid = false;
  };

  class CMLS_API Directory : public FilesystemEntry
  {
  public:
    Directory(std::string const & path);
    virtual ~Directory();

    bool create() const;

    bool remove() const;

    bool exist() const;

    bool iterate(bool recursive, std::function<void(FilesystemEntry *entry)>) const;

  };

  class CMLS_API File : public FilesystemEntry
  {
  public:
    File(std::string const & path);
    virtual ~File();

    bool createPath() const;

    bool remove() const;

    std::string basename() const;

    std::string extension() const;

    cmls::File &extension(std::string const& newExtension);

    uint64_t size() const;

    bool exist() const;

    uint16_t crc16() const;

    uint64_t lastWriteTime() const;

    bool writeString(std::string const &inData);
    bool readString(std::string &outData);

  };
}
