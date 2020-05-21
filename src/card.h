#pragma once

#include "entity.h"
#include "logger.h"

class Player;
class Entity;

enum class CardType
{	
	Attack,
	Defend,
	Bash
};

class Card
{
public:
	int cost = 0;
	int dmg = 0;
	int block = 0;
	bool needsTarget = false;
	CardType type;

	Card(int cost, CardType type) : cost(cost), type(type) {}
	virtual ~Card() {}

	virtual bool Play(Player* p, Entity* e);
	virtual std::string ToString() const { return "Unknown Card!"; }

	[[nodiscard]] constexpr int Cost() const noexcept { return cost; }
	[[nodiscard]] constexpr CardType Type() const noexcept { return type; }
	void LogMe() { Log(ToString()); }
};

class CardAttack : public Card
{	
public:
	explicit CardAttack(int dmg = 6) : Card(1, CardType::Attack) { this->dmg = dmg; needsTarget = true; }
	std::string ToString() const override { return "Attack!"; }
};

class CardDefend : public Card
{
public:
	explicit CardDefend(int block = 5) : Card(1, CardType::Defend) { this->block = block; }
	std::string ToString() const override { return "Defend!"; }
};

class CardBash : public Card
{
public:
	int vulnerable = 2;
	explicit CardBash() : Card(2, CardType::Bash) { dmg = 8; needsTarget = true; }
	virtual bool Play(Player* p, Entity* e) override;
	std::string ToString() const override { return "Bash!"; }	
};