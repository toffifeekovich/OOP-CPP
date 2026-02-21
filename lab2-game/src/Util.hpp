#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <queue>
#include <vector>
#include <cmath>

struct IVec2 {
    int x = 0;
    int y = 0;
    bool operator==(const IVec2& other) const { return x == other.x && y == other.y; }
};

inline int manhattan(const IVec2& a, const IVec2& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

class Rng {
public:
    explicit Rng(unsigned seed) : eng(seed) {}
    int rangeInt(int lo, int hi) {
        std::uniform_int_distribution<int> d(lo, hi); 
        return d(eng); 
    }
    bool chance(float p) { 
        std::bernoulli_distribution d(p); 
        return d(eng); 
    }
    
private:
    std::mt19937 eng;
};

inline bool inBounds(int x, int y, int w, int h) {
    return x >= 0 && y >= 0 && x < w && y < h;
}

inline std::vector<IVec2> neighbors4(const IVec2& p) {
    return { {p.x+1,p.y}, {p.x-1,p.y}, {p.x,p.y+1}, {p.x,p.y-1} };
}
