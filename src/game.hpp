#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include "interpreter.hpp"
#include "map.hpp"
#include "game_history.hpp"

// Bots
#include "opponent.hpp"
#include "neutral.hpp"
// Mine
#include "unknown.hpp"

class Game : public Reader
{
public:
	Game();
	~Game();
	
	void Run();
	
	// Global getters
	Map * GetMap();
	GameHistory * GetHistory();
	Bot * GetBot(const std::string & name) const;
	Bot * GetNeutral() const;
	
private:
	
	// Check if game is over
	bool GameOver();
	
	void AddBot(Bot * bot);
	void ClearBots();
	
	void AddGameState(class GameState * state);
	
	// Input
	void onSettingsYourBot(const std::string & name);
	void onSettingsOpponentBot(const std::string & name);
	void onSettingsStartingArmies(int amount);
	void onSettingsMaxRounds(int amount);

	// Variables
	Interpreter m_interpreter;
	Map m_map;
	
	typedef std::map<std::string, Bot *> BotMap;
	
	BotMap m_bots;
	Neutral * m_neutral;
	
	typedef std::list<class GameState *> GameStateList;
	
	GameStateList m_states;

	GameHistory m_history;
};

// Keeping this global
extern Game * g_game;

#endif // GAME_HPP
