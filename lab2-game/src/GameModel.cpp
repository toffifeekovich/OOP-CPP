#include "GameModel.hpp"
#include <algorithm>
#include <cmath> 

GameModel::GameModel(int tileSize): m_tileSize(tileSize), m_map(m_mapW, m_mapH){
    m_player.setTilePos(m_map.playerSpawn());
    applyDifficultySettings();
}

void GameModel::setDifficulty(Difficulty d) {
    m_diff = d;
    applyDifficultySettings();
    m_state = GameState::Menu;
}

void GameModel::applyDifficultySettings() {
    if (m_diff == Difficulty::Easy) {
        m_mapW = 13; m_mapH = 11;
        m_enemyCount = 2;
        m_scoreMult = 1.0f;
        m_brickChance = 0.48f;
        m_initialCoins = 6;
    } else if (m_diff == Difficulty::Normal) {
        m_mapW = 17; m_mapH = 13;
        m_enemyCount = 3;
        m_scoreMult = 1.5f;
        m_brickChance = 0.55f;
        m_initialCoins = 10;
    } else {
        m_mapW = 21; m_mapH = 17;
        m_enemyCount = 5;
        m_scoreMult = 2.0f;
        m_brickChance = 0.60f;
        m_initialCoins = 14;
    }
}

void GameModel::newGame(unsigned seed) {
    m_rng = std::make_unique<Rng>(seed);

    m_map.resize(m_mapW, m_mapH);
    m_map.generateRandom(seed, m_brickChance);

    m_player = Player{};
    m_player.setTilePos(m_map.playerSpawn());

    m_objects.clear();
    m_effects.clear();

    m_stats = Stats{};
    m_stats.difficultyMultiplier = m_scoreMult;

    m_evtExplosion = 0;
    m_evtCoin = 0;

    spawnEnemies();
    spawnInitialCoins();

    m_state = GameState::Playing;
}

IVec2 GameModel::randomFreeTile(int minDistFromPlayer) const {
    for (int tries = 0; tries < 3000; ++tries) {
        int x = m_rng->rangeInt(1, m_map.width() - 2);
        int y = m_rng->rangeInt(1, m_map.height() - 2);
        IVec2 t{x, y};
        if (!isTileFreeForMove(t)) continue;
        if (manhattan(t, playerTile()) < minDistFromPlayer) continue;
        return t;
    }
    return {m_map.width() - 2, m_map.height() - 2};
}

void GameModel::spawnEnemies() {
    for (int i = 0; i < m_enemyCount; ++i) {
        bool chase = (i % 2 == 1);

        std::unique_ptr<Enemy> e;
        if (chase) e = std::make_unique<ChaseEnemy>();
        else       e = std::make_unique<RandomEnemy>();

        e->setTilePos(randomFreeTile(6));

        float cd = chase ? 0.26f : 0.30f;
        e->setMoveCooldown(cd);

        m_objects.push_back(std::move(e));
    }
}

void GameModel::spawnInitialCoins() {
    for (int i = 0; i < m_initialCoins; ++i) {
        auto c = std::make_unique<Coin>();
        c->setTilePos(randomFreeTile(3));
        m_objects.push_back(std::move(c));
    }
}

bool GameModel::hasBombAt(IVec2 t) const {
    for (auto& o : m_objects) {
        auto* b = dynamic_cast<Bomb*>(o.get());
        if (b && b->isAlive() && b->tilePos() == t) return true;
    }
    return false;
}

bool GameModel::isTileFreeForMove(IVec2 t) const {
    if (!m_map.isWalkable(t.x, t.y)) return false;
    if (hasBombAt(t)) return false;
    return true;
}

bool GameModel::tryPlaceBomb(IVec2 at) {
    if (m_state != GameState::Playing) return false;
    if (hasBombAt(at)) return false;

    int active = 0;
    for (auto& o : m_objects) {
        auto* b = dynamic_cast<Bomb*>(o.get());
        if (b && b->isAlive()) active++;
    }
    if (active >= m_player.maxBombs()) return false;

    m_objects.push_back(std::make_unique<Bomb>(at, m_player.bombRange(), 1.6f, 0));
    m_stats.bombsUsed++;
    return true;
}

void GameModel::maybeSpawnBonusOrCoin(IVec2 where) {

    if (m_rng->chance(0.45f)) {
        auto c = std::make_unique<Coin>();
        c->setTilePos(where);
        m_objects.push_back(std::move(c));
        return;
    }


    if (!m_rng->chance(0.18f)) return;

    int t = m_rng->rangeInt(0, 2);
    std::unique_ptr<GameObject> bonus;
    if (t == 0) bonus = std::make_unique<SpeedBonus>();
    if (t == 1) bonus = std::make_unique<RangeBonus>();
    if (t == 2) bonus = std::make_unique<BombCountBonus>();

    bonus->setTilePos(where);
    m_objects.push_back(std::move(bonus));
}

void GameModel::explodeBomb(const Bomb& b) {
    if (m_state != GameState::Playing) return;

    m_evtExplosion++;

    const IVec2 center = b.tilePos();

    auto addExplosion = [&](IVec2 p) {
        m_effects.push_back(std::make_unique<Explosion>(p, 0.2f));
    };

    auto hitTile = [&](IVec2 p) -> bool {
        Block* block = m_map.blockAt(p.x, p.y);

        if (!block) {
            addExplosion(p);
            return false;
        }

        block->onExplosion();
        addExplosion(p);

        if (block->destroyed()) {
            m_map.clearBlock(p.x, p.y);

            m_stats.bricksDestroyed++;
            m_stats.baseScore += 10;

            maybeSpawnBonusOrCoin(p);
        }

        return true;
    };

    addExplosion(center);

    const IVec2 dirs[4] = { {1,0},{-1,0},{0,1},{0,-1} };
    for (auto d : dirs) {
        for (int i = 1; i <= b.range(); ++i) {
            IVec2 p{ center.x + d.x * i, center.y + d.y * i };
            if (!inBounds(p.x, p.y, m_map.width(), m_map.height())) break;
            if (hitTile(p)) break;
        }
    }
}

void GameModel::onCoinPicked() {
    m_evtCoin++;
    m_stats.coinsPicked++;
    m_stats.baseScore += 5;
}

void GameModel::onBonusPicked() {
    m_stats.bonusesPicked++;
    m_stats.baseScore += 8;
}

bool GameModel::popExplosionSoundEvent() {
    if (m_evtExplosion > 0) { m_evtExplosion--; return true; }
    return false;
}

bool GameModel::popCoinSoundEvent() {
    if (m_evtCoin > 0) { m_evtCoin--; return true; }
    return false;
}

void GameModel::damageAndWinLoseCheck() {
    auto isOnFire = [&](IVec2 t) {
        for (auto& e : m_effects) {
            if (e->isAlive() && e->tilePos() == t) return true;
        }
        return false;
    };

    if (m_player.isAlive() && isOnFire(m_player.tilePos())) {
        m_player.takeDamage();
    }

    for (auto& o : m_objects) {
        if (!o->isAlive()) continue;

        if (auto* en = dynamic_cast<Enemy*>(o.get())) {
            if (isOnFire(en->tilePos())) {
                en->takeDamage();
                m_stats.enemiesKilled++;
                m_stats.baseScore += 50;
            }
            if (en->isAlive() && en->tilePos() == m_player.tilePos()) {
                m_player.takeDamage();
            }
        }
    }

    bool anyEnemyAlive = false;
    for (auto& o : m_objects) {
        auto* en = dynamic_cast<Enemy*>(o.get());
        if (en && en->isAlive()) { anyEnemyAlive = true; break; }
    }

    if (!m_player.isAlive()) {
        computeFinalScore();
        m_state = GameState::Lose;
    } else if (!anyEnemyAlive) {
        computeFinalScore();
        m_state = GameState::Win;
    }
}

void GameModel::cleanupDead() {
    auto dead = [](const std::unique_ptr<GameObject>& p) { return !p->isAlive(); };

    m_objects.erase(
        std::remove_if(m_objects.begin(), m_objects.end(), dead),
        m_objects.end());

    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(), dead), 
        m_effects.end());
}

void GameModel::computeFinalScore() {

    int shownSeconds = (int)std::round(m_stats.timeSeconds);

    float score = (float)m_stats.baseScore * m_stats.difficultyMultiplier;
    score -= (float)shownSeconds * m_stats.timePenaltyPerSec;
    score -= (float)m_stats.bombsUsed * (float)m_stats.bombPenalty;

    if (score < 0.f) score = 0.f;

    m_stats.finalScore = (int)std::lround(score);
}

void GameModel::update(float dt) {
    if (m_state == GameState::Playing) {
        m_stats.timeSeconds += dt;

        m_player.update(*this, dt);

        for (auto& o : m_objects) o->update(*this, dt);

        for (auto& e : m_effects) e->update(*this, dt);

        damageAndWinLoseCheck();
        cleanupDead();
    } else {
        for (auto& o : m_objects) o->update(*this, dt);
        for (auto& e : m_effects) e->update(*this, dt);
        cleanupDead();
    }
}
