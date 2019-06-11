#pragma once

#include <cu/cu_macro.h>

#include <memory>

namespace chaiscript { class ChaiScript; }

namespace bp
{

class ScriptEnv
{
public:
    auto& GetChai() { return m_chai; }

private:
    std::unique_ptr<chaiscript::ChaiScript> m_chai = nullptr;
    
    CU_SINGLETON_DECLARATION(ScriptEnv)

}; // ScriptEnv

}