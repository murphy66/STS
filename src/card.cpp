#include "card.h"
#include "entity.h"

bool Card::Play(Player* p, Entity* e)
{
	LogMe();
	if (needsTarget && (!e || e == p))
	{
		Log("Must have a valid target");
		return false;		
	}
	if (dmg > 0)
		e->Damage(dmg);
	if (block > 0)
		p->block += block;
	return true;
}

bool CardBash::Play(Player* p, Entity* e)
{
	if (!Card::Play(p, e))
		return false;

	e->vulnerable += 2;
	return true;
}