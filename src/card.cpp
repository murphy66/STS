#include "card.h"

bool CardAttack::Play(Player* p, Entity* e) 
{
	LogMe();
	if (!e)
	{
		Log("Must have a valid target");
		return false;
	}
	e->Damage(dmg);
	return true;
}

std::string CardAttack::ToString()
{
	return "Attack!";
}

bool CardDefend::Play(Player* p, Entity* e) 
{
	LogMe();
	p->block += block; 
	return true;
}

std::string CardDefend::ToString()
{
	return "Defend!";
}
