#include "entity.h"
#include "card.h"
#include "floor.h"
#include "graphicsrenderer.h"
#include "consolerenderer.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>
#include <thread>

using namespace std;

void ConsoleRenderedGame()
{
	auto player = Player::CreateIronclad();
	vector<unique_ptr<Enemy>> es;
	es.push_back(move(make_unique<JawWorm>()));
	es.push_back(move(make_unique<JawWorm>()));
	ConsoleInput input;
	Floor floor(player.get(), move(es));
	input.DoFloor(floor);
}

void GraphicsRenderedGame()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Slay The Spire");
	sf::View view(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
	window.setView(view);

	FloorRenderer renderer;

	auto player = Player::CreateIronclad();
	vector<unique_ptr<Enemy>> es;
	es.push_back(move(make_unique<JawWorm>()));
	Floor floor(player.get(), move(es));
	renderer.SetFloor(floor);

	while (window.isOpen() && !floor.IsFloorDone())
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
		renderer.UpdateGameState();

		window.display();
	}
}

int main()
{
	//ConsoleRenderedGame();
	GraphicsRenderedGame();

	return 0;
}