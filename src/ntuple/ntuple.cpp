#include "ntuple/ntuple.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>
#include <set>
#include <tuple>

namespace NTuple {

// helpers
static std::pair<int,int> d4(int s, int r, int c) {
    switch (s) {
        case 0: return {r,     c    };
        case 1: return {c,     7-r  };
        case 2: return {7-r,   7-c  };
        case 3: return {7-c,   r    };
        case 4: return {7-r,   c    };
        case 5: return {r,     7-c  };
        case 6: return {c,     r    };
        case 7: return {7-c,   7-r  };
        default: return {r, c};
    }
}

static Tuple2 norm(int a, int b) { return a < b ? Tuple2{a, b} : Tuple2{b, a}; }

// Smallest pair under all 8 D4 transforms = canonical representative.
static Tuple2 canonical(int a, int b) {
    int ra = a >> 3, ca = a & 7;
    int rb = b >> 3, cb = b & 7;
    Tuple2 best = norm(a, b);
    for (int s = 1; s < 8; ++s) {
        auto [r1, c1] = d4(s, ra, ca);
        auto [r2, c2] = d4(s, rb, cb);
        Tuple2 q = norm(r1*8+c1, r2*8+c2);
        if (std::tie(q.a, q.b) < std::tie(best.a, best.b)) best = q;
    }
    return best;
}

std::vector<TupleGroup> build_groups() {
    using Key = std::pair<int,int>;
    std::map<Key, std::set<Key>> gmap;

    // Add an adjacent pair and all 8 D4 symmetric expansions to the map.
    auto insert = [&](int r1, int c1, int r2, int c2) {
        Tuple2 p   = norm(r1*8+c1, r2*8+c2);
        Tuple2 can = canonical(p.a, p.b);
        Key key{can.a, can.b};
        int ra = p.a >> 3, ca_coord = p.a & 7;
        int rb = p.b >> 3, cb_coord = p.b & 7;
        for (int s = 0; s < 8; ++s) {
            auto [r1s, c1s] = d4(s, ra, ca_coord);
            auto [r2s, c2s] = d4(s, rb, cb_coord);
            Tuple2 q = norm(r1s*8+c1s, r2s*8+c2s);
            gmap[key].insert({q.a, q.b});
        }
    };

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            if (c+1 < 8)           insert(r, c, r,   c+1);   // horizontal
            if (r+1 < 8)           insert(r, c, r+1, c  );   // vertical
            if (r+1 < 8 && c+1<8)  insert(r, c, r+1, c+1);  // diagonal
            if (r+1 < 8 && c-1>=0) insert(r, c, r+1, c-1);  // anti-diagonal
        }

    std::vector<TupleGroup> groups;
    groups.reserve(gmap.size());
    for (auto& [key, syms] : gmap) {
        TupleGroup g;
        g.canonical = {key.first, key.second};
        g.syms.reserve(syms.size());
        for (auto& [a, b] : syms) g.syms.push_back({a, b});
        groups.push_back(std::move(g));
    }
    // Sorted by canonical (map iterates in key order) – deterministic.
    assert(groups.size() == 32 && "all-2 must yield exactly 32 canonical groups");
    return groups;
}

// Public APIs
const std::vector<TupleGroup>& all2_groups() {
    static const std::vector<TupleGroup> groups = build_groups();
    return groups;
}

int Net::cell_val(uint64_t my_bb, uint64_t opp_bb, int pos) {
    const uint64_t bit = 1ULL << pos;
    if (my_bb  & bit) return 2;   // current player → treated as black
    if (opp_bb & bit) return 0;   // opponent → treated as white
    return 1;                      // empty
}

float Net::eval_bbs(uint64_t my_bb, uint64_t opp_bb) const {
    const auto& groups = all2_groups();
    float sum = 0.f;
    for (int g = 0; g < N_GROUPS; ++g)
        for (auto& [a, b] : groups[g].syms)
            sum += w[g][lut_idx(cell_val(my_bb, opp_bb, a),
                                cell_val(my_bb, opp_bb, b))];
    return sum;
}

float Net::evaluate(const Board& board, Player player) const {
    // Board inversion: pass current player's bits as "my" (black-side).
    return eval_bbs(board.discs(player), board.discs(opponent(player)));
}

Net::Grad Net::grad_bbs(uint64_t my_bb, uint64_t opp_bb) const {
    const auto& groups = all2_groups();
    Grad g{};
    for (int i = 0; i < N_GROUPS; ++i)
        for (auto& [a, b] : groups[i].syms)
            g[i][lut_idx(cell_val(my_bb, opp_bb, a),
                         cell_val(my_bb, opp_bb, b))] += 1.f;
    return g;
}

Net::Grad Net::gradient(const Board& board, Player player) const {
    return grad_bbs(board.discs(player), board.discs(opponent(player)));
}

void Net::update(const Grad& grad, float delta, float alpha) {
    const float step = alpha * delta;
    for (int g = 0; g < N_GROUPS; ++g)
        for (int j = 0; j < LUT_SIZE; ++j)
            w[g][j] += step * grad[g][j];
}

bool Net::save(const std::string& path) const {
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f.write(reinterpret_cast<const char*>(w.data()), sizeof(w));
    return f.good();
}

bool Net::load(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    f.read(reinterpret_cast<char*>(w.data()), sizeof(w));
    return f.good();
}

} // namespace NTuple
