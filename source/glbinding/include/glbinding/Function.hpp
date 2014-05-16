#pragma once

#include <glbinding/Function.h>
#include <glbinding/Value.h>

#include <utility>
#include <functional>

namespace {

template <typename ReturnType, typename... Arguments>
struct FunctionHelper
{
    ReturnType call(gl::Function<ReturnType, Arguments...> & function, Arguments... arguments) const
    {
        function.before();
        ReturnType value = reinterpret_cast<typename gl::Function<ReturnType, Arguments...>::Signature>(function.address())(std::forward<Arguments>(arguments)...);
        function.after();
        return value;
    }

    ReturnType callVerbose(gl::Function<ReturnType, Arguments...> & function, Arguments... arguments) const
    {
        function.before();
        function.parameters(gl::createValues(std::forward<Arguments>(arguments)...));
        ReturnType value = reinterpret_cast<typename gl::Function<ReturnType, Arguments...>::Signature>(function.address())(std::forward<Arguments>(arguments)...);
        function.returnValue(gl::createValue(value));
        function.after();
        return value;
    }
};

template <typename... Arguments>
struct FunctionHelper<void, Arguments...>
{
    void call(gl::Function<void, Arguments...> & function, Arguments... arguments) const
    {
        function.before();
        reinterpret_cast<typename gl::Function<void, Arguments...>::Signature>(function.address())(std::forward<Arguments>(arguments)...);
        function.after();
    }

    void callVerbose(gl::Function<void, Arguments...> & function, Arguments... arguments) const
    {
        function.before();
        function.parameters(gl::createValues(std::forward<Arguments>(arguments)...));
        reinterpret_cast<typename gl::Function<void, Arguments...>::Signature>(function.address())(std::forward<Arguments>(arguments)...);
        function.returnValue(nullptr);
        function.after();
    }
};

}

namespace gl {

template <typename ReturnType, typename... Arguments>
Function<ReturnType, Arguments...>::Function(const char * _name)
: AbstractFunction(_name)
{
}

template <typename ReturnType, typename... Arguments>
ReturnType Function<ReturnType, Arguments...>::operator()(Arguments... arguments)
{
    if(isValid())
    {
        if (!callbacksEnabled())
        {
            return reinterpret_cast<Signature>(address())(std::forward<Arguments>(arguments)...);
        }
        else
        {
            if (!verboseCallbacks())
            {
                return FunctionHelper<ReturnType, Arguments...>().call(*this, std::forward<Arguments>(arguments)...);
            }
            else
            {
                return FunctionHelper<ReturnType, Arguments...>().callVerbose(*this, std::forward<Arguments>(arguments)...);
            }
        }
    }
    else
    {
         if (callbacksEnabled())
         {
            invalid();
         }

         return ReturnType();
    }
}

} // namespace gl
