#pragma once

#include "entity.h"
#include "logger.h"

class Player;
class Entity;


class Card
{
protected:
	int cost = 0;

public:
	Card(int cost) : cost(cost) {}
	virtual ~Card() {}

	virtual bool Play(Player* p, Entity* e) = 0;
	virtual std::string ToString() { return "Unknown Card!"; }

	[[nodiscard]] constexpr int Cost() { return cost; }
	void LogMe() { Log(ToString()); }
};

class CardAttack : public Card
{
	const int dmg = 0;
public:
	explicit CardAttack(int dmg = 5) : Card(1), dmg(dmg) {}
	bool Play(Player* p, Entity* e) override;
	std::string ToString() override;
};

class CardDefend : public Card
{
	int block = 0;
public:
	explicit CardDefend(int block = 5) : Card(1), block(block) {}
	bool Play(Player* p, Entity* e) override;
	std::string ToString() override;
};