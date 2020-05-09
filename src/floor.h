#pragma once

#include "entity.h"

class Floor
{
	Player* player;
	vector<std::unique_ptr<Enemy>> enemies;

public:
	Floor(Player* player);
	void DoFloor();
	[[nodiscard]] bool IsFloorDone();

private:
	bool DoNextPlayerAction();
};