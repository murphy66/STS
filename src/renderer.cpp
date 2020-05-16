#include "renderer.h"


FloorRenderer::FloorRenderer(const Floor& floor) : floor(&floor) 
{
	if (!font.loadFromFile(R"(./BADABB__.TTF)"))
		if (!font.loadFromFile(R"(./assets/BADABB__.TTF)"))
			if (!font.loadFromFile(R"(../assets/BADABB__.TTF)"))
				throw std::runtime_error("Failed to load font");

	Log("Font loaded");
}

void FloorRenderer::Draw(sf::RenderWindow& w) const
{
	DrawPlayer(w);
	DrawEnemies(w);
	DrawButtons(w);
}

void FloorRenderer::DrawPlayer(sf::RenderWindow& w) const
{
	DrawPlayerHand(w);
	DrawHealthBar(w, *floor->GetPlayer(), {0.2f, 0.45f});
}

void FloorRenderer::DrawPlayerHand(sf::RenderWindow& w) const
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

void FloorRenderer::DrawEnemies(sf::RenderWindow& w) const
{
	const vector<std::unique_ptr<Enemy>>& enemies = floor->GetEnemies();
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		sf::Vector2f pos { 0.6f + i * 0.15f, 0.45f };
		DrawHealthBar(w, *enemies[i].get(), pos);
	}
}

void FloorRenderer::DrawHealthBar(sf::RenderWindow& w, const Entity& e, sf::Vector2f pos) const
{
	std::string hpTxt = std::to_string(e.GetHp()) + "/" + std::to_string(e.GetMaxHp());
	DrawText(w, hpTxt, sf::Color::White, 0.05, pos);
}

void FloorRenderer::DrawText(sf::RenderWindow& w, const std::string& s, const sf::Color& col, double size, sf::Vector2f pos) const
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

void FloorRenderer::DrawButtons(sf::RenderWindow& w) const
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

void CardRenderer::Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card) const
{
	sf::RectangleShape rectangle{ size };
	rectangle.setFillColor(card.Type() == CardType::Attack ? sf::Color::Red : sf::Color::Blue);
	rectangle.setPosition(pos);

	w.draw(rectangle);
}