#pragma once
#include "board.h"
#include "types.h"

class IEvaluator {
    public:
        IEvaluator() = default; // TODO or other pattern
        virtual int evaluate(const Board& board, Player player) = 0; // pure virtual function
        virtual ~IEvaluator() = default;
};