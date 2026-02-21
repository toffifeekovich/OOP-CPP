#include "Objects.hpp"
#include "GameModel.hpp"
#include <algorithm>

void WallBlock::draw(sf::RenderWindow& w, int tileSize, int x, int y) {
    sf::RectangleShape r(sf::Vector2f((float)tileSize, (float)tileSize));
    r.setPosition((float)(x*tileSize), (float)(y*tileSize));
    r.setFillColor(sf::Color(70,70,90));
    w.draw(r);
}

void BrickBlock::draw(sf::RenderWindow& w, int tileSize, int x, int y) {
    sf::RectangleShape r(sf::Vector2f((float)tileSize, (float)tileSize));
    r.setPosition((float)(x*tileSize), (float)(y*tileSize));
    r.setFillColor(sf::Color(155,110,70));
    w.draw(r);

    sf::RectangleShape inner(sf::Vector2f((float)tileSize-10, (float)tileSize-10));
    inner.setPosition((float)(x*tileSize+5), (float)(y*tileSize+5));
    inner.setFillColor(sf::Color(130,90,60));
    w.draw(inner);
}

void Player::update(GameModel& model, float dt) {
    m_moveTimer += dt;

    if (m_moveTimer >= m_moveCooldown) {
        if (m_reqDx != 0 || m_reqDy != 0) {
            IVec2 next{ m_tile.x + m_reqDx, m_tile.y + m_reqDy };
            if (model.isTileFreeForMove(next)) m_tile = next;
            m_moveTimer = 0.0f;
        }
    }

    if (m_reqBomb) {
        model.tryPlaceBomb(m_tile);
        m_reqBomb = false;
    }

    m_reqDx = 0; m_reqDy = 0;
}

void Player::draw(sf::RenderWindow& w, int tileSize) const {
    sf::CircleShape c((float)tileSize * 0.38f);
    c.setPosition((float)(m_tile.x*tileSize + tileSize*0.12f),
                  (float)(m_tile.y*tileSize + tileSize*0.12f));
    c.setFillColor(sf::Color(80, 200, 120));
    w.draw(c);

    sf::CircleShape eye(5.f);
    eye.setFillColor(sf::Color::Black);
    eye.setPosition((float)(m_tile.x*tileSize + tileSize*0.52f),
                    (float)(m_tile.y*tileSize + tileSize*0.30f));
    w.draw(eye);
}


void Enemy::update(GameModel& model, float dt) {
    m_moveTimer += dt;
    if (m_moveTimer >= m_moveCooldown) {
        auto step = chooseStep(model);
        IVec2 next{ m_tile.x + step.x, m_tile.y + step.y };
        if ((step.x != 0 || step.y != 0) && model.isTileFreeForMove(next)) {
            m_tile = next;
        }
        m_moveTimer = 0.0f;
    }
}

void RandomEnemy::draw(sf::RenderWindow& w, int tileSize) const {
    sf::CircleShape c((float)tileSize * 0.34f);
    c.setPosition((float)(m_tile.x*tileSize + tileSize*0.16f),
                  (float)(m_tile.y*tileSize + tileSize*0.16f));
    c.setFillColor(sf::Color(220, 90, 90));
    w.draw(c);
}

void ChaseEnemy::draw(sf::RenderWindow& w, int tileSize) const {
    sf::RectangleShape r(sf::Vector2f((float)tileSize*0.74f, (float)tileSize*0.74f));
    r.setPosition((float)(m_tile.x*tileSize + tileSize*0.13f),
                  (float)(m_tile.y*tileSize + tileSize*0.13f));
    r.setFillColor(sf::Color(90, 90, 220));
    w.draw(r);
}

IVec2 RandomEnemy::chooseStep(GameModel& model) {
    auto& rng = model.rng();
    std::vector<IVec2> dirs = { {1,0},{-1,0},{0,1},{0,-1} };
    for (int i=0;i<6;i++) {
        int a = rng.rangeInt(0,3);
        int b = rng.rangeInt(0,3);
        std::swap(dirs[a], dirs[b]);
    }
    for (auto d : dirs) {
        IVec2 next{ m_tile.x + d.x, m_tile.y + d.y };
        if (model.isTileFreeForMove(next)) return d;
    }
    return {0,0};
}

IVec2 ChaseEnemy::chooseStep(GameModel& model) {
    auto p = model.playerTile();
    std::vector<IVec2> dirs = { {1,0},{-1,0},{0,1},{0,-1} };

    IVec2 best{0,0};
    int bestDist = 1'000'000;
    for (auto d : dirs) {
        IVec2 next{ m_tile.x + d.x, m_tile.y + d.y };
        if (!model.isTileFreeForMove(next)) continue;
        int dist = manhattan(next, p);
        if (dist < bestDist) { bestDist = dist; best = d; }
    }
    if (bestDist < 1'000'000) return best;

    auto& rng = model.rng();
    for (int i=0;i<6;i++) {
        int a = rng.rangeInt(0,3);
        int b = rng.rangeInt(0,3);
        std::swap(dirs[a], dirs[b]);
    }
    for (auto d : dirs) {
        IVec2 next{ m_tile.x + d.x, m_tile.y + d.y };
        if (model.isTileFreeForMove(next)) return d;
    }
    return {0,0};
}


Bomb::Bomb(IVec2 pos, int range, float fuseSec, int ownerId) {
    m_tile = pos;
    m_range = range;
    m_fuse = fuseSec;
    m_ownerId = ownerId;
}

void Bomb::update(GameModel& model, float dt) {
    m_timer += dt;
    if (m_timer >= m_fuse) {
        model.explodeBomb(*this);
        m_alive = false;
    }
}

void Bomb::draw(sf::RenderWindow& w, int tileSize) const {
    sf::CircleShape c((float)tileSize * 0.30f);
    c.setPosition((float)(m_tile.x*tileSize + tileSize*0.20f),
                  (float)(m_tile.y*tileSize + tileSize*0.20f));
    c.setFillColor(sf::Color(40, 40, 40));
    w.draw(c);

    sf::CircleShape spark(5.f);
    spark.setFillColor(sf::Color(250, 220, 80));
    spark.setPosition((float)(m_tile.x*tileSize + tileSize*0.65f),
                      (float)(m_tile.y*tileSize + tileSize*0.15f));
    w.draw(spark);
}


Explosion::Explosion(IVec2 pos, float lifeSec) {
    m_tile = pos;
    m_life = lifeSec;
}

void Explosion::update(GameModel& model, float dt) {
    (void)model;
    m_timer += dt;
    if (m_timer >= m_life) m_alive = false;
}

void Explosion::draw(sf::RenderWindow& w, int tileSize) const {
    sf::RectangleShape r(sf::Vector2f((float)tileSize, (float)tileSize));
    r.setPosition((float)(m_tile.x*tileSize), (float)(m_tile.y*tileSize));
    r.setFillColor(sf::Color(250, 170, 40, 200));
    w.draw(r);
}


void Bonus::update(GameModel& model, float dt) {
    (void)dt;
    if (model.playerTile() == m_tile) {
        apply(model.player());
        model.onBonusPicked();
        m_alive = false;
    }
}

void Bonus::draw(sf::RenderWindow& w, int tileSize) const {
    sf::CircleShape c((float)tileSize * 0.22f);
    c.setPosition((float)(m_tile.x*tileSize + tileSize*0.28f),
                  (float)(m_tile.y*tileSize + tileSize*0.28f));
    c.setFillColor(sf::Color(90, 130, 255));
    w.draw(c);
}


void Coin::update(GameModel& model, float dt) {
    (void)dt;
    if (model.playerTile() == m_tile) {
        model.onCoinPicked();
        m_alive = false;
    }
}

void Coin::draw(sf::RenderWindow& w, int tileSize) const {
    sf::CircleShape c((float)tileSize * 0.18f);
    c.setPosition((float)(m_tile.x*tileSize + tileSize*0.31f),
                  (float)(m_tile.y*tileSize + tileSize*0.31f));
    c.setFillColor(sf::Color(230, 210, 80));
    w.draw(c);
}
