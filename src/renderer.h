#pragma once

#include "floor.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>
#include <functional>


struct Button
{
	sf::Vector2f pos;
	sf::Vector2f size;
	std::string txt;
	float txtSize;
	std::function<void()> cb;
	Button(sf::Vector2f pos, sf::Vector2f size, const std::string& txt, float txtSize, const std::function<void()>& cb)
		: pos(pos), size(size), txt(txt), txtSize(txtSize), cb(cb) {}
};


class FloorRenderer
{
	const Floor* floor;
	sf::Font font;
	std::vector<Button> buttons;

public:
	FloorRenderer(const Floor& floor);

	void Draw(sf::RenderWindow& w) const;
	void DrawPlayer(sf::RenderWindow& w) const;
	void DrawPlayerHand(sf::RenderWindow& w) const;
	void DrawEnemies(sf::RenderWindow& w) const;
	void DrawHealthBar(sf::RenderWindow& w, const Entity& e, sf::Vector2f pos) const;
	void DrawText(sf::RenderWindow& w, const std::string& s, const sf::Color& col, double size, sf::Vector2f pos) const;

	void CreateButton(sf::Vector2f pos, sf::Vector2f size, const std::string& txt, float txtSize, 
		const std::function<void()>& cb) { buttons.emplace_back(pos, size, txt, txtSize, cb); }
	void DrawButtons(sf::RenderWindow& w) const;

	void Click(sf::RenderWindow& w, const sf::Event::MouseButtonEvent& e) const;
};


class CardRenderer
{
public:
	sf::Vector2f size { 0.08f, 0.14f};

	void Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card) const;
};