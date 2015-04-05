#include "game_history.hpp"

#include "region.hpp"

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

const Region * RegionHistory::GetRegion() const
{
	return m_region;
}

const Bot * RegionHistory::GetOwner() const
{
	return m_owner;
}

int RegionHistory::GetArmy() const
{
	return m_army;
}

int RegionHistory::GetRound() const
{
	return m_round;
}

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

const RegionHistory * RoundHistory::GetRegion(const Region * region) const
{
	// Locate the region that is placed in history
	std::vector<RegionHistory>::const_iterator it = std::find(m_regions.begin(), m_regions.end(), region);
	// Return only if it exists
	return it == m_regions.end() ? 0 : &*it;
}

const std::vector<ArmyMovement> & RoundHistory::GetMovements() const
{
	return m_movements;
}

void RoundHistory::AddRegion(const Region * region, const Bot * owner, int army)
{
	m_regions.push_back(RegionHistory(region, owner, army, m_round));
}

void RoundHistory::AddMovements(const ArmyMovement & movements)
{
	m_movements.push_back(movements);
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

const RoundHistory * GameHistory::GetRound(int round) const
{
	// Quick test to avoid memory leaks
	return round < (int)m_rounds.size() ? &m_rounds[round - 1] : 0;
}

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
		// Create the rounds that are missing
		for (int i = m_rounds.size(); i < rounds; ++i)
			m_rounds.push_back(RoundHistory(i));
	}
}

void GameHistory::AddRegion(const Region * region, const Bot * owner, int army)
{
	m_rounds[m_round - 1].AddRegion(region, owner, army);
}

void GameHistory::AddMovements(const ArmyMovement & movement)
{
	m_rounds[m_round - 1].AddMovements(movement);
}

void GameHistory::NextRound()
{
	++m_round;
}


