#pragma once

#include "entity.h"
#include "logger.h"

class Player;
class Entity;

enum class CardType
{	
	Attack,
	Defend
};

class Card
{
protected:
	int cost = 0;
	CardType type;

public:
	Card(int cost, CardType type) : cost(cost), type(type) {}
	virtual ~Card() {}

	virtual bool Play(Player* p, Entity* e) = 0;
	virtual std::string ToString() const { return "Unknown Card!"; }

	[[nodiscard]] constexpr int Cost() const noexcept { return cost; }
	[[nodiscard]] constexpr CardType Type() const noexcept { return type; }
	void LogMe() { Log(ToString()); }
};

class CardAttack : public Card
{
	const int dmg = 0;
public:
	explicit CardAttack(int dmg = 6) : Card(1, CardType::Attack), dmg(dmg) {}
	bool Play(Player* p, Entity* e) override;
	std::string ToString() const override { return "Attack!"; }
};

class CardDefend : public Card
{
	int block = 0;
public:
	explicit CardDefend(int block = 5) : Card(1, CardType::Defend), block(block) {}
	bool Play(Player* p, Entity* e) override;
	std::string ToString() const override { return "Defend!"; }
};