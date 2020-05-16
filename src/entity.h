#pragma once

#include "card.h"
#include "logger.h"
#include <vector>
#include <memory>
#include <string>

using std::vector;
using std::unique_ptr;

class Card;

class Entity
{
public:
	int maxHp = 0;
	int hp = 0;
	int block = 0;
	int str = 0;
	int dex = 0;

public:
	constexpr explicit Entity(int hp) noexcept : maxHp(hp), hp(hp) {}
	virtual ~Entity() {}
	
	void Damage(int dmg) noexcept;	

	[[nodiscard]] constexpr int GetBlock() const noexcept { return block; }
	[[nodiscard]] constexpr int GetHp() const noexcept { return hp; }
	[[nodiscard]] constexpr int GetMaxHp() const noexcept { return maxHp; }
	[[nodiscard]] constexpr bool IsAlive() const noexcept { return hp > 0; }
	void LogMe() const;	
};


class Player : public Entity
{
	vector<unique_ptr<Card>> deck;
	int energy = 3;

	vector<Card*> hand;
	vector<Card*> discard;
	vector<Card*> drawPile;

public:
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

	static [[nodiscard]] unique_ptr<Player> CreateIronclad();
};


class Enemy : public Entity
{
public:
	explicit Enemy(int hp) : Entity(hp) {}
	virtual void DoAction(Player* p) { Log("ca-caw!"); }
	[[nodiscard]] std::string ToString() const { return std::string("Bird ") + std::to_string(hp); }
};

class JawWorm : public Enemy
{
public:
	JawWorm() : Enemy(43) {}
	void DoAction(Player* p) override;
};