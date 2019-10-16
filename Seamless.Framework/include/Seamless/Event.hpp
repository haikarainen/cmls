
#pragma once

#include <Seamless/Export.hpp>
#include <Seamless/Error.hpp>
#include <Seamless/SharedIdentity.hpp>

#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>

namespace cmls
{
  template<typename... Args>
  class Trigger
  {
  public:
    Trigger()
    {

    }

    virtual void execute(Args... args) const
    {
      LogNotice("Non-implemented trigger fired");
    }

    virtual bool operator==(Trigger<Args...> const & rhs) const
    {
      LogNotice("Non-implemented trigger checked for equality");
      return false;
    }

    virtual bool operator!=(Trigger<Args...> const & rhs) const
    {
      return !(*this == rhs);
    }

  protected:
  };

  template<typename... Args>
  class Function : public Trigger<Args...>
  {
    using Declaration = void(*)(Args...);
  public:

    Function(Declaration decl) : m_declaration(decl)
    {

    }

    virtual void execute(Args... args) const override
    {
      (m_declaration)(args...);
    }

    virtual bool operator==(Trigger<Args...> const & rhs) const override
    {
      Function<Args...> const *asFunction = dynamic_cast<Function<Args...> const *>(&rhs);
      if (!asFunction)
      {
        return false;
      }

      return m_declaration == asFunction->m_declaration;
    }

  protected:
    Declaration m_declaration = nullptr;
  };

  template<typename Type, typename... Args>
  class MemberFunction : public Trigger<Args...>
  {
    using Declaration = void(Type::*)(Args...);

  public:

    explicit MemberFunction(Type *target, Declaration decl) : m_target(target), m_declaration(decl)
    {

    }

    virtual void execute(Args... args) const override
    {
      (m_target->*m_declaration)(args...);
    }


    virtual bool operator==(Trigger<Args...> const & rhs) const override
    {
      auto asMemberFunction = dynamic_cast<MemberFunction<Type, Args...> const *>(&rhs);
      if (!asMemberFunction)
      {
        return false;
      }

      return m_declaration == asMemberFunction->m_declaration &&
        m_target == asMemberFunction->m_target;
    }
       
  protected:
    Type *m_target = nullptr;
    Declaration m_declaration = nullptr;
  };

  template<typename LambdaType, typename... Args>
  class LambdaFunction : public Trigger<Args...>, public SharedIdentity
  {
  public:

    using ThisType = LambdaFunction<LambdaType, Args...>;

    LambdaFunction(LambdaType callable) : m_callable(callable)
    {
    }

    virtual void execute(Args... args) const override
    {
      (m_callable)(args...);
    }

    virtual bool operator==(Trigger<Args...> const & rhs) const
    {
      auto asLambda = dynamic_cast<ThisType const*>(&rhs);
      if (!asLambda)
      {
        return false;
      }

      return getId() == asLambda->getId();
    }

  protected:
    LambdaType m_callable;
  };


  template <typename... Args>
  class Event
  {
  public:
    using Declaration = void(*)(Args...);
    using TriggerType = Trigger<Args...>;
    using TriggerPtr = std::shared_ptr<TriggerType>;
    Event()
    {
      m_mutex = new std::recursive_mutex();
    }

    ~Event()
    {
      clear();
      delete m_mutex;
    }
    
    template<typename LambdaType>
    void operator+=(LambdaFunction<LambdaType, Args...> const & trigger)
    {
      if (bound(&trigger))
      {
        return;
      }

      std::scoped_lock lock(*m_mutex);
      auto newPtr = std::make_shared<LambdaFunction<LambdaType, Args...>>(trigger);
      auto upCast = std::static_pointer_cast<TriggerType>(newPtr);
      m_triggers.push_back(upCast);
    }

    template<typename LambdaType>
    void operator+=(LambdaFunction<LambdaType, Args...> const && trigger)
    {
      if (bound(&trigger))
      {
        return;
      }

      std::scoped_lock lock(*m_mutex);
      auto newPtr = std::make_shared<LambdaFunction<LambdaType, Args...>>(trigger);
      auto upCast = std::static_pointer_cast<TriggerType>(newPtr);
      m_triggers.push_back(upCast);
    }

    template<typename TargetType>
    void operator+=(MemberFunction<TargetType, Args...> const && trigger)
    {
      if (bound(&trigger))
      {
        return;
      }

      std::scoped_lock lock(*m_mutex);
      auto newPtr = std::make_shared<MemberFunction<TargetType, Args...>>(trigger);
      auto upCast = std::static_pointer_cast<TriggerType>(newPtr);
      m_triggers.push_back(upCast);
    }

    void operator+=(Declaration const && trigger)
    {
      *this += Function<Args...>(trigger);
    }

    void operator+=(Function<Args...> const && trigger)
    {
      if (bound(&trigger))
      {
        return;
      }
      std::scoped_lock lock(*m_mutex);
      auto newPtr = std::make_shared<Function<Args...>>(trigger);
      auto upCast = std::static_pointer_cast<TriggerType>(newPtr);
      m_triggers.push_back(upCast);
    }

    template<typename LambdaType>
    void operator+=(LambdaType const && lambdaTrigger)
    {
      std::scoped_lock lock(*m_mutex);
      auto newPtr = std::make_shared<LambdaFunction<LambdaType, Args...>>(lambdaTrigger);
      auto upCast = std::static_pointer_cast<TriggerType>(newPtr);
      m_triggers.push_back(upCast);
    }


    template<typename TargetType>
    void operator-=(MemberFunction<TargetType, Args...> const && trigger)
    {
      if (!bound(&trigger))
      {
        return;
      }

      auto newPtr = std::make_shared<MemberFunction<TargetType, Args...>>(trigger);
      auto upCast = std::static_pointer_cast<TriggerType>(newPtr);
      
      *this -= *upCast;
    }

    void operator-=(Function<Args...> const && trigger)
    {
      if (!bound(&trigger))
      {
        return;
      }

      auto newPtr = std::make_shared<Function<Args...>>(trigger);
      auto upCast = std::static_pointer_cast<TriggerType>(newPtr);
      
      *this -= *upCast;
    }

    void operator-=(TriggerType const & trigger)
    {
      std::scoped_lock lock(*m_mutex);

      if (!bound(&trigger))
      {
        return;
      }

      std::vector<TriggerPtr> newTriggers;
      for (auto t : m_triggers)
      {
        if (*t != trigger)
        {
          newTriggers.push_back(t);
        }
      }

      newTriggers.swap(m_triggers);
    }

    void operator()(Args... args) const
    {
      execute(args...);
    }


    void execute(Args... args) const
    {
      if (!bound())
      {
        return;
      }

      std::scoped_lock lock(*m_mutex);
      std::vector<TriggerPtr> temp = m_triggers;
      
      for (auto &trigger : temp)
      {
        trigger->execute(args...);
      }
    }

    void clear()
    {
      std::scoped_lock lock(*m_mutex);
      m_triggers.clear();
    }

    bool bound() const
    {
      return m_triggers.size() > 0;
    }

    bool bound(TriggerPtr ptr) const
    {
      std::scoped_lock lock(*m_mutex);
      for (auto t : m_triggers)
      {
        if (ptr && *t == *ptr)
        {
          return true;
        }
      }

      return false;
    }

    bool bound(TriggerType const * rhs) const
    {
      std::scoped_lock lock(*m_mutex);
      for (auto t : m_triggers)
      {
        if (rhs && *t == *rhs)
        {
          return true;
        }
      }

      return false;
    }

  protected:
    std::vector<TriggerPtr> m_triggers;
    std::recursive_mutex * m_mutex = nullptr;
  };

}