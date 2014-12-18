#pragma once
#ifndef STL_SORT_HPP
#define STL_SORT_HPP

// Compare functions used for stl methods

// Sorting functions
// Sort depending on amount of armies
inline bool compare_region_army(const Region * first, const Region * second)
{
	return first->GetArmies() > second->GetArmies();
}
// Sort depending on neighbor armies
inline bool compare_region_neighbor_army(const Region * first, const Region * second)
{
	int first_army = 0, second_army = 0;
	// Calculate first
	{
		const Regions & neighbors = first->GetNeighbors();
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
			if (first->GetOwner() != (*it)->GetOwner())
				first_army += (*it)->GetArmies()-1;
	}
	// Calculate second
	{
		const Regions & neighbors = second->GetNeighbors();
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
			if (first->GetOwner() != (*it)->GetOwner())
				second_army += (*it)->GetArmies()-1;
	}
	return first_army > second_army;
}
// Sort depending on neighbor effective armies
inline bool compare_region_neighbor_army_diff(const Region * first, const Region * second)
{
	int first_army = 0, second_army = 0,
		first_a = first->GetArmies(), second_a = second->GetArmies();
	// Calculate first
	{
		const Regions & neighbors = first->GetNeighbors();
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
			if (first->GetOwner() != (*it)->GetOwner())
				first_army += (*it)->GetArmies()-1;
	}
	// Calculate second
	{
		const Regions & neighbors = second->GetNeighbors();
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
			if (first->GetOwner() != (*it)->GetOwner())
				second_army += (*it)->GetArmies()-1;
	}
	// Avoid crash
	if (first_a < 1)
		first_a = 1;
	if (second_a < 1)
		second_a = 1;
	// Do the calculations
	return 
		((float)first_army / (float)first_a) >
		((float)second_army / (float)second_a);
}
// Sort depending on super region priority
inline bool compare_region_super_region_priority(const Region * first, const Region * second)
{
	// Get parent
	SuperRegion * first_super = first->GetSuperRegion();
	SuperRegion * second_super = second->GetSuperRegion();
	
	// Get connected regions
	Regions first_regions = Bot::GetConnectedSuperRegion(first);
	Regions second_regions = Bot::GetConnectedSuperRegion(second);
	
	// Calculate priority
	float first_prio = first_super->GetPriority(first_regions.size());
	float second_prio = second_super->GetPriority(second_regions.size());
	
	// Sort
	return first_prio > second_prio;
}
// Sort depending on super region and bot priority
struct CompareBotSuperRegionPriority
{
	CompareBotSuperRegionPriority(Bot * b)
	: bot(b)
	{}
	
	Bot * bot;
	
	inline bool operator()(const Region * first, const Region * second)
	{
		// Get parent
		SuperRegion * first_super = first->GetSuperRegion();
		SuperRegion * second_super = second->GetSuperRegion();
		
		// Count connected region
		int first_count = first_super->GetBotRegionCount(bot);
		int second_count = second_super->GetBotRegionCount(bot);
		
		// Calculate priority
		float first_prio = first_super->GetPriority(first_count);
		float second_prio = second_super->GetPriority(second_count);
		
		// Sort
		return first_prio > second_prio;
	}
};

#endif // STL_SORT_HPP