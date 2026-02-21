#pragma once
#include "Util.hpp"
#include <memory>
#include <vector>

class Block;

class Map {
public:
    Map() = default;
    Map(int w, int h);

    void resize(int w, int h);

    int width() const { return m_w; }
    int height() const { return m_h; }

    Block* blockAt(int x, int y) const;
    void setBlock(int x, int y, std::unique_ptr<Block> b);
    void clearBlock(int x, int y);

    bool isSolid(int x, int y) const;
    bool isWalkable(int x, int y) const;

    void generateRandom(unsigned seed, float brickChance);

    IVec2 playerSpawn() const { return {1,1}; }

private:
    int m_w = 0, m_h = 0;
    std::vector<std::unique_ptr<Block>> m_blocks;

    int idx(int x, int y) const { return y * m_w + x; }

    void placeFixedWalls();
    void randomFillBricks(Rng& rng, float brickChance);
    void clearSpawnZone(const IVec2& s);
    bool isMapAcceptable(float reachableRatioNeeded) const;
};
