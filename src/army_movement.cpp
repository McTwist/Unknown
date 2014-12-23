#include "army_movement.hpp"

#include "region.hpp"

// Find depending on super const Region and bot priority
struct FindMovement
{
	FindMovement(const Region * f, const Region * t)
	: from(f), to(t)
	{}
	
	const Region * from;
	const Region * to;
	
	inline bool operator()(const Movement & movement)
	{
		return movement.from == from && movement.to == to;
	}
};

// Reset the movements
void ArmyMovement::Reset()
{
	m_army_movement.clear();
}

// Add new movement
void ArmyMovement::AddMovement(const Region * from, const Region * to, int armies)
{
	Movements::iterator it = GetMovement(from, to);
	// Append on old one
	if (it != m_army_movement.end())
	{
		it->armies += armies;
	}
	// Add new
	else
	{
		Movement movement;
		movement.from = from;
		movement.to = to;
		movement.armies = armies;
		m_army_movement.push_back(movement);
	}
}

// Add new movement
void ArmyMovement::AddMovement(const Movement & movement)
{
	AddMovement(movement.from, movement.to, movement.armies);
}

// Add a list of movements
void ArmyMovement::AddMovements(const Movements & movements)
{
	m_army_movement.insert(m_army_movement.end(), movements.begin(), movements.end());
}

// Get current list of movements
const Movements & ArmyMovement::GetMovements() const
{
	return m_army_movement;
}

// Get movement that exist already
Movements::iterator ArmyMovement::GetMovement(const Region * from, const Region * to)
{
	return std::find_if(m_army_movement.begin(), m_army_movement.end(), FindMovement(from, to));
}
Movements::const_iterator ArmyMovement::GetMovement(const Region * from, const Region * to) const
{
	return std::find_if(m_army_movement.begin(), m_army_movement.end(), FindMovement(from, to));
}

// Get amount of armies depending on movement
int ArmyMovement::GetArmiesFromMovement(const Region * from, const Region * to) const
{
	Movements::const_iterator it = GetMovement(from, to);

	return (it != m_army_movement.end()) ? it->armies : -1;
}

// Get movements from const Region
Movements ArmyMovement::GetMovementsFromRegion(const Region * from) const
{
	Movements found;
	for (Movements::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->from == from)
			found.push_back(*it);
	return found;
}

// Get movements to const Region
Movements ArmyMovement::GetMovementsToRegion(const Region * to) const
{
	Movements found;
	for (Movements::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->to == to)
			found.push_back(*it);
	return found;
}

// Get amount of armies moved from const Region
int ArmyMovement::GetArmiesFromRegion(const Region * from) const
{
	int armies = 0;
	for (Movements::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->from == from)
			armies += it->armies;
	return armies;
}

// Get amount of armies moved to const Region
int ArmyMovement::GetArmiesToRegion(const Region * to) const
{
	int armies = 0;
	for (Movements::const_iterator it = m_army_movement.begin(); it != m_army_movement.end(); ++it)
		if (it->to == to)
			armies += it->armies;
	return armies;
}
