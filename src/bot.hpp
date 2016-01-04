#pragma once
#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <list>

#include "interpreter.hpp"
#include "game_state.hpp"

#include "region.hpp"

// Bot base class

class Bot : public Reader, public GameState
{
public:
	Bot();

	// Handle name
	void SetName(const std::string & name);
	const std::string & GetName() const;
	
	int GetVisibleArmy() const;
	int GetEstimatedArmy() const;
	int GetTotalArmyAttackPower() const;
	
	// Get regions
	Regions GetRegions() const;
	Regions GetRegions(const Regions & regions) const;
	Regions GetNeighbors() const;
	Regions GetAffectedRegions() const;
	Regions GetUnaffectedRegions() const;
	Regions GetEffectiveRegions(const Regions & regions) const;
	Regions GetHostileRegions(const Regions & regions) const;
	Regions GetNeutralRegions(const Regions & regions) const;
	static Regions GetConnectedRegions(const Region * region);
	static Regions GetConnectedSuperRegion(const Region * region);
	
	// Clear current bot state
	void ClearState();
	
protected:
	
	// Input
	void onUpdateMap(int region, const std::string & name, int armies);
	
	virtual void onEndRound();
	
	// Regions owned
	Regions m_regions;
	
private:
	
	std::string m_name;
	
	int m_visible_armies;
	int m_estimated_armies;
};

#endif // BOT_HPP
