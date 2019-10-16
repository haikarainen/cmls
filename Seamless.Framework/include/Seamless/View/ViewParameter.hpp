
#pragma once

#include <Seamless/Export.hpp>

#include <string>
#include <map>
#include <vector>

#include <any>
#include <optional>

namespace cmls
{
  using ViewParameters = std::map<std::string, std::any>;

  class CMLS_API ViewParameterList
  {
  public:

    ViewParameterList(ViewParameters &parameters);

    /** Sets the given parameter */
    template<typename ParameterType>
    void set(std::string const& name, ParameterType parameter)
    {
      m_parameters[name] = parameter;
    }

    /** Gets the given parameter, or nullptr if it doesnt exist */
    template<typename ParameterType>
    std::optional<ParameterType> get(std::string const& name) const
    {
      auto finder = m_parameters.find(name);
      if (finder == m_parameters.end())
      {
        return std::optional<ParameterType>();
      }

      try
      {
        return std::any_cast<ParameterType>(finder->second);
      }
      catch (std::bad_any_cast const &e)
      {
        return std::optional<ParameterType>();
      }
    }

  protected:
    std::map<std::string, std::any> &m_parameters;
  };

}