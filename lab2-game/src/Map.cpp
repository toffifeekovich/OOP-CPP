#include "Map.hpp"
#include "Objects.hpp"
#include <queue>

Map::Map(int w, int h) { resize(w, h); }

void Map::resize(int w, int h) {
    m_w = w; m_h = h;
    m_blocks.clear();
    m_blocks.resize((size_t)m_w * (size_t)m_h);
}

Block* Map::blockAt(int x, int y) const {
    if (!inBounds(x,y,m_w,m_h)) return nullptr;
    return m_blocks[idx(x,y)].get();
}

void Map::setBlock(int x, int y, std::unique_ptr<Block> b) {
    if (!inBounds(x,y,m_w,m_h)) return;
    m_blocks[idx(x,y)] = std::move(b);
}

void Map::clearBlock(int x, int y) {
    if (!inBounds(x,y,m_w,m_h)) return;
    m_blocks[idx(x,y)].reset();
}

bool Map::isSolid(int x, int y) const {
    auto* b = blockAt(x,y);
    return b && b->isSolid();
}

bool Map::isWalkable(int x, int y) const {
    return inBounds(x,y,m_w,m_h) && !isSolid(x,y);
}

void Map::placeFixedWalls() {
    for (int x=0; x<m_w; ++x) { setBlock(x,0,std::make_unique<WallBlock>()); setBlock(x,m_h-1,std::make_unique<WallBlock>()); }
    for (int y=0; y<m_h; ++y) { setBlock(0,y,std::make_unique<WallBlock>()); setBlock(m_w-1,y,std::make_unique<WallBlock>()); }

    for (int y=2; y<m_h-2; y+=2) {
        for (int x=2; x<m_w-2; x+=2) {
            setBlock(x,y,std::make_unique<WallBlock>());
        }
    }
}

void Map::randomFillBricks(Rng& rng, float brickChance) {
    for (int y=1; y<m_h-1; ++y) {
        for (int x=1; x<m_w-1; ++x) {
            if (blockAt(x,y)) continue;
            if (rng.chance(brickChance)) {
                setBlock(x,y,std::make_unique<BrickBlock>());
            }
        }
    }
}

void Map::clearSpawnZone(const IVec2& s) {
    clearBlock(s.x, s.y);
    clearBlock(s.x+1, s.y);
    clearBlock(s.x, s.y+1);
    clearBlock(s.x+1, s.y+1);
}

bool Map::isMapAcceptable(float reachableRatioNeeded) const {
    const IVec2 start = playerSpawn();
    if (!isWalkable(start.x, start.y)) return false;

    std::vector<char> vis((size_t)m_w*(size_t)m_h, 0);
    std::queue<IVec2> q;
    q.push(start);
    vis[idx(start.x,start.y)] = 1;

    int reachable = 0;
    while (!q.empty()) {
        auto p = q.front(); 
        q.pop();
        reachable++;
        for (auto n : neighbors4(p)) {
            if (!inBounds(n.x,n.y,m_w,m_h)) continue;
            if (vis[idx(n.x,n.y)]) continue;
            if (!isWalkable(n.x,n.y)) continue;
            vis[idx(n.x,n.y)] = 1;
            q.push(n);
        }
    }

    return reachable >= (int)((m_w*m_h) * reachableRatioNeeded);
}

void Map::generateRandom(unsigned seed, float brickChance) {
    Rng rng(seed);

    if (brickChance < 0.35f) brickChance = 0.35f;
    if (brickChance > 0.65f) brickChance = 0.65f;

    const float needed = 0.22f;

    for (int attempt=0; attempt<200; ++attempt) {
        for (auto& b : m_blocks) b.reset();

        placeFixedWalls();
        randomFillBricks(rng, brickChance);

        clearSpawnZone(playerSpawn());
        clearSpawnZone({m_w-3, 1});
        clearSpawnZone({1, m_h-3});
        clearSpawnZone({m_w-3, m_h-3});

        if (isMapAcceptable(needed)) return;

        (void)rng.rangeInt(0, 1'000'000);
    }
}
