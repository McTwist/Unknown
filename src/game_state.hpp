#pragma once
#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

// CGame state that updates game related information

class GameState
{
public:
	virtual void onStartRound(int round) {}
	virtual void onEndRound() {}
};

#endif // GAME_STATE_HPP