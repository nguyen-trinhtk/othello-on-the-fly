#pragma once

#include "eval_registry.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

namespace bench {

inline int parse_pos_int(const char* s, const char* name, int fallback)
{
    if (!s || !*s)
        return fallback;
    char* end = nullptr;
    const long v = std::strtol(s, &end, 10);
    if (end == s || *end != '\0' || v <= 0) {
        std::cerr << "Invalid " << name << "; using default " << fallback << ".\n";
        return fallback;
    }
    return static_cast<int>(v);
}

inline std::string eval_choices()
{
    std::ostringstream out;
    const auto& entries = eval_registry();
    for (std::size_t i = 0; i < entries.size(); ++i) {
        if (i > 0)
            out << " | ";
        out << entries[i].name;
    }
    return out.str();
}

inline std::optional<bool> parse_parallel(const char* name)
{
    if (std::strcmp(name, "none") == 0)
        return false;
    if (std::strcmp(name, "thread-pool") == 0)
        return true;
    return std::nullopt;
}

} // namespace bench
