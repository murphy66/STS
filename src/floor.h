#pragma once

#include "entity.h"


class Floor
{
public:
	enum class FloorState
	{
		FloorNotStarted,
		PlayerTurn,
		EnemyTurn,
		FloorEnded
	};

private:
	FloorState state = FloorState::FloorNotStarted;
	Player* player;
	vector<std::unique_ptr<Enemy>> enemies;

public:
	Floor(Player* player, std::vector<std::unique_ptr<Enemy>>&& es);
	[[nodiscard]] constexpr FloorState GetState() const noexcept { return state; }
	[[nodiscard]] bool IsFloorDone();
	[[nodiscard]] const auto GetPlayer() const noexcept { return player; }
	[[nodiscard]] const auto& GetEnemies() const noexcept { return enemies; }
	bool PlayCard(int cardIdx, int enemyIdx);
	bool PlayCard(int cardIdx, Entity* e);

	void StartFloor();
	void EndTurn();
	void DoEnemyTurn();

	friend class InputActions;
};
