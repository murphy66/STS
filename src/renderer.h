#pragma once

#include "floor.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>


class FloorRenderer
{
	const Floor* floor;
	sf::Font font;

public:
	FloorRenderer(const Floor& floor);

	void Draw(sf::RenderWindow& w) const;
	void DrawPlayer(sf::RenderWindow& w) const;
	void DrawPlayerHand(sf::RenderWindow& w) const;
	void DrawText(sf::RenderWindow& w, const std::string& s, const sf::Color& col, double size, sf::Vector2f pos) const;
};


class CardRenderer
{
public:
	sf::Vector2f size { 0.08f, 0.14f};

	void Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card) const;
};