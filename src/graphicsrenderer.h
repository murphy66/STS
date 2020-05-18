#pragma once

#include "floor.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>
#include <functional>
#include <mutex>
#include <condition_variable>


class GraphicsInput 
{
	std::vector<std::function<bool(Floor&)>> actions;	
public:	
	void AddAction(std::function<bool(Floor&)>&& a);
	bool IsEmpty() const { return actions.empty(); }
	bool DoNextPlayerAction(Floor& f);
};


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
	Floor* floor = nullptr;
	sf::Font font;
	std::vector<Button> buttons;	

public:
	GraphicsInput input;

	FloorRenderer();

	void SetFloor(Floor& floor) { this->floor = &floor; }

	void Draw(sf::RenderWindow& w);
	void DrawPlayer(sf::RenderWindow& w);
	void DrawPlayerHand(sf::RenderWindow& w);
	void DrawEnemies(sf::RenderWindow& w);
	void DrawHealthBar(sf::RenderWindow& w, const Entity& e, sf::Vector2f pos);
	void DrawText(sf::RenderWindow& w, const std::string& s, const sf::Color& col, double size, sf::Vector2f pos);

	void CreateButton(sf::Vector2f pos, sf::Vector2f size, const std::string& txt, float txtSize, 
		const std::function<void()>& cb) { buttons.emplace_back(pos, size, txt, txtSize, cb); }	
	void DrawButtons(sf::RenderWindow& w);
	void CreateEndTurnButton();

	void Click(sf::RenderWindow& w, const sf::Event::MouseButtonEvent& e) const;

	void UpdateGameState();
};


class CardRenderer
{
public:
	sf::Vector2f size { 0.08f, 0.14f};

	void Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card) const;
};