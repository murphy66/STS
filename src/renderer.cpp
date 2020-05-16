#include "renderer.h"

FloorRenderer::FloorRenderer(const Floor& floor) : floor(&floor) 
{
	if (!font.loadFromFile(R"(./BADABB__.TTF)"))
	{
		if (!font.loadFromFile(R"(./assets/BADABB__.TTF)"))
			throw std::runtime_error("Failed to load font");
	}

	Log("Font loaded");
}

void FloorRenderer::Draw(sf::RenderWindow& w) const
{
	DrawPlayer(w);
}

void FloorRenderer::DrawPlayer(sf::RenderWindow& w) const
{
	DrawPlayerHand(w);

	//TODO: Healthbar
	auto player = floor->GetPlayer();
	std::string hpTxt = std::to_string(player->GetHp()) + "/" + std::to_string(player->GetMaxHp());
	DrawText(w, hpTxt, sf::Color::White, 0.05, {0.25f, 0.5f});
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

void CardRenderer::Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card) const
{
	sf::RectangleShape rectangle{ size };
	rectangle.setFillColor(card.Type() == CardType::Attack ? sf::Color::Red : sf::Color::Blue);
	rectangle.setPosition(pos);

	w.draw(rectangle);
}