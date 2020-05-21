#pragma once

#include "floor.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>
#include <functional>
#include <mutex>
#include <condition_variable>


class EntityRenderer;

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
	std::vector<Button> buttons;	
	int selectedCardIdx = -1;
	vector<EntityRenderer> entityRenderers;

public:
	GraphicsInput input;

	FloorRenderer();

	void SetFloor(Floor& floor);
	void Draw(sf::RenderWindow& w);
	void CreateButton(sf::Vector2f pos, sf::Vector2f size, const std::string& txt, float txtSize, 
		const std::function<void()>& cb) { buttons.emplace_back(pos, size, txt, txtSize, cb); }	
	void Click(sf::RenderWindow& w, const sf::Event::MouseButtonEvent& e);
	void UpdateGameState();
};


class CardRenderer
{
private:
	const sf::Vector2f size { 0.08f, 0.14f };
	const sf::Vector2f delta { 0.12f, 0.0f };
	const sf::Vector2f cardPos { 0.2f, 0.8f };	
	void Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card, bool selected) const;

public:
	void Draw(sf::RenderWindow& w, size_t cardIdx, const Card& card, bool selected) const;
	[[nodiscard]] int GetCardIdxFromCoords(double x, double y) const;
};

class EntityRenderer
{
private:
	sf::Texture texCharacter;
	sf::Vector2f pos;
	sf::Vector2f characterSize{0.15f, 0.25f};	

public:
	Entity* entity = nullptr;
	
	EntityRenderer(Entity* entity, sf::Vector2f pos);
	void Draw(sf::RenderWindow& w) const;
	void DrawHealthBar(sf::RenderWindow& w, sf::Vector2f pos) const;
	[[nodiscard]] bool HitTest(sf::Vector2f c) const;	
};