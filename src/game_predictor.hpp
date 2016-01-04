#pragma once
#ifndef GAME_PREDICTOR_HPP
#define GAME_PREDICTOR_HPP

#include <vector>

class Bot;
class Region;
typedef std::vector<Region *> Regions;

/*
 * Game Predictor
 * Analyzes histories and current situation to determine various variables
 * like possible army count per round, movements and placements
 */

class GamePredictor
{
public:

	// Set maximum armies that anyone can get each round
	void SetMaxArmyPerRound(int max_army);

	// Get bot current regions
	Regions GetBotRegions(const Bot * bot) const;
	// Get how many armies a bot could get this round
	int GetBotArmyPerRound(const Bot * bot) const;
	// Get how many armies a bot could have
	int GetBotCurrentArmy(const Bot * bot) const;
	// Get how many active armies a bot could have
	int GetBotCurrentActiveArmy(const Bot * bot) const;
	// Get how many armies a region could have
	int GetRegionCurrentArmy(const Region * region) const;

private:

	int m_max_army_round;
};

#endif // GAME_PREDICTOR_HPP
