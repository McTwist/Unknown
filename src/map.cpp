#include "map.hpp"

#include <queue>

#include "stlastar.h"

#include "game.hpp"

#include "debug.hpp"

Map::Map()
{
}

Map::~Map()
{
	Reset();
}

// Reset map data
void Map::Reset()
{
	for (RegionMap::iterator it = m_regions.begin(); it != m_regions.end(); ++it)
	{
		DestroyRegion(it->second);
	}
	for (SuperRegionMap::iterator it = m_super_regions.begin(); it != m_super_regions.end(); ++it)
	{
		DestroySuperRegion(it->second);
	}
	m_regions.clear();
	m_super_regions.clear();
}

// Clear current state
void Map::ClearState()
{
	// reset all owners as in next round it will be different
	for (RegionMap::iterator it = m_regions.begin(); it != m_regions.end(); ++it)
	{
		it->second->SetOwner(0);
	}
}

// Create a super region
void Map::onSetupMapSuperRegion(int id, int bonus)
{
	SuperRegion * super_region = CreateSuperRegion(id, bonus);
	m_super_regions.insert(std::make_pair(id, super_region));
}

// Create region inside a super region
void Map::onSetupMapRegion(int region, int super_region)
{
	// Get neutral bot
	// Note: Setting neutral is an invalid move
	//Bot * neutral = g_game->GetBot("neutral");
	
	// Create region
	Region * reg = CreateRegion(region);
	// Neutral owns everything at start
	//reg->SetOwner(neutral);
	// Add to list
	m_regions.insert(std::make_pair(region, reg));
	
	// Add to super region
	if (SuperRegion * sup = GetSuperRegion(super_region))
		sup->AddRegion(reg);
}

// Setup map neighbors
void Map::onSetupMapNeighbor(int region, const std::vector<int> & neighbors)
{
	RegionMap::iterator it = m_regions.find(region);
	// Did not find a valid region
	if (it == m_regions.end())
		return;
	
	Region * reg = it->second;
	Region * regb = 0;
	RegionMap::iterator ct;
	
	// Go through list
	for (std::vector<int>::const_iterator bt = neighbors.begin(); bt != neighbors.end(); ++bt)
	{
		ct = m_regions.find(*bt);
		// Did not find a valid region
		if (ct == m_regions.end())
			continue;
		
		regb = ct->second;
		
		// Shake hands
		reg->AddNeighbor(regb);
		regb->AddNeighbor(reg);
	}
}

// Setup wasteland
void Map::onSetupMapWasteland(int region)
{
	RegionMap::iterator it = m_regions.find(region);
	// Did not find a valid region
	if (it == m_regions.end())
		return;

	Region * reg = it->second;

	// Sate as wasteland
	if (SuperRegion * sup = reg->GetSuperRegion())
		sup->AddWasteland(reg);
}

// Regions opponent picked
void Map::onSetupMapOpponentStartingRegions(const std::vector<int> & regions)
{
	GameHistory * history = g_game->GetHistory();
	for (std::vector<int>::const_iterator it = regions.begin(); it != regions.end(); ++it)
	{
		Region * region = GetRegion(*it);
		history->AddRegion(region);
	}
}

// Update map status
void Map::onUpdateMap(int region_id, const std::string & name, int armies)
{
	RegionMap::iterator it = m_regions.find(region_id);
	// Did not find a valid region
	if (it == m_regions.end())
		return;
	
	Region * region = it->second;
	
	// Update armies
	region->SetArmies(armies);
	
	// Change only if valid
	if (Bot * bot = g_game->GetBot(name))
	{
		region->SetOwner(bot);
		// Check wasteland
		if (region->IsWasteland())
			region->GetSuperRegion()->RemoveWasteland(region);
	}

	// Update history for region
	g_game->GetHistory()->AddRegion(region);
}

// Place opponent armies
void Map::onOpponentPlaceArmies(const std::string & /*name*/, int region_id, int amount)
{
	RegionMap::iterator it = m_regions.find(region_id);
	// Did not find a valid region
	if (it == m_regions.end())
		return;
	
	Region * region = it->second;
	
	// Update armies
	region->AddArmies(amount);
}

// Attack/move opponent armies
void Map::onOpponentAttackTransfer(const std::string & /*name*/, int source_region, int target_region, int amount)
{
	RegionMap::iterator it = m_regions.find(source_region);
	// Did not find a valid region
	if (it == m_regions.end())
		return;

	Region * source = it->second;

	it = m_regions.find(target_region);
	// Did not find a valid region
	if (it == m_regions.end())
		return;

	Region * target = it->second;

	// Prepare the movement
	ArmyMovements movements;
	movements.AddMovement(source, target, amount);

	// Add the movement
	g_game->GetHistory()->AddMovements(movements);
}

// Ending round
void Map::onEndRound()
{
	// Cannot be sure about owners anymore
	ClearState();
}

// Get super region
SuperRegion * Map::GetSuperRegion(int id) const
{
	SuperRegionMap::const_iterator it = m_super_regions.find(id);
	if (it == m_super_regions.end())
		return 0;
	return it->second;
}

// Get region
Region * Map::GetRegion(int id) const
{
	RegionMap::const_iterator it = m_regions.find(id);
	if (it == m_regions.end())
		return 0;
	return it->second;
}

// Find best way
// Implemented as A*
std::vector<Region *> Map::FindBestWay(const Region * start, const Region * end) const
{
	typedef AStarSearch<RegionNode> Search;
	Search astarsearch(m_regions.size());
	
	RegionNode node_start(start);
	RegionNode node_end(end);
	
	astarsearch.SetStartAndGoalStates(node_start, node_end);
	
	unsigned int state = 0;
	
	// Do the search
	do
	{
		state = astarsearch.SearchStep();
	}
	while (state == Search::SEARCH_STATE_SEARCHING);
	
	std::vector<Region *> regions;
	
	// Found solution
	if (state == Search::SEARCH_STATE_SUCCEEDED)
	{
		RegionNode * region_node = astarsearch.GetSolutionStart();
		do
		{
			regions.push_back(m_regions.at(region_node->GetRegion()->GetId()));
			
			region_node = astarsearch.GetSolutionNext();
		}
		while (region_node);
		
		astarsearch.FreeSolutionNodes();
	}
	// Failed
	else if (state == Search::SEARCH_STATE_FAILED)
	{
		Debug::Log("Astar error: Failed to find a path\n");
	}
	// Out of memory
	else if (state == Search::SEARCH_STATE_OUT_OF_MEMORY)
	{
		// THIS SHOULD NOT HAPPEN
		// If this happens, double the nodes next time
		Debug::Log("Astar memory error: Out of nodes (%d)\n", m_regions.size());
	}
	
	// Will probably not be used in the final program
	astarsearch.EnsureMemoryFreed();
	
	return regions;
}

// Please implement this as stlastar.h in stlbreadthfirst.h
template<class T>
struct LinkedNode
{
	LinkedNode(T n, LinkedNode<T> * p)
	: node(n), parent(p)
	{}
	T node;
	LinkedNode<T> * parent;
};

typedef LinkedNode<Region *> LinkedRegion;

// Find closest region from a range of regions
// Implemented as breadth first
Regions Map::FindClosestRegion(Region * start, const Regions & regions) const
{
	// Initialization of various variables
	std::vector<LinkedRegion *> nodes;
	std::queue<LinkedRegion *> queue;
	std::set<Region *> explored;
	std::set<Region *> regions_set(regions.begin(), regions.end());
	LinkedRegion * final_node = 0;
	LinkedRegion * parent_node = 0;
	LinkedRegion * temp = 0;
	Region * region = 0;
	
	// Add first one
	temp = new LinkedRegion(start, 0);
	nodes.push_back(temp);
	queue.push(temp);
	explored.insert(start);
	
	// Continue as long as there is more
	while (!queue.empty())
	{
		// Get next node
		parent_node = queue.front();
		queue.pop();
		region = parent_node->node;
		
		// Check if finished
		if (regions_set.find(region) != regions_set.end())
		{
			final_node = parent_node;
			break;
		}
		
		// Append all neighbors
		const Regions & neighbors = region->GetNeighbors();
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
		{
			region = *it;
			// Check if already traversed
			if (explored.find(region) == explored.end())
			{
				temp = new LinkedRegion(region, parent_node);
				nodes.push_back(temp);
				queue.push(temp);
				explored.insert(region);
			}
		}
	}
	
	// Final path
	Regions path;
	
	// Iterate through final result
	while (final_node)
	{
		path.push_back(final_node->node);
		final_node = final_node->parent;
	}
	// Correct order
	std::reverse(path.begin(), path.end());
	
	// Deallocate
	for (std::vector<LinkedRegion *>::iterator it = nodes.begin(); it != nodes.end(); ++it)
		delete *it;
	return path;
}

// Allocation
SuperRegion * Map::CreateSuperRegion(int id, int bonus)
{
	return new SuperRegion(id, bonus);
}

void Map::DestroySuperRegion(SuperRegion * super_region)
{
	delete super_region;
}

Region * Map::CreateRegion(int id)
{
	return new Region(id);
}

void Map::DestroyRegion(Region * region)
{
	delete region;
}
