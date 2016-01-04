#pragma once
#ifndef ARMY_PLACEMENTS_HPP
#define ARMY_PLACEMENTS_HPP

#include <map>

class Region;

// Current placements
typedef std::map<int, int> ArmyPlacementList;

class ArmyPlacements
{
public:

	ArmyPlacements();

	// Set current max placement
	void SetCurrentPlacement(int armies);

	// Clear placements
	void Clear();

	// Add armies to region
	int AddArmies(const Region * region, int armies);
	int AddArmies(int region, int armies);
	// Set armies for a region
	void SetArmies(const Region * region, int armies);
	void SetArmies(int region, int armies);
	// Add placements
	void AddPlacements(const ArmyPlacementList & placements);

	// Get available armies left to place
	int GetAvailableArmies() const;

	// Get placement for a region
	int GetRegionPlacement(const Region * region) const;

	// Get list of placements
	const ArmyPlacementList & GetPlacements() const;

private:

	int m_armies;

	ArmyPlacementList m_placement;
};

#endif // ARMY_PLACEMENTS_HPP
