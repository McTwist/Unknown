#pragma once
#ifndef UNKNOWN_HPP
#define UNKNOWN_HPP

#include <map>

#include "bot.hpp"
#include "interpreter.hpp"
#include "army_movements.hpp"
#include "army_placements.hpp"
#include "game_predictor.hpp"

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
	
	// Current moves
	ArmyPlacements m_placements;

	// Advanced movement handler
	ArmyMovements m_movements;

	// Predicting the game
	GamePredictor m_predictor;
};

#endif // UNKNOWN_HPP
