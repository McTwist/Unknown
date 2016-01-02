#include "game.hpp"

#include "game_state.hpp"

// Global object
Game * g_game = 0;

Game::Game()
{
	m_interpreter.AddReader(this);
	m_interpreter.AddReader(&m_map);
	
	// Add state
	AddGameState(&m_map);
	
	// Add neutral
	AddBot(m_neutral = new Neutral);
}

Game::~Game()
{
	m_neutral = 0;
	ClearBots();
}

// Run game code
void Game::Run()
{
	for (;!GameOver();)
	{
		m_interpreter.Update();
	}
}

// Get map
Map * Game::GetMap()
{
	return &m_map;
}

// Get game history
GameHistory * Game::GetHistory()
{
	return &m_history;
}

// Get bot from name
Bot * Game::GetBot(const std::string & name) const
{
	BotMap::const_iterator it = m_bots.find(name);
	return it != m_bots.end() ? it->second : 0;
}

// Get neutral bot
// Note: Created to make it easier and faster to get this
Bot * Game::GetNeutral() const
{
	return m_neutral;
}

// Check if game is over
bool Game::GameOver()
{
	// Why even bother? It gets destroyed when done anyway...
	return false;
}

// Add bot to the system
void Game::AddBot(Bot * bot)
{
	m_interpreter.AddReader(bot);
	m_bots.insert(std::make_pair(bot->GetName(), bot));
	// Not sure if put here
	AddGameState(bot);
}

// Remove all bots
void Game::ClearBots()
{
	for (BotMap::iterator it = m_bots.begin(); it != m_bots.end(); ++it)
		delete it->second;
	m_bots.clear();
}

// Adds a new game state
void Game::AddGameState(GameState * state)
{
	m_states.push_back(state);
}

// Create your bot
void Game::onSettingsYourBot(const std::string & name)
{
	Unknown * bot = new Unknown;
	bot->SetName(name);
	m_interpreter.AddWriter(bot);
	AddBot(bot);
}

// Create opponent bot
void Game::onSettingsOpponentBot(const std::string & name)
{
	Opponent * bot = new Opponent;
	bot->SetName(name);
	AddBot(bot);
}

// Start of game
// Note: This relies on the underlying structure of Interpreter to be called first
void Game::onSettingsStartingArmies(int /*amount*/)
{
	// End this round
	if (m_history.GetRound() > 0)
		for (std::list<GameState *>::iterator it = m_states.begin(); it != m_states.end(); ++it)
			(*it)->onEndRound();

	// New history
	m_history.NextRound();

	// Start next round
	for (std::list<GameState *>::iterator it = m_states.begin(); it != m_states.end(); ++it)
		(*it)->onStartRound(m_history.GetRound());
}

// Setting max rounds
void Game::onSettingsMaxRounds(int amount)
{
	m_history.SetRounds(amount);
}
