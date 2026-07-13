#pragma once
#include "eval/eval.h"
#include "ntuple/ntuple.h"
#include <cmath>
#include <string>

class NtupleEvaluator : public IEvaluator {
public:
    explicit NtupleEvaluator(const std::string& path = "ntuple.bin") {
        m_net.load(path);
    }

    int evaluate(const Board& board, Player player) override {
        return static_cast<int>(std::round(m_net.evaluate(board, player)));
    }

    // Expose the underlying net so training tools can update and re-save it.
    NTuple::Net& net() { return m_net; }
    const NTuple::Net& net() const { return m_net; }

private:
    NTuple::Net m_net;
};
