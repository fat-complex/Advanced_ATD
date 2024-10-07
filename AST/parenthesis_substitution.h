#pragma once

#include <string>

class ParenthesisSubstitution
{
public:
    static auto perform(const std::string& expression) -> std::string;
};