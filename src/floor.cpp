#include "floor.h"
#include <algorithm>
#include <iostream>


Floor::Floor(Player* player, std::vector<std::unique_ptr<Enemy>>&& es): player(player) , enemies(std::move(es))
{
}

void Floor::DoFloor()
{
	if (!player->IsAlive())
		return;

	player->StartFloor();

	while(!IsFloorDone())
	{
		player->BeginTurn();
		while (DoNextPlayerAction()) 
		{
			if (IsFloorDone())
				return;
		}
		player->EndTurn();

		for (const auto& e : enemies)
		{
			if (!e->IsAlive())
				continue;

			e->DoAction(player);
			if (!player->IsAlive())
				return;
		}
	}
}

[[nodiscard]] bool Floor::IsFloorDone()
{
	return std::all_of(enemies.begin(), enemies.end(), [](const auto& e) { return !e->IsAlive(); });
}

bool Floor::DoNextPlayerAction()
{
	// Show hand to player
	std::string hand = std::string() + "Hp: " + std::to_string(player->GetHp()) +
					 	" Mana: " + std::to_string(player->GetEnergy()) + 
						" Block: " + std::to_string(player->GetBlock()) + " ";
	for (int i = 0; i < player->GetHand().size(); ++i)
	{
		hand += "[" + player->GetHand()[i]->ToString() + "] ";
	}
	Log(hand);

	// Show enemy hps
	std::string nmy = "Enemies: ";
	for (int i = 0; i < enemies.size(); ++i)
	{
		nmy += (i > 0 ? ", " : "") + enemies[i]->ToString();
	}
	Log(nmy);

	int cardIdx, enemyIdx;
	std::cin >> cardIdx >> enemyIdx;

	if (cardIdx == -1) // End turn
		return false;

	Enemy* e = (enemyIdx >= 0 && enemyIdx < enemies.size()) ? enemies[enemyIdx].get() : nullptr;
	player->PlayCard(cardIdx, e);
	return true;
}