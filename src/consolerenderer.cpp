#include "consolerenderer.h"

void ConsoleInput::DoFloor(Floor& f)
{
	f.StartFloor();	
	while (!f.IsFloorDone())
	{
		while (DoNextPlayerAction(f))
		{
			if (f.IsFloorDone())
				return;
		}
		f.EndTurn();
		f.DoEnemyTurn();
	}
}

bool ConsoleInput::DoNextPlayerAction(Floor& f)
{
	auto player = f.GetPlayer();
	const auto& enemies = f.GetEnemies();

	// Show hand to player
	{
		std::string hand = std::string() + "Hp: " + std::to_string(player->GetHp()) +
			" Mana: " + std::to_string(player->GetEnergy()) +
			" Block: " + std::to_string(player->GetBlock()) + " ";
		for (int i = 0; i < player->GetHand().size(); ++i)
		{
			hand += "[" + player->GetHand()[i]->ToString() + "] ";
		}
		Log(hand);
	}

	// Show enemy hps
	{
		std::string nmy = "Enemies: ";
		for (int i = 0; i < enemies.size(); ++i)
		{
			nmy += (i > 0 ? ", " : "") + enemies[i]->ToString();
		}
		Log(nmy);
	}

	int cardIdx, enemyIdx;
	std::cin >> cardIdx >> enemyIdx;

	if (cardIdx == -1) // End turn
		return false;

	if (!f.PlayCard(cardIdx, enemyIdx))
		Log("Failed to play card");

	return true;
}