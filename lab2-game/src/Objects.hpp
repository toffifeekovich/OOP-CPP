#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include "Util.hpp"

class GameModel;

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void update(GameModel& model, float dt) = 0;
    virtual void draw(sf::RenderWindow& w, int tileSize) const = 0;

    bool isAlive() const { return m_alive; }
    void kill() { m_alive = false; }

    IVec2 tilePos() const { return m_tile; }
    void setTilePos(IVec2 p) { m_tile = p; }

protected:
    IVec2 m_tile{};
    bool m_alive = true;
};


class Block {
public:
    virtual ~Block() = default;
    virtual bool isSolid() const = 0;
    virtual void onExplosion() = 0;
    bool destroyed() const { return m_destroyed; }

    virtual void draw(sf::RenderWindow& w, int tileSize, int x, int y) = 0;

protected:
    bool m_destroyed = false;
};

class WallBlock : public Block {
public:
    bool isSolid() const override { return true; }
    void onExplosion() override {}
    void draw(sf::RenderWindow& w, int tileSize, int x, int y) override;
};

class BrickBlock : public Block {
public:
    bool isSolid() const override { return true; }
    void onExplosion() override { m_destroyed = true; }
    void draw(sf::RenderWindow& w, int tileSize, int x, int y) override;
};


class Player : public GameObject {
public:
    void update(GameModel& model, float dt) override;
    void draw(sf::RenderWindow& w, int tileSize) const override;

    void requestMove(int dx, int dy) { m_reqDx = dx; m_reqDy = dy; }
    void requestBomb() { m_reqBomb = true; }

    int maxBombs() const { return m_maxBombs; }
    int bombRange() const { return m_bombRange; }
    float moveCooldown() const { return m_moveCooldown; }

    void addMaxBombs(int v) { m_maxBombs += v; }
    void addRange(int v) { m_bombRange += v; }
    void addSpeed(float v) { m_moveCooldown = std::max(0.06f, m_moveCooldown - v); }

    void takeDamage() { m_alive = false; }

private:
    int m_reqDx = 0, m_reqDy = 0;
    bool m_reqBomb = false;

    float m_moveTimer = 0.0f;
    float m_moveCooldown = 0.16f;

    int m_maxBombs = 1;
    int m_bombRange = 2;
};


class Enemy : public GameObject {
public:
    void update(GameModel& model, float dt) override;

    void takeDamage() { m_alive = false; }
    void setMoveCooldown(float v) { m_moveCooldown = v; }

protected:
    virtual IVec2 chooseStep(GameModel& model) = 0;

    float m_moveTimer = 0.0f;
    float m_moveCooldown = 0.28f;
};

class RandomEnemy : public Enemy {
public:
    void draw(sf::RenderWindow& w, int tileSize) const override;
protected:
    IVec2 chooseStep(GameModel& model) override;
};

class ChaseEnemy : public Enemy {
public:
    void draw(sf::RenderWindow& w, int tileSize) const override;
protected:
    IVec2 chooseStep(GameModel& model) override;
};


class Bomb : public GameObject {
public:
    Bomb(IVec2 pos, int range, float fuseSec, int ownerId);

    void update(GameModel& model, float dt) override;
    void draw(sf::RenderWindow& w, int tileSize) const override;

    int range() const { return m_range; }

private:
    int m_range = 2;
    float m_timer = 0.0f;
    float m_fuse = 1.6f;
    int m_ownerId = 0;
};


class Explosion : public GameObject {
public:
    Explosion(IVec2 pos, float lifeSec);

    void update(GameModel& model, float dt) override;
    void draw(sf::RenderWindow& w, int tileSize) const override;

private:
    float m_timer = 0.0f;
    float m_life = 0.35f;
};


class Bonus : public GameObject {
public:
    void update(GameModel& model, float dt) override;
    void draw(sf::RenderWindow& w, int tileSize) const override;
    virtual void apply(Player& p) = 0;
};

class SpeedBonus : public Bonus { public: void apply(Player& p) override { p.addSpeed(0.03f); } };
class RangeBonus : public Bonus { public: void apply(Player& p) override { p.addRange(1); } };
class BombCountBonus : public Bonus { public: void apply(Player& p) override { p.addMaxBombs(1); } };


class Coin : public GameObject {
public:
    void update(GameModel& model, float dt) override;
    void draw(sf::RenderWindow& w, int tileSize) const override;
};
