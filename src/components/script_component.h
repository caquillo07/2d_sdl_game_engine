//
// Created by Hector Mejia on 12/22/23.
//

#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include <sol/sol.hpp>

struct ScriptComponent {
    sol::function func;

    explicit ScriptComponent(sol::function func = sol::lua_nil) {
        this->func = func;
    }
};

#endif //SCRIPT_COMPONENT_H
