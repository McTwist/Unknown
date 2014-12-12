#include "bot.hpp"

#include <queue>

#include "game.hpp"
#include "region.hpp"
#include "debug.hpp"

#include "stl_if.hpp"

Bot::Bot()
: m_visible_armies(0)
, m_estimated_armies(0)
{
}

// Set bot name
void Bot::SetName(const std::string & name)
{
	m_name = name;
}

// Get bot name
const std::string & Bot::GetName() const
{
	return m_name;
}

// Get visible army count
int Bot::GetVisibleArmy() const
{
	return m_visible_armies;
}

// Get the estimated value of how many armies the bot have
int Bot::GetEstimatedArmy() const
{
	return m_estimated_armies; // Make some calculation of how many armies the bot should have
}

// Get current total army attack power
int Bot::GetTotalArmyAttackPower() const
{
	return m_visible_armies - m_regions.size();
}

// Get neigbors
Regions Bot::GetNeighbors() const
{
	Regions regions;
	std::set<Region *> check_list;
	Region * region;
	// Two ways of adding to list
#if 0
	// Go through owning regions
	for (Regions::const_iterator it = m_regions.begin(); it != m_regions.end(); ++it)
	{
		region = *it;
		// Go through neighbors
		const Regions & neighbors = region->GetNeighbors();
		for (Regions::const_iterator at = neighbors.begin(); at != neighbors.end(); ++at)
		{
			region = *at;
			// Check duplicates and ownership
			if (region->GetOwner() != this && check_list.find(region) == check_list.end())
			{
				regions.push_back(region);
				check_list.insert(region);
			}
		}
	}
#else
	// Go through owning regions
	for (Regions::const_iterator it = m_regions.begin(); it != m_regions.end(); ++it)
	{
		region = *it;
		// Go through neighbors
		const Regions & neighbors = region->GetNeighbors();
		for (Regions::const_iterator at = neighbors.begin(); at != neighbors.end(); ++at)
		{
			region = *at;
			if (region->GetOwner() != this)
				check_list.insert(region);
		}
	}
	regions.assign(check_list.begin(), check_list.end());
#endif
	return regions;
}

// Get regions that have hostile region close to it
Regions Bot::GetAffectedRegions() const
{
	Regions regions = m_regions;
	// Remove all regions with no enemy neighbor
	regions.erase(
		std::remove_if(
			regions.begin(), regions.end(), 
			if_region_no_enemy_neighbor),
		regions.end());
	return regions;
}

// Get regions that have no hostile region close to it
Regions Bot::GetUnaffectedRegions() const
{
	Regions regions = m_regions;
	// Remove all regions with an enemy neighbor
	regions.erase(
		std::remove_if(
			regions.begin(), regions.end(), 
			if_region_enemy_neighbor),
		regions.end());
	return regions;
}

// Get effective regions that have more than one(1) army, 
// hence giving ability to attack
Regions Bot::GetEffectiveRegions(const Regions & region_list) const
{
	// Duplicate
	Regions regions;
	Region * region;
	for (Regions::const_iterator it = region_list.begin(); it != region_list.end(); ++it)
	{
		region = *it;
		// Require at least one army
		if (region->GetArmies() > 1)
			regions.push_back(region);
	}
	return regions;
}

// Get regions that are hostile and could attack back
// This is only regions that are taken over by a bot
Regions Bot::GetHostileRegions(const Regions & region_list) const
{
	Bot * neutral = g_game->GetNeutral();
	// Duplicate
	Regions regions;
	Region * region;
	for (Regions::const_iterator it = region_list.begin(); it != region_list.end(); ++it)
	{
		region = *it;
		// Pick only other bots
		if (region->GetOwner() != this && region->GetOwner() != neutral)
			regions.push_back(region);
	}
	return regions;
}

// Get regions that are neutral
Regions Bot::GetNeutralRegions(const Regions & region_list) const
{
	Bot * neutral = g_game->GetNeutral();
	// Duplicate
	Regions regions;
	Region * region;
	for (Regions::const_iterator it = region_list.begin(); it != region_list.end(); ++it)
	{
		region = *it;
		// Pick only other bots
		if (region->GetOwner() == neutral)
			regions.push_back(region);
	}
	return regions;
}

// Get regions connected into one whole landmass
// Could be slow if there's a lot of regions involved
Regions Bot::GetConnectedRegions(const Region * start)
{
	Region * region = const_cast<Region *>(start);
	// Some variables needed
	Bot * bot = region->GetOwner();
	std::set<Region *> explored;
	std::queue<Region *> queue;
	
	// Insert first node
	queue.push(region);
	explored.insert(region);
	
	Regions regions;
	
	// Go through queue
	while (!queue.empty())
	{
		region = queue.front();
		queue.pop();
		
		// Add to list of owned
		regions.push_back(region);
		
		// Go through neighbors
		const Regions & neighbors = region->GetNeighbors();
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
		{
			region = *it;
			// Check owner and explored
			if (region->GetOwner() == bot && explored.find(region) == explored.end())
			{
				queue.push(region);
				explored.insert(region);
			}
		}
	}
	
	return regions;
}

// Get regions connected into one whole landmass in a super region
// Could be slow if there's a lot of regions involved
Regions Bot::GetConnectedSuperRegion(const Region * start)
{
	Region * region = const_cast<Region *>(start);
	// Some variables needed
	Bot * bot = region->GetOwner();
	SuperRegion * super_region = region->GetSuperRegion();
	std::set<Region *> explored;
	std::queue<Region *> queue;
	
	// Insert first node
	queue.push(region);
	explored.insert(region);
	
	Regions regions;
	
	// Go through queue
	while (!queue.empty())
	{
		region = queue.front();
		queue.pop();
		
		// Add to list of owned
		regions.push_back(region);
		
		// Go through neighbors
		const Regions & neighbors = region->GetNeighbors();
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
		{
			region = *it;
			// Check owner and explored
			if (region->GetOwner() == bot && 
				region->GetSuperRegion() == super_region && 
				explored.find(region) == explored.end())
			{
				queue.push(region);
				explored.insert(region);
			}
		}
	}
	
	return regions;
}

// Clear current state for regions
void Bot::ClearState()
{
	m_regions.clear();
}

// Update owned regions
void Bot::onUpdateMap(int region, const std::string & name, int armies)
{
	// Check correct bot
	if (m_name != name)
		return;
	
	// Get region
	Region * reg = g_game->GetMap()->GetRegion(region);
	if (!reg)
		return;
	
	// Calculate visible armies
	m_visible_armies += armies;
	
	// Add to list
	m_regions.push_back(reg);
}

// Ending the round
void Bot::onEndRound()
{
	// Clear as they are probably invalid next round
	m_regions.clear();
	// No armies
	m_visible_armies = 0;
}
