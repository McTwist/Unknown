#pragma once
#ifndef ARMY_MOVEMENT_HPP
#define ARMY_MOVEMENT_HPP

#include <map>
#include <vector>

class Region;

// Movement structure
struct Movement
{
	const Region * from;
	const Region * to;
	int armies;
};

// Current movements
typedef std::vector<Movement> Movements;

class ArmyMovement
{
public:

	// Reset movements
	void Reset();

	// Add movement
	void AddMovement(const Region * from, const Region * to, int armies);
	void AddMovement(const Movement & movement);
	void AddMovements(const Movements & movements);

	// Get list of movement
	const Movements & GetMovements() const;
	
	int GetArmiesFromMovement(const Region * from, const Region * to) const;
	Movements GetMovementsFromRegion(const Region * from) const;
	Movements GetMovementsToRegion(const Region * to) const;
	int GetArmiesFromRegion(const Region * from) const;
	int GetArmiesToRegion(const Region * to) const;

private:

	Movements::iterator GetMovement(const Region * from, const Region * to);
	Movements::const_iterator GetMovement(const Region * from, const Region * to) const;

	Movements m_army_movement;

};

#endif // ARMY_MOVEMENT_HPP