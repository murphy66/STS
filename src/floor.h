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
	[[nodiscard]] const auto GetPlayer() const noexcept { return player; }
	[[nodiscard]] const auto& GetEnemies() const noexcept { return enemies; }

private:
	bool DoNextPlayerAction();
};