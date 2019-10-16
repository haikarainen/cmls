
#include "Seamless/CompilerCommon/Module.hpp"
#include "Seamless/Filesystem.hpp"

#include "Seamless/Error.hpp"

std::map<std::string, cmls::Module> cmls::Module::allModules(std::string const& rootPath)
{
  cmls::Directory rootDir(rootPath);
  cmls::Directory modulesDir(rootDir.fullPath() + "\\modules\\");

  if (!modulesDir.exist())
  {
    return {};
  }

  std::map<std::string, cmls::Module> returner;
  modulesDir.iterate(false, [&](auto f){
    auto dir = dynamic_cast<cmls::Directory*>(f);
    if (dir)
    {
      returner[dir->name()] = cmls::Module(dir->fullPath());
    }
  });

  return returner;
}

cmls::Module::Module()
{
}

cmls::Module::Module(std::string const& path)
{
  m_path = cmls::Directory(path).fullPath();
  parseConfiguration();
}

cmls::Module::~Module()
{
}

std::vector<std::string> cmls::Module::includes() const
{
  return m_implicitIncludes;
}

std::vector<std::string> cmls::Module::defines() const
{
  return m_defines;
}

std::vector<std::string> cmls::Module::links() const
{
  return m_links;
}

std::vector<std::string> cmls::Module::includePaths() const
{
  auto includes = m_additionalIncludePaths;
  includes.push_back(implicitIncludePath());

  return includes;
}

std::vector<std::string> cmls::Module::linkerPaths() const
{
  auto linkpaths = m_additionalLinkerPaths;
  linkpaths.push_back(implicitLinkerPath());

  return linkpaths;
}

void cmls::Module::parseConfiguration()
{
  auto conf = cmls::File(m_path + "\\module.conf");
  if (!conf.exist())
  {
    return;
  }

  std::string data;
  if (!conf.readString(data))
  {
    LogWarning("Failed to read configuration for module %s", cmls::Directory(m_path).name().c_str());
    return;
  }

  auto lines = cmls::split(data, { '\n' });
  for (auto line : lines)
  {
    line = cmls::trim(line);

    auto args = cmls::parseArguments(line, true);
    if (args.size() == 0)
    {
      continue;
    }

    auto cmd = cmls::strToLower(args[0]);

    if (cmd == "link")
      for (uint32_t i = 1; i < args.size(); i++)
        m_links.push_back(args[i]);

    if (cmd == "define")
      for (uint32_t i = 1; i < args.size(); i++)
        m_defines.push_back(args[i]);

    if (cmd == "includepath")
      for (uint32_t i = 1; i < args.size(); i++)
        m_additionalIncludePaths.push_back(args[i]);

    if (cmd == "linkerpath")
      for (uint32_t i = 1; i < args.size(); i++)
        m_additionalLinkerPaths.push_back(args[i]);

    if (cmd == "include")
      for (uint32_t i = 1; i < args.size(); i++)
        m_implicitIncludes.push_back(args[i]);

  }
}

std::string cmls::Module::implicitIncludePath() const
{
  return m_path + "\\include\\";
}

std::string cmls::Module::implicitLinkerPath() const
{
  return m_path + "\\lib\\";
}
