#pragma once
#ifndef STL_REMOVE_IF_HPP
#define STL_REMOVE_IF_HPP

// Compare functions used for stl methods

// Removing functions
// Remove all that have no enemy neighbor
inline bool if_region_no_enemy_neighbor(const Region * value)
{
	const Regions & neighbors = value->GetNeighbors();
	for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
		// Not owning
		if (value->GetOwner() != (*it)->GetOwner())
			return false;
	return true;
}
// Remove all that have at least one enemy neighbor
inline bool if_region_enemy_neighbor(const Region * value)
{
	const Regions & neighbors = value->GetNeighbors();
	for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
		// Not owning
		if (value->GetOwner() != (*it)->GetOwner())
			return true;
	return false;
}

#endif // STL_REMOVE_IF_HPP