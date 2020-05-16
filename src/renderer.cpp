#include "renderer.h"

void CardRenderer::Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card)
{
	sf::RectangleShape rectangle{ size };
	rectangle.setFillColor(card.Type() == CardType::Attack ? sf::Color::Red : sf::Color::Blue);
	rectangle.setPosition(pos);

	w.draw(rectangle);
}