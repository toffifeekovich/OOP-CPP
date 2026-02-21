#pragma once
#include "Map.hpp"
#include "Objects.hpp"
#include <memory>
#include <vector>

enum class GameState { Menu, Playing, Win, Lose };
enum class Difficulty { Easy = 1, Normal = 2, Hard = 3 };

struct Stats {
    int bricksDestroyed = 0;
    int enemiesKilled = 0;
    int coinsPicked = 0;
    int bonusesPicked = 0;
    int bombsUsed = 0;
    float timeSeconds = 0.f;

    int baseScore = 0;
    float difficultyMultiplier = 1.f;

    float timePenaltyPerSec = 2.0f;
    int bombPenalty = 3;

    int finalScore = 0;
};

class GameModel {
public:
    explicit GameModel(int tileSize);

    void setDifficulty(Difficulty d);
    void newGame(unsigned seed);
    void goToMenu() { m_state = GameState::Menu; }

    void update(float dt);

    const Map& map() const { return m_map; }
    const std::vector<std::unique_ptr<GameObject>>& objects() const { return m_objects; }
    const std::vector<std::unique_ptr<GameObject>>& effects() const { return m_effects; }

    Player& player() { return m_player; }
    const Player& player() const { return m_player; }
    IVec2 playerTile() const { return m_player.tilePos(); }

    int tileSize() const { return m_tileSize; }
    GameState state() const { return m_state; }
    Difficulty difficulty() const { return m_diff; }
    const Stats& stats() const { return m_stats; }

    bool isTileFreeForMove(IVec2 t) const;
    bool hasBombAt(IVec2 t) const;

    bool tryPlaceBomb(IVec2 at);
    void explodeBomb(const Bomb& b);

    Rng& rng() { return *m_rng; }

    void onCoinPicked();
    void onBonusPicked();

    bool popExplosionSoundEvent();
    bool popCoinSoundEvent();

private:
    int m_tileSize = 32;
    Difficulty m_diff = Difficulty::Normal;

    int m_mapW = 17;
    int m_mapH = 13;
    int m_enemyCount = 3;
    float m_scoreMult = 1.5f;
    float m_brickChance = 0.55f;
    int m_initialCoins = 8;

    Map m_map;
    Player m_player;

    std::vector<std::unique_ptr<GameObject>> m_objects;
    std::vector<std::unique_ptr<GameObject>> m_effects;
    std::unique_ptr<Rng> m_rng;

    GameState m_state = GameState::Menu;
    Stats m_stats{};

    int m_evtExplosion = 0;
    int m_evtCoin = 0;

    void applyDifficultySettings();
    void spawnEnemies();
    void spawnInitialCoins();

    IVec2 randomFreeTile(int minDistFromPlayer) const;
    void maybeSpawnBonusOrCoin(IVec2 where);

    void damageAndWinLoseCheck();
    void cleanupDead();
    void computeFinalScore();
};
