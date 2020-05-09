#include "entity.h"
#include "card.h"

#define CATCH_CONFIG_MAIN 
#include <catch2/catch.hpp>

using namespace std;

class Entity;
class Player;
class Card;


TEST_CASE("Test Player", "[Test Player]")
{
	{
		Player p;

		// Can't draw card with an empty deck.
		p.DrawCard();
		REQUIRE(p.GetHand().empty());

		// Setup deck and check if hand is empty.
		p.AddCard(make_unique<CardDefend>());
		p.StartFloor();
		REQUIRE(p.GetHand().empty());

		// Try to draw 5 cards but can only draw one.
		p.DrawCards(5);
		REQUIRE(p.GetHand().size() == 1);

		// Can't play second card with only 1 card in hand.
		p.PlayCard(1, nullptr);
		REQUIRE(p.GetHand().size() == 1);

		// Hand is empty after playing the one card in hand and one energy is used up.
		p.PlayCard(0, nullptr);
		REQUIRE(p.GetHand().empty());
		REQUIRE(p.GetEnergy() == 2);

		// Redraw our one card and it is in out hand again.
		p.DrawCard();
		REQUIRE(p.GetHand().size() == 1);
	}

	{
		Player p;
		for (int i = 0; i < 4; ++i)
			p.AddCard(make_unique<CardDefend>());
		p.StartFloor();
		p.DrawCards(4);

		REQUIRE(p.PlayCard(0, nullptr));
		REQUIRE(p.PlayCard(0, nullptr));
		REQUIRE(p.PlayCard(0, nullptr));
		REQUIRE(p.PlayCard(0, nullptr) == false);
	}
}

TEST_CASE("Test Defend", "[Test Cards]")
{
	Player p;
	p.AddCard(make_unique<CardDefend>(5));
	p.StartFloor();
	p.DrawCard();
	p.PlayCard(0, nullptr);
	REQUIRE(p.GetBlock() == 5);
}

TEST_CASE("Test Attack", "[Test Cards]")
{
	Player p;
	Entity e(20);
	p.AddCard(make_unique<CardAttack>(5));
	p.StartFloor();
	p.DrawCard();
	p.PlayCard(0, &e);
	REQUIRE(e.GetHp() == 15);
}