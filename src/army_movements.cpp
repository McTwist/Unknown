#include "army_movements.hpp"

#include "region.hpp"

// Find depending on super const Region and bot priority
struct FindMovement
{
	FindMovement(const Region * f, const Region * t)
	: from(f), to(t)
	{}
	
	const Region * from;
	const Region * to;
	
	inline bool operator()(const ArmyMovement & movement)
	{
		return movement.from == from && movement.to == to;
	}
};

ArmyMovements::ArmyMovements()
{
}

ArmyMovements::ArmyMovements(const ArmyMovements & movements)
{
	m_army_movement = movements.m_army_movement;
}

ArmyMovements::ArmyMovements(const ArmyMovementList & movements)
{
	AddMovements(movements);
}

// Reset the movements
void ArmyMovements::Reset()
{
	m_army_movement.clear();
}

// Add new movement
void ArmyMovements::AddMovement(const Region * from, const Region * to, int armies)
{
	ArmyMovementList::iterator it = GetMovement(from, to);
	// Append on old one
	if (it != m_army_movement.end())
	{
		it->armies += armies;
	}
	// Add new
	else
	{
		ArmyMovement movement;
		movement.from = from;
		movement.to = to;
		movement.armies = armies;
		m_army_movement.push_back(movement);
	}
}

// Add new movement
void ArmyMovements::AddMovement(const ArmyMovement & movement)
{
	AddMovement(movement.from, movement.to, movement.armies);
}

// Add a list of movements
void ArmyMovements::AddMovements(const ArmyMovementList & movements)
{
	m_army_movement.insert(m_army_movement.end(), movements.begin(), movements.end());
}

// Remove movement
void ArmyMovements::RemoveMovement(const Region * from, const Region * to)
{
	ArmyMovementList::iterator it = GetMovement(from, to);
	// Found it
	if (it != m_army_movement.end())
	{
		m_army_movement.erase(it);
	}
}

// Get current list of movements
const ArmyMovementList & ArmyMovements::GetMovements() const
{
	return m_army_movement;
}

// Get movement that exist already
ArmyMovementList::iterator ArmyMovements::GetMovement(const Region * from, const Region * to)
{
	return std::find_if(m_army_movement.begin(), m_army_movement.end(), FindMovement(from, to));
}
ArmyMovementList::const_iterator ArmyMovements::GetMovement(const Region * from, const Region * to) const
{
	return std::find_if(m_army_movement.begin(), m_army_movement.end(), FindMovement(from, to));
}

// Get amount of armies depending on movement
int ArmyMovements::GetArmiesFromMovement(const Region * from, const Region * to) const
{
	ArmyMovementList::const_iterator it = GetMovement(from, to);

	return (it != m_army_movement.end()) ? it->armies : -1;
}

// Get movements from const Region
ArmyMovementList ArmyMovements::GetMovementsFromRegion(const Region * from) const
{
	ArmyMovementList found;
	for (ArmyMovementList::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->from == from)
			found.push_back(*it);
	return found;
}

// Get movements to const Region
ArmyMovementList ArmyMovements::GetMovementsToRegion(const Region * to) const
{
	ArmyMovementList found;
	for (ArmyMovementList::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->to == to)
			found.push_back(*it);
	return found;
}

// Get amount of armies moved from const Region
int ArmyMovements::GetArmiesFromRegion(const Region * from) const
{
	int armies = 0;
	for (ArmyMovementList::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->from == from)
			armies += it->armies;
	return armies;
}

// Get amount of armies moved to const Region
int ArmyMovements::GetArmiesToRegion(const Region * to) const
{
	int armies = 0;
	for (ArmyMovementList::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->to == to)
			armies += it->armies;
	return armies;
}
