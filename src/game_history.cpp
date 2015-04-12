#include "game_history.hpp"

#include "region.hpp"

// There's no region available
struct IfHasNoRegion
{
	IfHasNoRegion(const Region * reg) : region(reg) {}
	const Region * region;
	// The operator used
	bool operator()(const RegionHistory & history) const
	{
		return history.GetRegion() != region;
	}
};

/*
 * Region History
 */
RegionHistory::RegionHistory(const Region * region, const Bot * owner, int army, int round)
	: 
	m_region(region), 
	m_owner(owner), 
	m_army(army), 
	m_round(round)
{
}

// Get the region
const Region * RegionHistory::GetRegion() const
{
	return m_region;
}

// Get bot owner
const Bot * RegionHistory::GetOwner() const
{
	return m_owner;
}

// Get army
int RegionHistory::GetArmy() const
{
	return m_army;
}

// Get round number
int RegionHistory::GetRound() const
{
	return m_round;
}

// To easier locate the region
bool RegionHistory::operator==(const Region * region) const
{
	return m_region == region;
}

/*
 * Round History
 */
RoundHistory::RoundHistory()
	: m_round(0)
{
}

RoundHistory::RoundHistory(int round)
	: m_round(round)
{
}

// Get latest region history from region
const RegionHistory * RoundHistory::GetRegion(const Region * region) const
{
	// Locate the region that is placed in history
	RegionHistoryList::const_iterator it = std::find(m_regions.begin(), m_regions.end(), region);
	// Return only if it exists
	return it == m_regions.end() ? 0 : &*it;
}

// Get region history list
const RegionHistoryList RoundHistory::GetRegionHistory(const Region * region) const
{
	RegionHistoryList history = m_regions;
	// Remove the regions that does not contain the region we're looking for
	history.erase(
		std::remove_if(
			history.begin(), history.end(), 
			IfHasNoRegion(region)), 
		history.end());
	return history;
}

// Get round movements
const ArmyMovements & RoundHistory::GetMovements() const
{
	return m_movements;
}

// Get region movements this round
ArmyMovements RoundHistory::GetRegionMovements(const Region * region) const
{
	// Create a new momovement list
	ArmyMovements movements(m_movements.GetMovementsFromRegion(region));
	movements.AddMovements(m_movements.GetMovementsToRegion(region));
	return movements;
}

// Add region to round
void RoundHistory::AddRegion(const Region * region, const Bot * owner, int army)
{
	m_regions.push_back(RegionHistory(region, owner, army, m_round));
}

// Add movements to round
void RoundHistory::AddMovements(const ArmyMovements & movements)
{
	m_movements.AddMovements(movements.GetMovements());
}

/*
 * Game History
 */
GameHistory::GameHistory()
	: m_round(1)
{
}

int GameHistory::GetRound() const
{
	return m_round;
}

// Get a round depending on number
const RoundHistory * GameHistory::GetRound(int round) const
{
	// Quick test to avoid memory leaks
	return round < (int)m_rounds.size() ? &m_rounds[round - 1] : 0;
}

// Get a round depending on region
const RoundHistory * GameHistory::GetRound(const Region * region) const
{
	for (int i = m_round - 1; i > 0; --i)
	{
		const RoundHistory & round = m_rounds[i];
		// Check if there's history of the region
		if (round.GetRegion(region))
			return &round;
	}
	return 0;
}

// Set rounds available
void GameHistory::SetRounds(int rounds)
{
	if (rounds > (int)m_rounds.size())
	{
		// Remove unneeded rounds
		while ((int)m_rounds.size() > rounds)
			m_rounds.pop_back();
	}
	else
	{
		m_rounds.reserve(rounds);
		// Create the rounds that are missing
		for (int i = m_rounds.size(); i < rounds; ++i)
			m_rounds.push_back(RoundHistory(i));
	}
}

// Add region to current round
void GameHistory::AddRegion(const Region * region)
{
	m_rounds[m_round - 1].AddRegion(region, region->GetOwner(), region->GetArmies());
}

// Add a region to current round
void GameHistory::AddRegion(const Region * region, const Bot * owner, int army)
{
	m_rounds[m_round - 1].AddRegion(region, owner, army);
}

// Add movements to a current round
void GameHistory::AddMovements(const ArmyMovements & movement)
{
	m_rounds[m_round - 1].AddMovements(movement);
}

// Increase round number
void GameHistory::NextRound()
{
	++m_round;
}
