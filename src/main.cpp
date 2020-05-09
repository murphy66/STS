#include "entity.h"
#include "card.h"
#include "floor.h"

using namespace std;

class Entity;
class Player;
class Card;


int main()
{
	auto player = Player::CreateIronclad();
	Floor floor(player.get());
	floor.DoFloor();
	if (player->IsAlive())
		cout << "Floor is done\n";
	else
		cout << "Player is dead\n";

	return 0;
}