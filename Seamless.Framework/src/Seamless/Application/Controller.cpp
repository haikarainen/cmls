
#include "Seamless/Application/Controller.hpp"

cmls::Controller::Controller(cmls::Application * const application)
  : m_application(application)
{

}

cmls::Application *cmls::Controller::application() const
{
  return m_application;
}
