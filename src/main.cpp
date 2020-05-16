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
	vector<unique_ptr<Enemy>> es;
	es.push_back(move(make_unique<JawWorm>()));
	es.push_back(move(make_unique<JawWorm>()));
	Floor floor(player.get(), move(es));
	player->StartFloor();
	player->BeginTurn();
	
    FloorRenderer renderer(floor);
    renderer.CreateButton({0.85f, 0.8f}, {0.1f, 0.1f}, "End\nTurn", 0.04f, []() { std::cout << "End Turn" << std::endl; });
    
	while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
			{
			    if (event.mouseButton.button == sf::Mouse::Left)
			    {
			    	renderer.Click(window, event.mouseButton);
			    }
			}
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