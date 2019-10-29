
#include "Seamless/Filesystem.hpp"

#include "Seamless/Error.hpp"
#include "Seamless/Stream.hpp"
#include "Seamless/Data.hpp"

#include <filesystem>

cmls::FilesystemEntry::FilesystemEntry(std::string const & path)
{
  std::error_code ec(0, std::system_category());
  auto p = std::filesystem::absolute(path, ec);
  if (ec.value() == 0)
  {
    m_path = p.string();
  }
  else
  {
    m_path = path;
  }
}

cmls::FilesystemEntry::~FilesystemEntry()
{

}

std::string cmls::FilesystemEntry::name()
{
  return std::filesystem::path(m_path).filename().string();
}

std::string cmls::FilesystemEntry::fullPath()
{
  return m_path;
}

std::string cmls::FilesystemEntry::path()
{
  return std::filesystem::path(m_path).parent_path().string();
}

std::string cmls::FilesystemEntry::relativePath(std::string relative)
{
  return std::filesystem::relative(relative, m_path).string();
}

cmls::Directory::Directory(std::string const & path)
  : FilesystemEntry(path)
{

}

cmls::Directory::~Directory()
{

}

bool cmls::Directory::create() const
{
  std::error_code ec;
  std::filesystem::create_directories(m_path, ec);
  if (ec.value() != 0)
  {
    LogError("std::filesystem::create_directories failed (%u)", ec.value());
    return false;
  }

  return true;
}

bool cmls::Directory::remove() const
{
  LogWarning("Untested code. Refuse to continue");
  return false;
  std::error_code ec;
  std::filesystem::remove_all(m_path, ec);
  if (ec.value() != 0)
  {
    LogError("std::filesystem::remove_all failed (%u)", ec.value());
    return false;
  }

  return true;
}

bool cmls::Directory::exist() const
{
  return std::filesystem::is_directory(m_path);
}

bool cmls::Directory::iterate(bool recursive, std::function<void(FilesystemEntry *entry)> func) const
{
  if (!cmls::Directory(m_path).exist())
  {
    LogError("directory does not exist");
    return false;
  }

  if (recursive)
  {
    for (auto& p : std::filesystem::recursive_directory_iterator(m_path))
    {
      if (p.is_regular_file())
      {
        cmls::File f(p.path().string());
        func(&f);
      }
      else if (p.is_directory())
      {
        cmls::Directory d(p.path().string());
        func(&d);
      }
    }
  }
  else
  {
    for (auto& p : std::filesystem::directory_iterator(m_path))
    {
      if (p.is_regular_file())
      {
        cmls::File f(p.path().string());
        func(&f);
      }
      else if (p.is_directory())
      {
        cmls::Directory d(p.path().string());
        func(&d);
      }
    }
  }


  return true;
}

cmls::File::File(std::string const & path)
  : FilesystemEntry(path)
{

}

cmls::File::~File()
{

}

bool cmls::File::createPath() const
{
  return cmls::Directory(std::filesystem::path(m_path).parent_path().string()).create();
}

bool cmls::File::remove() const
{
  LogWarning("Untested code. Refuse to continue");
  return false;
  std::error_code ec;
  std::filesystem::remove(m_path, ec);
  if (ec.value() != 0)
  {
    LogError("std::filesystem::remove failed (%u)", ec.value());
    return false;
  }

  return true;
}

std::string cmls::File::basename() const
{
  return std::filesystem::path(m_path).stem().string();
}

std::string cmls::File::extension() const
{
  return cmls::trimLeft(std::filesystem::path(m_path).extension().string(), {"."});
}

cmls::File &cmls::File::extension(std::string const& newExtension)
{
  m_path = std::filesystem::path(m_path).replace_extension(newExtension).string();

  return *this;
}

uint64_t cmls::File::size() const
{
  std::error_code ec;
  uint64_t returner = std::filesystem::file_size(m_path, ec);
  if(ec.value() == 0)
  {
    return returner;
  }
  else
  {
    LogError("std::filesystem::file_size failed (%u)", ec.value());
    return 0;
  }
}

bool cmls::File::exist() const
{
  return std::filesystem::is_regular_file(m_path);
}

uint16_t cmls::File::crc16() const
{
  cmls::Stream stream;
  if (!stream.readFile(m_path))
  {
    LogError("could not read file");
    return 0;
  }

  return cmls::crc16(stream.begin(), stream.size());
}

uint64_t cmls::File::lastWriteTime() const
{
  return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(m_path).time_since_epoch()).count();
}

bool cmls::File::writeString(std::string const& inData)
{
  std::ofstream handle(m_path);
  if (!handle.is_open())
  {
    LogError("open failed");
    return false;
  }

  handle.write(inData.data(), inData.size());

  if (handle.fail())
  {
    LogError("write failed");
    return false;
  }

  handle.close();

  return true;
}

bool cmls::File::readString(std::string& outData)
{
  std::ifstream handle(m_path);
  if (!handle.is_open())
  {
    LogError("open failed");
    return false;
  }

  std::string contents((std::istreambuf_iterator<char>(handle)), std::istreambuf_iterator<char>());

  if (handle.fail())
  {
    LogError("read failed");
    return false;
  }

  handle.close();

  outData = contents;

  return true;
}
