#include "entity.h"
#include "logger.h"
#include "random.h"
#include <random>
#include <string>
#include <sstream>

using std::to_string;

void Entity::LogMe() const 
{ 
	Log("Hp: " + std::to_string(hp) + " Block: " + std::to_string(block)); 
}

void Entity::Damage(int dmg) noexcept 
{
	Log("Got attacked for " + std::to_string(dmg));
	if (vulnerable > 0)
		dmg = dmg + dmg / 2;

	int d = dmg;
	dmg = std::max(dmg - block, 0);
	block = std::max(block - d, 0);
	if (dmg == 0)
		return;

	hp = std::max(hp - dmg, 0);
}

void Entity::BeginTurn()
{
	block = 0;
	if (vulnerable > 0) --vulnerable;
}

void Player::AddCard(unique_ptr<Card>&& c)
{
	Log(std::string("Adding card ") + c->ToString());
	deck.push_back(move(c));
}

void Player::StartFloor()
{
	Log("Setting up deck");
	hand.clear();
	discard.clear();
	drawPile.clear();
	for (auto& c : deck)
		drawPile.push_back(c.get());
	ReShuffle();
}

void Player::ReShuffle()
{
	Log("Shuffling deck");
	static std::random_device rd;
	static std::mt19937 gen(rd());

	for (Card* c : discard)
		drawPile.push_back(c);
	discard.clear();
	shuffle(drawPile.begin(), drawPile.end(), gen);
}

void Player::DrawCard()
{	
	const size_t MAX_HAND_SIZE = 10;
	if (hand.size() >= MAX_HAND_SIZE)
	{
		Log("Tried to draw a card but my hand is already full");
		return;
	}

	if (drawPile.empty())
	{
		if (discard.empty())
		{
			Log("Tried to draw a card but there are no cards to draw");
			return;
		}
		ReShuffle();
	}

	Log(std::string("Drawing card... ") + drawPile.back()->ToString());
	hand.push_back(drawPile.back());
	drawPile.pop_back();
}

void Player::DrawCards(int num)
{
	for (int i = 0; i < num; ++i)
		DrawCard();
    std::sort(hand.begin(), hand.end(), [](const Card* c1, const Card* c2) { return c1->type < c2->type; });
}

bool Player::PlayCard(size_t idx, Entity* e)
{
	if (idx >= hand.size())
	{
		Log("Card with index: " + to_string(idx) + " cannot be played because there are only " + 
			to_string(hand.size()) + " cards in hand");
		return false;
	}

	Card* c = hand[idx];
	int cost  = c->Cost();
	if (cost > energy)
	{
		Log("Can't play card with cost: " + to_string(cost) + " because player has only " + to_string(energy) + " energy");
		return false;
	}
	
	if (!c->Play(this, e))
		return false;

	energy -= cost;
	discard.push_back(c);
	hand.erase(hand.begin()+idx);
	return true;
}

bool Player::Discard(int idx)
{
	if (idx >= hand.size())
	{
		Log("Card with index: " + to_string(idx) + " cannot be discarded because there are only " + 
			to_string(hand.size()) + " cards in hand");
		return false;
	}

	Card* c = hand[idx];
	discard.push_back(c);
	hand.erase(hand.begin()+idx);
	return true;
}

void Player::BeginTurn()
{
	Entity::BeginTurn();
	energy = maxEnergy;
	block = 0;
	DrawCards(nDrawCard);
}

void Player::EndTurn()
{	
	while(!hand.empty())
		Discard(0);
}

[[nodiscard]] unique_ptr<Player> Player::CreateIronclad()
{
	auto p = std::make_unique<Player>();
	for (int i = 0; i < 5; ++i)
	{
		p->AddCard(std::make_unique<CardDefend>());
		p->AddCard(std::make_unique<CardAttack>());
	}
	p->AddCard(std::make_unique<CardBash>());
	return std::move(p);
}

void EnemyAction::Do(Player* p, Enemy* me)
{
	if (dmg > 0)
		p->Damage(dmg);
	if (block > 0)
		me->block += block;
	done = true;
}

void Enemy::DoNextAction(Player* p)
{
	if (actions.empty() || actions.back()->done)
		actions.push_back(std::move(GetNextAction()));
	return actions.back()->Do(p, this);
}

[[nodiscard]] const EnemyAction& Enemy::GetIntention()
{
	if (actions.empty() || actions.back()->done)
		actions.push_back(std::move(GetNextAction()));
	return *actions.back();
}

[[nodiscard]] std::string EnemyAction::ToString() const
{
	std::stringstream ss;
	if (dmg > 0)
		ss << "attacks for " << dmg;
	if (block > 0)
		ss << (ss.str().empty() ? "" : " and ") << "buffs block";
	return ss.str();
}

bool Enemy::DidAction(EnemyActionType type, int rounds) const
{
	if (actions.size() < rounds)
		return false;

	for (int i = 0; i < rounds; ++i)
	{
		if (actions[actions.size() - 1 - i]->type != type)
			return false;
	}
	return true;
}

JawWorm::JawWorm() : Enemy(Rand(42, 46)) { actions.push_back(Chomp()); }

[[nodiscard]] std::unique_ptr<EnemyAction> JawWorm::GetNextAction()
{
	int r = Rand(100);
	if (r <= 45)
	{
		if (DidAction(EnemyActionType::JawWormBellow, 1))
			return GetNextAction();
		return Bellow();
	}
	if (r <= 75)
	{
		if (DidAction(EnemyActionType::JawWormThrash, 2))
			return GetNextAction();
		return Thrash();
	}
	return Chomp();
}
