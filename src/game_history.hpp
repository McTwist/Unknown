#pragma once
#ifndef GAME_HISTORY_HPP
#define GAME_HISTORY_HPP

#include "army_movements.hpp"
#include "army_placements.hpp"

#include <vector>

/*
 * Game History
 * A structure to keep track of the game
 */

// Forwards
class Region;
class Bot;

// History of a region
class RegionHistory
{
public:

	RegionHistory(const Region * region, const Bot * owner, int army, int round);

	// Get
	const Region * GetRegion() const;
	const Bot * GetOwner() const;
	int GetArmy() const;
	int GetRound() const;

	// Operators
	bool operator==(const Region * region) const;

private:

	const Region * m_region;
	const Bot * m_owner;
	int m_army;
	int m_round;
};

typedef std::vector<RegionHistory> RegionHistoryList;

// History of a round
class RoundHistory
{
public:

	RoundHistory();
	RoundHistory(int round);

	// Get
	// Get history of region
	// Return 0 if history does not exist
	const RegionHistory * GetRegion(const Region * region) const;
	// Get a list of history of region
	RegionHistoryList GetRegionHistory(const Region * region) const;
	// Get region history
	const RegionHistoryList & GetHistories() const;
	// Get all army movements
	const ArmyMovements & GetMovements() const;
	// Get all army placements
	const ArmyPlacements & GetPlacements() const;
	// Get army movements depending on region
	ArmyMovements GetRegionMovements(const Region * region) const;
	// Get army placement depending on region
	int GetRegionPlacement(const Region * region) const;

	// Get round number
	int GetRound() const;

	// Set
	void AddRegion(const Region * region, const Bot * owner, int army);
	void AddMovement(const Region * from, const Region * to, int army);
	void AddMovements(const ArmyMovements & movements);
	void AddPlacement(const Region * region, int army);
	void AddPlacement(int region, int army);
	void AddPlacements(const ArmyPlacements & placements);

private:

	RegionHistoryList m_regions;
	ArmyMovements m_movements;
	ArmyPlacements m_placements;
	int m_round;
};

// History of a full game
class GameHistory
{
public:

	GameHistory();

	// Get current round
	int GetRound() const;
	// Get max rounds
	int GetMaxRounds() const;
	// Get a round
	const RoundHistory * GetRound(int round) const;
	// Get a round depending on region
	// This might return 0 if no history exists for the region
	const RoundHistory * GetRound(const Region * region) const;

	// Set how many rounds
	void SetRounds(int rounds);

	// Get bot current regions owned
	RegionHistoryList GetBotRegions(const Bot * owner) const;

	// Modify current round
	// Add region that have changed
	void AddRegion(const Region * region);
	void AddRegion(const Region * region, const Bot * owner, int army);
	// Add movement that occurred
	void AddMovements(const ArmyMovements & movement);
	// Add placement that occured
	void AddPlacements(const ArmyPlacements & placement);
	// Move on to next round
	void NextRound();

private:

	std::vector<RoundHistory> m_rounds;
	int m_round;
};

#endif // GAME_HISTORY_HPP