#include "army_placements.hpp"

#include "region.hpp"

ArmyPlacements::ArmyPlacements()
: m_armies(0)
{
}

// Set armies to place
void ArmyPlacements::SetCurrentPlacement(int armies)
{
	m_armies = armies;
}

// Clear placements
void ArmyPlacements::Clear()
{
	m_armies = 0;
	m_placement.clear();
}

// Add armies to region
// Return actual amount of armies added
int ArmyPlacements::AddArmies(const Region * region, int armies)
{
	return AddArmies(region->GetId(), armies);
}

// Add native region placement
int ArmyPlacements::AddArmies(int region, int armies)
{
	// Don't even dare go there
	if (armies > m_armies)
		armies = m_armies;
	// Add to other placements
	SetArmies(region, armies);
	// Remove armies on hand
	m_armies -= armies;

	return armies;
}

// Set armies for a region
void ArmyPlacements::SetArmies(const Region * region, int armies)
{
	SetArmies(region->GetId(), armies);
}

// Set native region placement
void ArmyPlacements::SetArmies(int region, int armies)
{
	// Add to other placements
	m_placement[region] += armies;
}

// Add placements to placements
void ArmyPlacements::AddPlacements(const ArmyPlacementList & placement)
{
	m_placement.insert(placement.begin(), placement.end());
}

// Get available armies to place
int ArmyPlacements::GetAvailableArmies() const
{
	return m_armies;
}

// Get placement for region
int ArmyPlacements::GetRegionPlacement(const Region * region) const
{
	ArmyPlacementList::const_iterator it = m_placement.find(region->GetId());
	if (it == m_placement.end())
		return 0;
	return it->second;
}

// Get current placements
const ArmyPlacementList & ArmyPlacements::GetPlacements() const
{
	return m_placement;
}
