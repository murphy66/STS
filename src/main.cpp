#include "entity.h"
#include "card.h"
#include "floor.h"
#include "graphicsrenderer.h"
#include "consolerenderer.h"
#include "qlearn.h"
#include <SFML/window.hpp>
#include <SFML/graphics.hpp>
#include <thread>

using namespace std;

void ConsoleRenderedGame()
{
    auto player = Player::CreateIronclad();
    vector<unique_ptr<Enemy>> es;
    es.push_back(move(make_unique<JawWorm>()));
    es.push_back(move(make_unique<JawWorm>()));
    ConsoleInput input;
    Floor floor(player.get(), move(es));
    input.DoFloor(floor);
}

static void PrintSuggestion(Floor& floor, QL::QLearn<QL::QState, QL::QMove>* ql) 
{
    if (!ql)
        return;

    auto state = QL::GetState(floor);
    auto rewards = ql->GetStateRewards(state);
    auto move = std::max_element(rewards.begin(), rewards.end());
    std::cout << move - rewards.begin() << std::endl;
    for (int i = 0; i < Player::nDrawCard+1; ++i)
    {
        std::cout << rewards[i] << ", ";
    }
    std::cout << std::endl;
}

void GraphicsRenderedGame(QL::QLearn<QL::QState, QL::QMove>* ql = nullptr)
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Slay The Spire");
    sf::View view(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    window.setView(view);

    FloorRenderer renderer;

    auto player = Player::CreateIronclad();
    vector<unique_ptr<Enemy>> enemies;
    enemies.push_back(move(make_unique<JawWorm>()));
    Floor floor(player.get(), move(enemies));
    renderer.SetFloor(floor);

    while (window.isOpen() && !floor.IsFloorDone())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    renderer.Click(window, event.mouseButton);
                    PrintSuggestion(floor, ql);
                }
            }
        }

        window.clear(sf::Color::Black);

        renderer.Draw(window);
        renderer.UpdateGameState();

        window.display();
    }
}

int main()
{
    //ConsoleRenderedGame();

    auto ql = QL::Learn();
    GraphicsRenderedGame(&ql);

    return 0;
}
