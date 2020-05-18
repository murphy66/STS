#pragma once

#include "floor.h"


class ConsoleInput
{
public:
	void DoFloor(Floor& f);

	bool DoNextPlayerAction(Floor& f);
};