#include "graphicsrenderer.h"
#include <chrono>
#include <sstream>


static void DrawText(sf::RenderWindow& w, const std::string& s, const sf::Color& col, double size, sf::Vector2f pos)
{
	static sf::Font font;
	static bool loaded = false;
	if (!loaded)
	{
		if (!font.loadFromFile(R"(./BADABB__.TTF)"))
			if (!font.loadFromFile(R"(./assets/BADABB__.TTF)"))
				if (!font.loadFromFile(R"(../assets/BADABB__.TTF)"))
					throw std::runtime_error("Failed to load font");

		Log("Font loaded");
		loaded = true;
	}

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

FloorRenderer::FloorRenderer() 
{
	CreateButton({ 0.85f, 0.8f }, { 0.1f, 0.1f }, "End\nTurn", 0.04f, [this]() {
		input.AddAction([](Floor& f) {
			Log("Ending turn");
			return false;
		});
	});
}

void FloorRenderer::SetFloor(Floor& floor) 
{
	this->floor = &floor;
	playerRenderer = std::make_unique<EntityRenderer>(floor.GetPlayer(), sf::Vector2f{ 0.2f, 0.25f });

	const auto& enemies = floor.GetEnemies();
	for (size_t i = 0; i < enemies.size(); ++i)
	{
		sf::Vector2f pos { 0.6f + i * 0.15f, 0.25f };
		enemyRenderers.emplace_back(enemies[i].get(), pos);
	}
}

void FloorRenderer::Draw(sf::RenderWindow& w)
{
	playerRenderer->Draw(w);

	for (const auto& er : enemyRenderers)
		er.Draw(w);
	
	// Draw cards
	{
		auto player = floor->GetPlayer();
		const auto& hand = player->GetHand();

		CardRenderer cr;
		for(size_t i = 0; i < hand.size(); ++i)
			cr.Draw(w, i, *hand[i], selectedCardIdx == i);
	}

	// Draw Buttons
	for (const auto& btn : buttons)
	{
		sf::RectangleShape rectangle{ btn.size };
		rectangle.setFillColor(sf::Color::Green);
		rectangle.setPosition(btn.pos);

		w.draw(rectangle);
		DrawText(w, btn.txt, sf::Color::White, btn.txtSize, btn.pos);
	}

	// Draw Energy
	{
		auto player = floor->GetPlayer();
		DrawText(w, std::to_string(player->GetEnergy()), sf::Color::White, 0.05, sf::Vector2f{0.1f, 0.8f});
	}
}

void FloorRenderer::Click(sf::RenderWindow& w, const sf::Event::MouseButtonEvent& e)
{
	double sx = double(e.x) / w.getSize().x;
	double sy = double(e.y) / w.getSize().y;
	sf::Vector2f c {static_cast<float>(sx), static_cast<float>(sy)};

	for (const auto& btn : buttons)
	{
		if (sx >= btn.pos.x && sx <= btn.pos.x + btn.size.x &&
			sy >= btn.pos.y && sy <= btn.pos.y + btn.size.y)
		{
			btn.cb();
			selectedCardIdx = -1;
			return;
		}
	}

	// Hittest cards
	auto card = CardRenderer().GetCardIdxFromCoords(sx, sy);
	const auto& hand = floor->GetPlayer()->GetHand();
	if (card >= 0 && card < hand.size())
	{
		if (card == selectedCardIdx)
		{
			floor->PlayCard(card, 0);
			selectedCardIdx = -1;
		} 
		else
		{
			selectedCardIdx = CardRenderer().GetCardIdxFromCoords(sx, sy);
		}
		return;
	}

	// Hittest Player
	if (playerRenderer->HitTest(c) && selectedCardIdx >= 0 && selectedCardIdx < hand.size())
	{
		floor->PlayCard(selectedCardIdx, playerRenderer->entity);
		selectedCardIdx = -1;
		return;
	}

	// Hittest enemies
	for (const auto& e : enemyRenderers)
	{
		if (e.HitTest(c) && selectedCardIdx >= 0 && selectedCardIdx < hand.size())
		{
			floor->PlayCard(selectedCardIdx, e.entity);
			selectedCardIdx = -1;
			return;
		}
	}

	selectedCardIdx = -1;
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

void CardRenderer::Draw(sf::RenderWindow& w, const sf::Vector2f& pos, const Card& card, bool selected) const
{
	auto GetCardColor = [](CardType type)
	{
		switch(type)
		{
		case CardType::Attack:
			return sf::Color::Red;
		case CardType::Defend:
			return sf::Color::Green;
		case CardType::Bash:
			return sf::Color::Red;
		default:
			return sf::Color(160, 160, 160);
		}
	};


	sf::RectangleShape rectangle{ size };
	rectangle.setFillColor(GetCardColor(card.Type()));
	rectangle.setPosition(pos);
	if (selected)
	{
		rectangle.setOutlineColor(sf::Color::Yellow);
		rectangle.setOutlineThickness(0.005f);
	}

	w.draw(rectangle);
	DrawText(w, card.ToString(), sf::Color::White, 0.03, pos);
}

void CardRenderer::Draw(sf::RenderWindow& w, size_t cardIdx, const Card& card, bool selected) const
{
	Draw(w, cardPos + (float)cardIdx * delta, card, selected);
}

int CardRenderer::GetCardIdxFromCoords(double x, double y) const
{
	if (y < cardPos.y || y > cardPos.y + size.y)
		return -1;

	int idx = static_cast<int>((x - cardPos.x) / delta.x);
	double cx = cardPos.x + idx * delta.x;
	if (x >= cx && x <= cx + size.x)
		return idx;
	return -1;
}


EntityRenderer::EntityRenderer(Entity* entity, sf::Vector2f pos) : entity(entity), pos(pos)
{
	if (!texCharacter.loadFromFile("ironclad.png"))
		if (!texCharacter.loadFromFile("./assets/ironclad.png"))
			if (!texCharacter.loadFromFile("../assets/ironclad.png"))
				throw std::runtime_error("Failed to load character texture");

	Log("Character texture loaded");
}

void EntityRenderer::Draw(sf::RenderWindow& w) const
{
	// Draw figure
	{
		sf::RectangleShape rectangle(characterSize);
		rectangle.setTexture(&texCharacter);
		rectangle.setPosition(pos);
		w.draw(rectangle);
	}

	const float hpTextSize = 0.04f;
	const float hpMeterHeight = 0.05f;
	// Draw HealthBar
	{
		const sf::Color hpMeterColor(213, 59, 59);		
		sf::RectangleShape hpMeter({characterSize.x, hpMeterHeight});
		auto hpPos = pos + sf::Vector2f(0, characterSize.y);
		hpMeter.setPosition(hpPos);
		hpMeter.setFillColor(sf::Color::Transparent);
		hpMeter.setOutlineThickness(0.003f);
		hpMeter.setOutlineColor(hpMeterColor);
		w.draw(hpMeter);

		float hpRatio = static_cast<float>(entity->hp) / entity->maxHp;
		sf::RectangleShape hpFill({hpRatio * characterSize.x, hpMeterHeight});
		hpFill.setPosition(hpPos);
		hpFill.setFillColor(hpMeterColor);
		w.draw(hpFill);

		std::string hpTxt = std::to_string(entity->GetHp()) + "/" + std::to_string(entity->GetMaxHp());
		auto txtPos = hpPos; 
		DrawText(w, hpTxt, sf::Color::White, hpTextSize, txtPos);
	}

	// Draw Block
	{
		sf::Vector2f txtPos = pos;
		txtPos.y += characterSize.y + 0.05f;
		std::stringstream txt;
		if (entity->block > 0)
			txt << "Block: " << entity->block;
		if (entity->vulnerable > 0)
			txt << (txt.str().empty() ? "" : "\n") << "vulnerable: " << entity->vulnerable;
		DrawText(w, txt.str(), sf::Color::White, 0.02, txtPos);
	}
}

[[nodiscard]] bool EntityRenderer::HitTest(sf::Vector2f c) const
{
	return c.x >= pos.x && c.x <= pos.x + characterSize.x &&
		c.y >= pos.y && c.y <= pos.y + characterSize.y;
}

void EnemyRenderer::Draw(sf::RenderWindow& w) const
{
	EntityRenderer::Draw(w);
	
	Enemy* e = static_cast<Enemy*>(entity);
	auto intTxt = e->GetIntention().ToString();
	auto txtPos = pos;
	txtPos.y -= 0.03f;
	DrawText(w, intTxt, sf::Color::White, 0.03, txtPos);
}