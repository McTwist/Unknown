#pragma once
#ifndef UNKNOWN_HPP
#define UNKNOWN_HPP

#include <map>

#include "bot.hpp"
#include "interpreter.hpp"

struct PlacementMove
{
	PlacementMove(int r, int a)
	: region(r), armies(a)
	{}
	int region;
	int armies;
};

struct AttackTransferMove
{
	AttackTransferMove(int s, int t, int a)
	: source(s), target(t), armies(a)
	{}
	int source;
	int target;
	int armies;
};

// Pairing together two regions
/*template<typename T>
struct DoublePair
{
	DoublePair(const T & f, const T & s)
	: first(f), second(s)
	{}
	
	bool operator<(const DoublePair & left, const DoublePair & right) const
	{
		if (left.first != right.first)
			return left.first < right.first;
		else
			return left.second < right.second;
	}
	
	const T first;
	const T second;
};*/

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
	// desc<source, target, armies>
	typedef std::pair<int, int> Destination;
	typedef std::map<Destination, int> MovementList;
	// desc<region, armies>
	typedef std::map<int, int> AttackRegionList;
	
	// Current moves
	PlacementList m_placement;
	MovementList m_moves;
	AttackRegionList m_to_regions;
};

#endif // UNKNOWN_HPP