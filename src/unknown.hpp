#pragma once
#ifndef UNKNOWN_HPP
#define UNKNOWN_HPP

#include <map>

#include "bot.hpp"
#include "interpreter.hpp"
#include "army_movements.hpp"

// My bot

class Unknown : public Bot, public Writer
{
public:
	Unknown();
	
private:
	
	// Input
	void onPickStartingRegions(float time, const std::vector<int> & regions);
	void onPickStartingRegion(float time, const std::vector<int> & regions);
	void onSettingsStartingArmies(int amount);
	void onGoPlaceArmies(float time);
	void onGoAttackTransfer(float time);
	
	// State
	void onStartRound(int round);
	
	// Handle output
	void PlaceArmy(Region * region, int amount);
	void MoveArmy(Region * source, Region * target, int amount);
	
	int GetAttackRegion(const Region * region) const;
	
	void SendPlaceArmies();
	void SendAttackTransfer();
	
	// How many armies that can be used this turn
	int m_armies;
	
	// desc<region, armies>
	typedef std::map<int, int> PlacementList;
	
	// Current moves
	PlacementList m_placement;

	// Advanced movement handler
	ArmyMovements m_movements;
};

#endif // UNKNOWN_HPP