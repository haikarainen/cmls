
#include "Seamless/Application.hpp"

#include "Seamless/Filesystem.hpp"
#include "Seamless/Error.hpp"
#include "Seamless/Http.hpp"
#include "Seamless/Application/Controller.hpp"
#include "Seamless/Stream.hpp"

cmls::Application::Application(std::string const& root, std::string const &site)
{
  cmls::Log::registerHandler(this);

  m_root = cmls::Directory(root).fullPath();
  m_site = site;
  m_viewManager = new cmls::ViewManager(this);
  m_configurationManager = new cmls::ConfigurationManager(this);
}

cmls::Application::~Application()
{
  for (auto c : m_controllers)
  {
    delete c.second;
  }

  delete m_viewManager;
  if (m_logHandle)
  {
    m_logHandle.close();
  }

  cmls::Log::unregisterHandler(this);
}

bool cmls::Application::initialize()
{
  if (!m_configurationManager->resolve())
  {
    LogError("Failed to resolve configuration");
    return false;
  }
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
  m_logHandle.flush();
}

std::string const& cmls::Application::log() const
{
  return m_log;
}

void cmls::Application::route(cmls::HttpMethod method, std::string const& patternText, cmls::Controller* controller)
{
  m_routes.push_back({patternText, method, controller});
}

cmls::Controller* cmls::Application::controller(std::string const& classId)
{
  auto finder = m_controllers.find(classId);
  if (finder == m_controllers.end())
  {
    return nullptr;
  }

  return finder->second;
}

void cmls::Application::controller(std::string const& classId, cmls::Controller* controllerInstance)
{
  auto finder = m_controllers.find(classId);
  if (finder != m_controllers.end())
  {
    return;
  }

  m_controllers[classId] = controllerInstance;
}

void cmls::Application::staticRoute(std::vector<std::string> const& exts, std::string const& mimeType)
{
  for (auto e : exts)
  {
    m_staticServes[e] = mimeType;
  }
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
  // First attempt to find a route
  for (auto const &r : m_routes)
  {
    uint16_t routeMethod = r.method;
    uint16_t requestMethod = request.method();

    bool methodMatches = (routeMethod & requestMethod) > 0;

    auto pathStr = request.path().toString();

    auto result = r.pattern.evaluate(pathStr);
    if(result.matches() && methodMatches)
    {
      r.controller->handleRequest(request, response);

      return true;
    }
  }

  // Then, try to match any of the static file servings
  auto path = request.path();
  if (!path.isFile())
  {
    return false;
  }

  auto servPath = sitePath() + "\\static\\" + path.toString();
  auto servFile = cmls::File(servPath);
  auto fileExt = servFile.extension();
  if(!servFile.exist())
  {
    LogNotice("File doesnt exist (%s)", servFile.fullPath().c_str());
    return false;
  }

  auto staticFind = m_staticServes.find(fileExt);
  if (staticFind == m_staticServes.end())
  {
    LogNotice("Not a static route");
    return false;
  }

  cmls::Stream fileData;
  if (!fileData.readFile(servFile.fullPath()))
  {
    LogNotice("Failed to read file (%s)", servFile.fullPath().c_str());
    return false;
  }

  auto acceptEncoding = cmls::split(request.header("Accept-Encoding"), {','});
  for (auto e : acceptEncoding)
  {
    if (cmls::trim(e) == "deflate")
    {
      cmls::Stream deflateStream;
      if (fileData.compress(deflateStream))
      {
        fileData.swap(deflateStream);
        response.header("Content-Encoding", "deflate");
        LogNotice("Deflate response");
      }
      break;
    }
  }

  response.header("Content-Type", staticFind->second);
  response << fileData;
  


  return true;
}