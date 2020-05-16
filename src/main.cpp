#include "entity.h"
#include "card.h"
#include "floor.h"
#include "renderer.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>

using namespace std;

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Slay The Spire");
	sf::View view(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
	window.setView(view);

	auto player = Player::CreateIronclad();
	Floor floor(player.get());
	player->StartFloor();
	player->BeginTurn();
	
    FloorRenderer renderer(floor);
    
	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        renderer.Draw(window);
        
        window.display();
    }

	/*auto player = Player::CreateIronclad();
	Floor floor(player.get());
	floor.DoFloor();
	if (player->IsAlive())
		cout << "Floor is done\n";
	else
		cout << "Player is dead\n";*/
	
	return 0;
}