#pragma once

#include "card.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>


class FloorRenderer
{
public:
	
};


class CardRenderer
{
public:
	sf::Vector2f size { 0.08f, 0.14f};

	void Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card);
};