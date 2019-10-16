
#include "Seamless/Application.hpp"

#include "Seamless/Filesystem.hpp"
#include "Seamless/Error.hpp"

cmls::Application::Application(std::string const& root, std::string const &site)
{
  cmls::Log::registerHandler(this);

  m_root = cmls::Directory(root).fullPath();
  m_site = site;
  m_viewManager = new cmls::ViewManager(this);
}

cmls::Application::~Application()
{
  delete m_viewManager;
  cmls::Log::unregisterHandler(this);
}

bool cmls::Application::initialize()
{
  return true;
}

std::string const& cmls::Application::site() const
{
  return m_site;
}

std::string cmls::Application::sitePath() const
{
  return m_root + "\\sites\\" + m_site;
}

cmls::ViewManager* cmls::Application::viewManager() const
{
  return m_viewManager;
}

void cmls::Application::writeLine(std::string const& line)
{
  auto linen = line + "\n";
  m_log += linen;
  if (!m_logHandle)
  {
    m_logHandle.open(root() + "\\cmls.Application.log");
  }

  if (!m_logHandle)
  {
    return;
  }

  m_logHandle.write(linen.c_str(), linen.size());
}

std::string const& cmls::Application::log() const
{
  return m_log;
}

bool cmls::Application::loadConfiguration()
{
  std::string confPath = sitePath() + "\\site.conf";
  cmls::File confFile(confPath);
  if (!confFile.exist())
  {
    LogError("Configuration file for site %s does not exist", site().c_str());
    return false;
  }

  std::string conf;
  if (!confFile.readString(conf))
  {
    LogError("Failed to read configuration file for site %s", site().c_str());
    return false;
  }

  return true;
}

std::string const& cmls::Application::root() const
{
  return m_root;
}


bool cmls::Application::handleRequest(cmls::Request const &request, cmls::Response &response)
{
  // @todo, route and invoke controllers
  return true;
}