#pragma once
#ifndef ARMY_MOVEMENTS_HPP
#define ARMY_MOVEMENTS_HPP

#include <map>
#include <vector>

class Region;

// ArmyMovement structure
struct ArmyMovement
{
	const Region * from;
	const Region * to;
	int armies;
};

// Current movements
typedef std::vector<ArmyMovement> ArmyMovementList;

class ArmyMovements
{
public:

	ArmyMovements();
	ArmyMovements(const ArmyMovements & movements);
	ArmyMovements(const ArmyMovementList & movements);

	// Reset movements
	void Reset();

	// Add movement
	void AddMovement(const Region * from, const Region * to, int armies);
	void AddMovement(const ArmyMovement & movement);
	void AddMovements(const ArmyMovementList & movements);
	
	// Remove movement
	void RemoveMovement(const Region * from, const Region * to);

	// Get list of movement
	const ArmyMovementList & GetMovements() const;
	
	int GetArmiesFromMovement(const Region * from, const Region * to) const;
	ArmyMovementList GetMovementsFromRegion(const Region * from) const;
	ArmyMovementList GetMovementsToRegion(const Region * to) const;
	int GetArmiesFromRegion(const Region * from) const;
	int GetArmiesToRegion(const Region * to) const;

private:

	ArmyMovementList::iterator GetMovement(const Region * from, const Region * to);
	ArmyMovementList::const_iterator GetMovement(const Region * from, const Region * to) const;

	ArmyMovementList m_army_movement;

};

#endif // ARMY_MOVEMENTS_HPP