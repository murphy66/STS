#pragma once

#include "card.h"
#include "entity.h"
#include "random.h"
#include "floor.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <functional>
#include <map>
#include <numeric>
#include <iostream>
#include <thread>


// TODO: move elsewhere
template<class T, class K>
std::ostream& operator<<(std::ostream& os, const std::pair<T, K>& p)
{
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << "["; 
    for (int i = 0; i < v.size(); ++i) { 
        os << v[i]; 
        if (i != v.size() - 1) 
            os << ", "; 
    } 
    os << "]"; 
    return os; 
}


namespace QL
{

std::vector<int> GetState(Entity& e)
{
	//return {e.maxHp, e.hp, e.block, e.vulnerable};
    return {e.hp, e.block};
}

std::vector<int> GetState(Player& p)
{
	std::vector<int> state = GetState(static_cast<Entity&>(p));
	state.push_back(p.GetEnergy());
	for (int i = 0; i < Player::nDrawCard; ++i)
	{
		const auto& hand = p.GetHand();
		if (i < hand.size())
			state.push_back(static_cast<int>(hand[i]->type));
		else
			state.push_back(-1);
	}
	return state;
}

std::vector<int> GetState(Enemy& e)
{
    auto state = GetState(static_cast<Entity&>(e));
	state.push_back(static_cast<int>(e.GetIntention().type));
	return state;
}

std::vector<int> GetState(Floor& f)
{
	auto state = GetState(*f.GetPlayer());
	auto enemyState = GetState(*f.GetEnemies()[0]);
	state.insert(state.end(), enemyState.begin(), enemyState.end());
	return state;
}

std::string GetStateHeader()
{
    std::string ret = "PlayerHP\tPlayerBlock\tEnergy\t";
    for (auto i = 0; i < Player::nDrawCard; ++i) 
        ret += "Card\t";
    ret += "MonsterHp\tMonsterBlock\tMonsterAction";
    return ret;
}

template<class State, class Move>
class QLearn
{
public:
	// returns the next state and the reward
	using MoveFunc = std::function<std::pair<State, double>(const Move& m)>;
	using StateFunc = std::function<State()>;
    using GameEndedFunc = std::function<bool()>;

private:
	std::map<State, std::vector<double>> rewardTbl;

	double lrate = 0.1;
	double disc = 0.95;
	double gambl = 1.0;
	double gamblDecay = 0.0;
    double minGambl = 0.1;

	std::vector<Move> allMoves;
	StateFunc stateFunc;
	MoveFunc moveFunc;
    GameEndedFunc gameEndedFunc;

public:
	QLearn(const std::vector<Move>& allMoves, const StateFunc& stateFunc, const MoveFunc& moveFunc,
            const GameEndedFunc& gameEndedFunc, int games) : 
		allMoves(allMoves), stateFunc(stateFunc), moveFunc(moveFunc), gameEndedFunc(gameEndedFunc)
	{
        gamblDecay = std::pow(minGambl, 1.0/games); 
	}

	[[nodiscard]] std::vector<double>& GetStateRewards(const State& state)
	{
		auto& rewards = rewardTbl[state];
		if (rewards.empty())
			rewards.resize(allMoves.size());
		return rewards;
	}

    static size_t GetWeightedRandom(const vector<double>& weights)
    {
        double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
        auto rand = Rand(100000) / sum;
        for (size_t i = 0; i < weights.size(); ++i)
        {
            if (rand < weights[i])
                return i;
            rand -= weights[i];
        }
        return weights.size() - 1;
    }

    std::pair<Move, double> GetMove(const State& currState, bool allowRand = true)
	{
        auto randMove = [&]() { return std::make_pair(allMoves[Rand((int)allMoves.size()) - 1], 0.0); };
        if (allowRand)
        {
            bool rnd = gambl >= Rand(100)/100.0;
            if (rnd)
            {
                // TODO: check if move is possible.
                return randMove();
            }
        }

		// Iterate over valid moves. Get best valid moves or return random
		const auto& rewards = GetStateRewards(currState);
        auto moveIdx = GetWeightedRandom(rewards);
        return std::make_pair(allMoves[moveIdx], rewards[moveIdx]); 

		/*const Move* bestMove = nullptr;
		double bestReward = 0.0;
		for (int i = 0; i < rewards.size(); ++i)
		{
			double reward = rewards[i];
			if (!bestMove || (reward > bestReward))
			{
				bestMove = &allMoves[i];
				bestReward = reward;
			}
		}
		return std::make_pair(*bestMove, bestReward);*/
	}

	Move QMove(const State& currState)
	{
        auto [m, r] = GetMove(currState);
		std::vector<double>& moveRewards = GetStateRewards(currState);
		double curr = moveRewards[m];
		auto [nextState, reward] = moveFunc(m);
		auto itNext = std::max_element(rewardTbl[nextState].begin(), rewardTbl[nextState].end());
		double next = itNext == rewardTbl[nextState].end() ? 0 : *itNext;
		moveRewards[m] = curr + lrate * (reward + disc * next - curr);
		return m;
	}

	void PrintRewardTbl() const
	{
        std::cout << GetStateHeader() << std::endl;
		std::cout << "\t";
		for (const auto& m : allMoves)
			std::cout << m << "\t";
		std::cout << std::endl;

		for (const auto& [state, rewards] : rewardTbl)
		{
			std::cout << state;
			for (auto r : rewards)
				std::cout << "\t" << r;
			std::cout << std::endl;
		}
	}

	void Learn()
	{
		/*int cnt = 0;
		while (gambl > minGambl)
		{
			auto move = QMove(stateFunc());
			if (++cnt % 100000 == 0)
				std::cout << "Gamboool " << gambl << std::endl;
		}
		
		PrintRewardTbl();*/
        while(!gameEndedFunc())
        {
            QMove(stateFunc());
        }
        gambl *= gamblDecay;
	}
};

using QMove = int;
using QState = std::vector<int>;

QLearn<QState, QMove> Learn()
{
	auto player = Player::CreateIronclad();
	vector<unique_ptr<Enemy>> enemies;
	enemies.push_back(move(std::make_unique<JawWorm>()));
	Floor f(player.get(), move(enemies));
    f.StartFloor();

    auto restartFloor = [&]() {
        player = Player::CreateIronclad();
	    vector<unique_ptr<Enemy>> enemies;
	    enemies.push_back(move(std::make_unique<JawWorm>()));
	    f = Floor(player.get(), move(enemies));
        f.StartFloor();
    };

	std::vector<QMove> allMoves;
	for (int i = 0; i < Player::nDrawCard; ++i)
		allMoves.push_back(i);
	allMoves.push_back(static_cast<int>(allMoves.size())); // end turn

    QLearn<QState, QMove>::StateFunc stateFunc = [&](){ return GetState(f); };
    QLearn<QState, QMove>::MoveFunc moveFunc = [&](const QMove& m){
        int prevHp = f.GetPlayer()->GetHp();

		if (f.GetState() != Floor::FloorState::FloorEnded)
		{
			if (m == allMoves.size()-1)
			{
				f.EndTurn();
				f.DoEnemyTurn();
                //double reward = f.GetPlayer()->GetHp() - prevHp;
				//return std::make_pair(GetState(f), reward);
			}
            else
            {
                int cardIdx = m;
                bool success = f.PlayCard(cardIdx, 0);
                if (!success)
                    return std::make_pair(GetState(f), -10.0);
            }
        }

        //const auto& enemies = f.GetEnemies();
        //auto enemyHps = std::accumulate(enemies.begin(), enemies.end(), 0, [](int acc, const std::unique_ptr<Enemy>& e){ return acc + e->GetHp(); });
        double reward = 0.0; //f.GetPlayer()->GetHp() - prevHp;
        if (f.GetState() == Floor::FloorState::FloorEnded)
        {
            if (f.GetPlayer()->IsAlive())
                reward = f.GetPlayer()->GetHp();
            else
                reward = -10.0;
        }
        auto ret = std::make_pair(GetState(f), reward);
        return ret;
    };
    QLearn<QState, QMove>::GameEndedFunc gameEndedFunc = [&]() { return f.IsFloorDone(); };

    Log("Starting to learn...");

    constexpr int nGames = 10000000;
    QLearn ql(allMoves, stateFunc, moveFunc, gameEndedFunc, nGames); 
    for (int i = 0; i < nGames; ++i)
    {
        if (i % (nGames / 100) == 0)
        {
            std::cout << i / (nGames / 100) << "%" << std::endl;
            //ql.PrintRewardTbl();
        }
        ql.Learn();
        restartFloor();
    }
    //ql.PrintRewardTbl();
    Log("Finished learning");
    return ql;
}

}
