#include "floor.h"
#include <algorithm>
#include <iostream>


Floor::Floor(Player* player, std::vector<std::unique_ptr<Enemy>>&& es)
 : player(player), enemies(std::move(es))
{
}

[[nodiscard]] bool Floor::IsFloorDone()
{
	return !player->IsAlive() || std::all_of(enemies.begin(), enemies.end(), [](const auto& e) { return !e->IsAlive(); });
}

bool Floor::PlayCard(int cardIdx, int enemyIdx)
{
	Enemy* e = (enemyIdx >= 0 && enemyIdx < enemies.size()) ? enemies[enemyIdx].get() : nullptr;
	return PlayCard(cardIdx, e);
}

bool Floor::PlayCard(int cardIdx, Entity* e)
{
	if (state != FloorState::PlayerTurn)
		throw std::logic_error("Invalid state change");

	bool success = player->PlayCard(cardIdx, e);
	if (IsFloorDone())
		state = FloorState::FloorEnded;

	return success;
}

void Floor::StartFloor() 
{
	if (state != FloorState::FloorNotStarted)
		throw std::logic_error("Invalid state change");

	if (IsFloorDone())
	{
		state = FloorState::FloorEnded;
		return;
	}
	
	player->StartFloor();
	player->BeginTurn();
	state = FloorState::PlayerTurn; 
}


void Floor::EndTurn()
{ 
	if (state != FloorState::PlayerTurn)
		throw std::logic_error("Invalid state change");

	player->EndTurn();
	state = FloorState::EnemyTurn; 
}

void Floor::DoEnemyTurn() 
{
	if (state != FloorState::EnemyTurn)
		throw std::logic_error("Invalid state change");

	for (auto& e : enemies)
		e->BeginTurn();

	for (const auto& e : enemies)
	{
		if (!e->IsAlive())
			continue;

		e->DoNextAction(player);
		if (IsFloorDone())
		{
			state = FloorState::FloorEnded;
			return;
		}
	}

	player->BeginTurn();
	state = FloorState::PlayerTurn;
}