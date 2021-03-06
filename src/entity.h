#pragma once

#include "card.h"
#include "logger.h"
#include <vector>
#include <memory>
#include <string>

using std::vector;
using std::unique_ptr;

class Card;
class Enemy;

class Entity
{
public:
	int maxHp = 0;
	int hp = 0;
	int block = 0;
	int str = 0;
	int dex = 0;
	int vulnerable = 0;

public:
	constexpr explicit Entity(int hp) noexcept : maxHp(hp), hp(hp) {}
	virtual ~Entity() {}
	
	void Damage(int dmg) noexcept;

	[[nodiscard]] constexpr int GetBlock() const noexcept { return block; }
	[[nodiscard]] constexpr int GetHp() const noexcept { return hp; }
	[[nodiscard]] constexpr int GetMaxHp() const noexcept { return maxHp; }
	[[nodiscard]] constexpr bool IsAlive() const noexcept { return hp > 0; }
	void BeginTurn();
	void LogMe() const;
};


class Player : public Entity
{
	vector<unique_ptr<Card>> deck;
    int maxEnergy = 3;
	int energy = 3;

	vector<Card*> hand;
	vector<Card*> discard;
	vector<Card*> drawPile;

public:
    static constexpr int nDrawCard = 5;

	Player() : Entity(30) {}
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	[[nodiscard]] const auto& GetHand() const { return hand; }
	[[nodiscard]] constexpr auto GetEnergy() const noexcept { return energy; }

	void AddCard(unique_ptr<Card>&& c);
	void StartFloor();
	void ReShuffle();
	void DrawCard();
	void DrawCards(int num);
	bool PlayCard(size_t idx, Entity* e);
	bool Discard(int idx);

	void BeginTurn();
	void EndTurn();

	[[nodiscard]] static unique_ptr<Player> CreateIronclad();
};


enum class EnemyActionType
{
	JawWormChomp, JawWormThrash, JawWormBellow
};

class EnemyAction
{
public:
	int dmg = 0;
	int block = 0;
	bool done = false;
	EnemyActionType type;

	EnemyAction(EnemyActionType type) : type(type) {}
	virtual void Do(Player* p, Enemy* me);
	std::string ToString() const;
};


class Enemy : public Entity
{
public:	
	vector<std::unique_ptr<EnemyAction>> actions;

	explicit Enemy(int hp) : Entity(hp) {}

	[[nodiscard]] virtual std::unique_ptr<EnemyAction> GetNextAction() = 0;
	[[nodiscard]] const EnemyAction& GetIntention();

	void DoNextAction(Player* p);

	[[nodiscard]] std::string ToString() const { return std::string("Bird ") + std::to_string(hp); }
	[[nodiscard]] bool DidAction(EnemyActionType type, int rounds) const;
};

class JawWorm : public Enemy
{
public:
	JawWorm();

	std::unique_ptr<EnemyAction> Chomp() { auto a = std::make_unique<EnemyAction>(EnemyActionType::JawWormChomp); a->dmg = 12; return a; }
	std::unique_ptr<EnemyAction> Thrash() { auto a = std::make_unique<EnemyAction>(EnemyActionType::JawWormThrash); a->dmg = 7; a->block = 5; return a; }
	std::unique_ptr<EnemyAction> Bellow() { auto a = std::make_unique<EnemyAction>(EnemyActionType::JawWormBellow); a->block = 9; return a; }  // TODO gain strength
	[[nodiscard]] std::unique_ptr<EnemyAction> GetNextAction() override;
};
