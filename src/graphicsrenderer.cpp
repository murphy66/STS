#include "graphicsrenderer.h"
#include <chrono>


FloorRenderer::FloorRenderer() 
{
	if (!font.loadFromFile(R"(./BADABB__.TTF)"))
		if (!font.loadFromFile(R"(./assets/BADABB__.TTF)"))
			if (!font.loadFromFile(R"(../assets/BADABB__.TTF)"))
				throw std::runtime_error("Failed to load font");

	Log("Font loaded");

	CreateEndTurnButton();
}

void FloorRenderer::Draw(sf::RenderWindow& w)
{
	DrawPlayer(w);
	DrawEnemies(w);
	DrawButtons(w);
}

void FloorRenderer::DrawPlayer(sf::RenderWindow& w)
{
	DrawPlayerHand(w);
	DrawHealthBar(w, *floor->GetPlayer(), {0.2f, 0.45f});
}

void FloorRenderer::DrawPlayerHand(sf::RenderWindow& w)
{
	auto player = floor->GetPlayer();
	const auto& hand = player->GetHand();

	const sf::Vector2f cardPos { 0.2f, 0.8f };
	const sf::Vector2f d { 0.12f, 0.0f };
	CardRenderer cr;
	for(size_t i = 0; i < hand.size(); ++i)
	{
		cr.Draw(w, cardPos + (float)i * d, *hand[i]);
	}	
}

void FloorRenderer::DrawEnemies(sf::RenderWindow& w)
{
	const vector<std::unique_ptr<Enemy>>& enemies = floor->GetEnemies();
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		sf::Vector2f pos { 0.6f + i * 0.15f, 0.45f };
		DrawHealthBar(w, *enemies[i].get(), pos);
	}
}

void FloorRenderer::DrawHealthBar(sf::RenderWindow& w, const Entity& e, sf::Vector2f pos)
{
	std::string hpTxt = std::to_string(e.GetHp()) + "/" + std::to_string(e.GetMaxHp());
	DrawText(w, hpTxt, sf::Color::White, 0.05, pos);
}

void FloorRenderer::DrawText(sf::RenderWindow& w, const std::string& s, const sf::Color& col, double size, sf::Vector2f pos)
{
	sf::Text text;
	text.setFont(font);
	text.setString(s);
	const long minTextSize = 20l;	// in pixels
	text.setCharacterSize(std::max(std::lround(size * w.getSize().y), minTextSize));
	text.setFillColor(col);
	text.scale(1.f / w.getSize().x, 1.f / w.getSize().y);
	text.setPosition(pos);
	w.draw(text);
}

void FloorRenderer::DrawButtons(sf::RenderWindow& w)
{
	for (const auto& btn : buttons)
	{
		sf::RectangleShape rectangle{ btn.size };
		rectangle.setFillColor(sf::Color::Green);
		rectangle.setPosition(btn.pos);

		w.draw(rectangle);
		DrawText(w, btn.txt, sf::Color::White, btn.txtSize, btn.pos);
	}
}

void FloorRenderer::CreateEndTurnButton()
{
	CreateButton({ 0.85f, 0.8f }, { 0.1f, 0.1f }, "End\nTurn", 0.04f, [this]() {
		std::cout << "End Turn" << std::endl;
		input.AddAction([](Floor& f) {
			Log("Ending turn");
			return false;
		});
	});
}

void FloorRenderer::Click(sf::RenderWindow& w, const sf::Event::MouseButtonEvent& e) const
{
	double sx = double(e.x) / w.getSize().x;
	double sy = double(e.y) / w.getSize().y;
	//std::cout << "coord: " << e.x << " " << e.y << " scaled: " << sx << " " << sy << std::endl;

	for (const auto& btn : buttons)
	{
		if (sx >= btn.pos.x && sx <= btn.pos.x + btn.size.x &&
			sy >= btn.pos.y && sy <= btn.pos.y + btn.size.y)
		{
			//std::cout << "Hit detected: " << btn.txt << std::endl;
			btn.cb();
		}
	}
}

void FloorRenderer::UpdateGameState()
{
	auto state = floor->GetState();
	switch (state)
	{
	case Floor::FloorState::FloorNotStarted:
		floor->StartFloor();
		break;

	case Floor::FloorState::PlayerTurn:
		if(!input.DoNextPlayerAction(*floor))
			floor->EndTurn();
		break;

	case Floor::FloorState::EnemyTurn:
		floor->DoEnemyTurn();
		break;

	case Floor::FloorState::FloorEnded:
		//TODO
		break;

	default:
		throw std::runtime_error("Not implemented state");
	};
}

void CardRenderer::Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card) const
{
	sf::RectangleShape rectangle{ size };
	rectangle.setFillColor(card.Type() == CardType::Attack ? sf::Color::Red : sf::Color::Blue);
	rectangle.setPosition(pos);

	w.draw(rectangle);
}

void GraphicsInput::AddAction(std::function<bool(Floor&)>&& a)
{
	actions.push_back(std::move(a));
}

bool GraphicsInput::DoNextPlayerAction(Floor& f)
{	
	for (const auto& act : actions)
	{
		if (!act(f))
		{
			actions.clear();	// Discard remaining player actions when turn ends.
			return false;
		}
	}

	actions.clear();

	return true;
}
