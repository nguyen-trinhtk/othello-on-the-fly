#pragma once

#include "eval/eval.h"

#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

struct EvalEntry {
    std::string_view name;
    std::string_view label;
    std::function<std::unique_ptr<IEvaluator>()> make;
};

// Generated at build time from include/eval/*_eval.h
#include "eval_registry.inc"

inline std::optional<std::size_t> find_eval(std::string_view name)
{
    const auto& entries = eval_registry();
    for (std::size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].name == name)
            return i;
    }
    return std::nullopt;
}

inline std::unique_ptr<IEvaluator> make_evaluator(std::string_view name)
{
    const auto index = find_eval(name);
    if (!index)
        return nullptr;
    return eval_registry()[*index].make();
}
