#pragma once
#ifndef GAME_HISTORY_HPP
#define GAME_HISTORY_HPP

#include "army_movement.hpp"

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

// History of a round
class RoundHistory
{
public:

	RoundHistory();
	RoundHistory(int round);

	// Get
	const RegionHistory * GetRegion(const Region * region) const;
	const std::vector<const ArmyMovement> & GetMovements() const;

	// Set
	void AddRegion(const Region * region, const Bot * owner, int army);
	void AddMovements(const ArmyMovement & movements);

private:

	std::vector<const RegionHistory> m_regions;
	std::vector<const ArmyMovement> m_movements;
	int m_round;
};

// History of a full game
class GameHistory
{
public:

	GameHistory();

	// Get current round
	int GetRound() const;
	// Get a round
	const RoundHistory * GetRound(int round) const;
	const RoundHistory * GetRound(const Region * region) const;

	// Set how many rounds
	void SetRounds(int rounds);

	// Modify current round
	// Add region that have changed
	void AddRegion(const Region * region, const Bot * owner, int army);
	// Add movement that occurred
	void AddMovements(const ArmyMovement & movement);
	// Move on to next round
	void NextRound();

private:

	RoundHistory & GetRound(int round);

	std::vector<RoundHistory> m_rounds;
	int m_round;
};

#endif // GAME_HISTORY_HPP