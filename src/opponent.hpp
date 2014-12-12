#pragma once
#ifndef OPPONENT_HPP
#define OPPONENT_HPP

#include "bot.hpp"

// Opponent information

class Opponent : public Bot
{
	void onOpponentPlaceArmies(const std::string & name, int region, int amount);
	void onOpponentAttackTransfer(const std::string & name, int source_region, int target_region,int amount);
};

#endif // OPPONENT_HPP
