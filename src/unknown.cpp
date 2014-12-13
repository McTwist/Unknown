#include "unknown.hpp"

// Required for connection to the map and check bot activity
#include "game.hpp"

// Timing and logging
#include "timer.hpp"
#include "debug.hpp"

#include "stl_if.hpp"
#include "stl_sort.hpp"

Unknown::Unknown()
: m_armies(0)
{
}

// Pick starting regions
void Unknown::onPickStartingRegions(float time, const std::vector<int> & regions)
{
	Timer timer;
	timer.Start();
	// Possible strategies:
	// * Less neighbors (Chokepoint)
	// * Smaller super region (Bonus army)
	// * Close regions (Better mobility)
	// * Bigger super regions (Spread fast)
	std::vector<int> ret(regions.begin(), regions.begin() + 6);
	SetStartingRegions(ret);
	Send();
	
	float dt = timer.MilliSeconds();
	// Time it
	if (dt > time * 0.5f)
		Debug::Log("Timing(pick_staring_regions): %.3f\n", dt);
}

// Pick starting region
void Unknown::onPickStartingRegion(float time, const std::vector<int> & regions)
{
	Timer timer;
	timer.Start();
	// Possible strategies:
	// * Less neighbors (Chokepoint)
	// * Smaller super region (Bonus army)
	// * Close regions (Better mobility)
	// * Bigger super regions (Spread fast)
	PickStartingRegion(regions.front());
	Send();
	
	float dt = timer.MilliSeconds();
	// Time it
	if (dt > time * 0.5f)
		Debug::Log("Timing(pick_staring_region): %.3f\n", dt);
}

// Get starting armies
void Unknown::onSettingsStartingArmies(int amount)
{
	m_armies = amount;
}

// Place armies
void Unknown::onGoPlaceArmies(float time)
{
	Timer timer;
	timer.Start();
	// Get neutral area bot
	//Bot * neutral = g_game->GetNeutral();
	
	Region * region = 0;
	
	// Current way to effectively place troops on each region depending on need
	// Always uses 60% of the current amount left, making it possible to use a
	// lot on important regions
	// Later version might implement priorities, army hogging, bailing, etc...
	float effective_troop_placer = 0.7f;
	
	// Possible strategies:
	// * Percentage of captured super region, capture all of it.
	// * Defend as long as enemies haven't increased too much.
	// * Bail small colonies if enemy is too strong, but only if having at least two more colonies (Might scrap this one as it is risky).
	// * Spread armies to the front, avoid balanced armies as it is more success with more at once.
	// * Attack when possibility of not loosing too many armies (Either by sending fewer armies or sending everything when high probability of success).
	// * Avoid a deadlock where both bots tries to overpower each other with armies. Could be fixed by after a couple of rounds doing the same thing, try to spread to other regions without loosing army power on the front. Benefit by taking over super regions.
	
	// Get Neighbors
	const Regions & neighbors = GetNeighbors();
	Regions hostile = GetHostileRegions(neighbors);
	// Get affected regions
	Regions affected = GetAffectedRegions();
	
	// Handle hostiles as they are active
	if (!hostile.empty())
	{
		// Place armies next to hostile and try to match them up or above
		// Keep in mind of multiple neighbors
		// Attack could be (0.6 * attack) / (0.7 * defense) > 1.0, but to to prevent deadlock,
		// consult lower value each turn to a limit of 70% chance of success.
		// Prevent deadlock by trying to spread to other regions
		
		// Copy to prevent modificatio of source
		Regions aff(affected);
		// Sort depending on army differences
		std::sort(aff.begin(), aff.end(), compare_region_neighbor_army_diff);
		for (Regions::iterator it = aff.begin(); it != aff.end(); ++it)
		{
			region = *it;
			// Get effective hostile neighbors
			Regions host = GetHostileRegions(region->GetNeighbors());
			// Sort for better measurements
			std::sort(host.begin(), host.end(), compare_region_army);
			// Get effective regions
			Regions effective = GetEffectiveRegions(host);
			int count = effective.size();
			// Nothing to be afraid of
			if (count == 0)
				continue;
			// Calculate armies
			int armies = Region::GetRegionsArmies(effective);
			int effective_armies = armies - count;
			
			// I should be stronger
			if (Region::CalculateAttackProbability(region, effective.front()) < 1.0f)
			{
				// Calculate troops
				int troops = m_armies * effective_troop_placer;
				// Check for errors
				if (troops > m_armies)
					troops = m_armies;
				if (troops <= 0)
					continue;
				// Place troops
				PlaceArmy(region, troops);
			}
			// They are stronger in number
			if (effective_armies > region->GetArmies() - 1)
			{
				// Calculate troops
				int troops = m_armies * effective_troop_placer;
				// Check for errors
				if (troops > m_armies)
					troops = m_armies;
				if (troops <= 0)
					continue;
				// Place troops
				PlaceArmy(region, troops);
			}
		}
	}
	// Roam freely
	if (m_armies > 0)
	{
		// Get all current super regions
		std::set<SuperRegion *> check_regions;
		std::set<SuperRegion *> check_neighbors;
		for (Regions::iterator it = m_regions.begin(); it != m_regions.end(); ++it)
			check_regions.insert((*it)->GetSuperRegion());
		for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
			check_neighbors.insert((*it)->GetSuperRegion());
		
		// Nameconvention: Derp?
		std::set<SuperRegion *> 
			// Super regions that you are on but do not own completely
			super_regions_that_you_are_on_but_do_not_own_completely, 
			// Super regions that you are neighbor with but are not on
			super_regions_that_you_are_neighbor_with_but_are_not_on;
		// Get super regions already on that there is neighbors on (Don't own that super region)
		std::set_intersection(
			check_regions.begin(), check_regions.end(),
			check_neighbors.begin(), check_neighbors.end(),
			inserter(super_regions_that_you_are_on_but_do_not_own_completely, super_regions_that_you_are_on_but_do_not_own_completely.begin()));
		
		// Regions to place on
		Regions place;
		
		// Got whole super region(s)
		// There is no neighbors on your super region
		if (super_regions_that_you_are_on_but_do_not_own_completely.empty())
		{
			// Get super regions that are next to the ones you are on
			std::set_difference(
				check_neighbors.begin(), check_neighbors.end(),
				check_regions.begin(), check_regions.end(),
				inserter(super_regions_that_you_are_neighbor_with_but_are_not_on, super_regions_that_you_are_neighbor_with_but_are_not_on.begin()));
			
			// Somehow you just destroyed the world...
			if (super_regions_that_you_are_neighbor_with_but_are_not_on.empty())
			{
				// Note: If you ever get here, please validate the engine and try to fix it
				Debug::Log("Error: You just took over the world!\n");
			}
			// Go through the super regions to locate best one to attack
			else
			{
				// Locate regions on the super regions
				for (Regions::iterator it = affected.begin(); it != affected.end(); ++it)
				{
					region = *it;
					// If on a region next to your region
					const Regions & neigh = region->GetNeighbors();
					for (Regions::const_iterator nt = neigh.begin(); nt != neigh.end(); ++nt)
					{
						region = *nt;
						// Is on the super region
						if (super_regions_that_you_are_neighbor_with_but_are_not_on.find(region->GetSuperRegion()) != super_regions_that_you_are_neighbor_with_but_are_not_on.end())
							place.push_back(*it);
					}
				}
			}
		}
		// Continue on the current super region(s) to take them over completely
		else
		{
			// Locate regions on the super regions
			for (Regions::iterator it = affected.begin(); it != affected.end(); ++it)
			{
				region = *it;
				// Check if on the super region
				if (super_regions_that_you_are_on_but_do_not_own_completely.find(region->GetSuperRegion()) != super_regions_that_you_are_on_but_do_not_own_completely.end())
				{
					// Locate neighbors
					const Regions & neigh = region->GetNeighbors();
					Regions neut = GetNeutralRegions(neigh);
					for (Regions::const_iterator nt = neut.begin(); nt != neut.end(); ++nt)
					{
						// Check if neighbor is inside the current super region
						if ((*nt)->GetSuperRegion() == region->GetSuperRegion())
						{
							// There are some neighbors in the area
							place.push_back(region);
							break;
						}
					}
				}
			}
		}
		
		// Sort in the way of priority
		std::sort(place.begin(), place.end(), compare_region_super_region_priority);
		
		// Go through the list, placing the last armies
		#if 0
		Regions::iterator it = place.begin();
		for (int i = m_armies; i > 0; --i)
		{
			region = *it;
			// Add one to each region
			PlaceArmy(region, 1);
			++it;
			// Go around
			if (it == place.end())
				it = place.begin();
		}
		#else
		// Place instead everything on one
		if (!place.empty())
			PlaceArmy(place.front(), m_armies);
		#endif
	}
	
	SendPlaceArmies();
	
	float dt = timer.MilliSeconds();
	// Time it
	if (dt > time * 0.5f)
		Debug::Log("Timing(place_armies): %.3f\n", dt);
}

// Attack or transfer armies
void Unknown::onGoAttackTransfer(float time)
{
	Timer timer;
	timer.Start();
	// Get neutral area bot
	//Bot * neutral = g_game->GetNeutral();
	
	Region * region = 0;
	
	// Get Neighbors
	const Regions & neighbors = GetNeighbors();
	Regions hostile = GetHostileRegions(neighbors);
	// Extra regions
	Regions inner = GetUnaffectedRegions();
	Regions effective = GetEffectiveRegions(inner);
	Regions affected = GetAffectedRegions();
	Regions affective = GetEffectiveRegions(affected);
	
	// TODO: Create valid possible moves
	// * Hold ground
	// * Attack when there is benefits
	// * Try to flank
	
	// Use:
	//Region::CalculateAttackProbability
	
	/*
	 * Decide to attack or not
	 * Make sure it is profitable enough (Try not to loose more than they do)
	 */
	{
		std::set<Region *> stand_ground;
		// Attack hostiles
		for (Regions::iterator it = affective.begin(); it != affective.end(); ++it)
		{
			region = *it;
			const Regions & neigh = region->GetNeighbors();
			Regions host = GetHostileRegions(neigh);
			int count = host.size();
			// Somehow found none (Should not be possible due to affected region)
			if (count == 0)
				continue;
			int armies = Region::GetRegionsArmies(host);
			
			std::sort(host.begin(), host.end(), compare_region_army);
			
			// Note: Attack probability could change depending on several interactions
			
			// Chance of they attacking, do not attack
			// Note: Might change depending on bot
			if (Region::CalculateAttackProbability(armies - count, region->GetArmies()) < 1.0f)
			{
				// Attack if enough power
				if (Region::CalculateAttackProbability(region, host.front()) > 1.0f)
				{
					// Note: How many armies moved should be changed later on
					MoveArmy(region, host.front(), region->GetArmies()-1);
				}
				else
				{
					stand_ground.insert(region);
				}
			}
			else
			{
				stand_ground.insert(region);
			}
		}
		// Attack neutrals
		for (Regions::iterator it = affective.begin(); it != affective.end(); ++it)
		{
			region = *it;
			// Should not move
			if (stand_ground.find(region) != stand_ground.end())
				continue;
			const Regions & neigh = region->GetNeighbors();
			Regions neutrals = GetNeutralRegions(neigh);
			int count = neutrals.size();
			// Somehow found none (Should not be possible due to affected region)
			if (count == 0)
				continue;
			
			Region * optimal = neutrals.front();
			float opt_max = 0;
			
			// Keep priority within super region
			for (Regions::iterator nt = neutrals.begin(); nt != neutrals.end(); ++nt)
			{
				Region * r = *nt;
				// Get my bot ownership of super region count
				int count = r->GetSuperRegion()->GetBotRegionCount(this);
				// Get priority of super region
				float prio = r->GetSuperRegion()->GetPriority(count);
				// Update optimal pick
				if (prio > opt_max)
				{
					opt_max = prio;
					optimal = r;
				}
			}
			//std::sort(neutrals.begin(), neutrals.end(), compare_region_super_region_priority);
			
			// TODO: Implement a split system
			/*for (Regions::iterator nt = neutrals.begin(); nt != neutrals.end(); ++nt)
			{
				Region * r = *nt;
				Regions reg = Bot::GetConnectedSuperRegion(r);
				Debug::Log("PRIORITY: %d, %f(%d)\n", 
					r->GetId(), 
					r->GetSuperRegion()->GetPriority(reg.size()), 
					reg.size());
			}*/
			
			// Attack if enough power
			if (Region::CalculateAttackProbability(region, optimal) > 1.0f)
			{
				// Note: How many armies moved should be changed later on
				MoveArmy(region, optimal, region->GetArmies()-1);
			}
		}
	}
	
	/*
	 * Spread to other areas, taking over super regions
	 * Could only happen if enough armies to do so
	 * Some super regions have a higher priority
	 */
	// TODO: Look into this
	
	/*
	 * Move all inner armies to the borders
	 * This makes sure there is no armies that are left behind
	 * Even if this is called right after the attacking, it might be called first
	 */
	// Go through unaffected effective regions
	for (Regions::iterator it = effective.begin(); it != effective.end(); ++it)
	{
		// Note: Currently it gets the closest regions that are needed to continue,
		// but this could be changed in the future to enforce a behavior to move
		// to best position depending on enemy
		// An another strategy to add is to make sure armies are also split up more
		// effectively towards multiple goals
		// Keep in mine that splitting up an army is mostly a bad idea due to the
		// randomness in this engine
		region = *it;
		// Get connected regions
		Regions landmass = GetConnectedRegions(region);
		// Get regions that have trouble
		Regions use = Region::GetDifferenceUnsorted(landmass, inner);
		// Find closest regions for this landmass
		// Note: Should return multiple paths to weight down moving only to lower count region(Or keep it as it is, a neat "feature")
		std::vector<Region *> path = g_game->GetMap()->FindClosestRegion(
			region, std::set<Region *>(use.begin(), use.end()));
			
		// No path found (Should not happen, but is handled anyway)
		if (path.empty())
		{
			Debug::Log("Warn: Found no internal path\n");
			continue;
		}
		// Moved itself (Should also not happen, but is handled anyway)
		else if (path.size() == 1)
		{
			Debug::Log("Warn: Pointing to itself\n");
			continue;
		}
		// Located a path
		else
		{
			// Only take The next one in the list
			Region * next = path.at(1);
			// Move it
			MoveArmy(region, next, region->GetArmies()-1);
		}
	}
	
	SendAttackTransfer();
	
	float dt = timer.MilliSeconds();
	// Time it
	if (dt > time * 0.5f)
		Debug::Log("Timing(attack/transfer): %.3f\n", dt);
}

// Starting the round
void Unknown::onStartRound(int round)
{
	// Changing strategy depending on how long it has gone
}

// Place armies on a region
// Smart placement
// Note: More checks could be made
void Unknown::PlaceArmy(Region * region, int amount)
{
	// Add to other placements
	m_placement[region->GetId()] += amount;
	// Add armies on this one
	region->AddArmies(amount);
	// Remove armies on hand
	m_armies -= amount;
}

// Move army from one place to an another
// Smart movement
// Note: More checks could be made
void Unknown::MoveArmy(Region * source, Region * target, int amount)
{
	// Add to other movements
	m_moves[std::make_pair(source->GetId(), target->GetId())] += amount;
	// Move temporary armies here
	source->MoveArmies(amount);
	// Add into attacked region
	m_to_regions[target->GetId()] += amount;
}

// Gets how much is attacked on a single region
int Unknown::GetAttackRegion(const Region * region) const
{
	AttackRegionList::const_iterator it = m_to_regions.find(region->GetId());
	if (it == m_to_regions.end())
		return 0;
	return it->second;
}

// Sends placement results to engine
void Unknown::SendPlaceArmies()
{
	// Nothing decided, do nothing
	if (m_placement.empty())
		NoMoves();
	// Place everything
	else
	{
		for (PlacementList::iterator it = m_placement.begin(); it != m_placement.end(); ++it)
			PlaceArmies(GetName(), it->first, it->second);
	}
	
	// And flush
	Send();
	
	// Clear it
	m_placement.clear();
}

// Sends movement results to engine
void Unknown::SendAttackTransfer()
{
	// Nothing decided, do nothing
	if (m_moves.empty())
		NoMoves();
	// Move everything
	else
	{
		for (MovementList::iterator it = m_moves.begin(); it != m_moves.end(); ++it)
			AttackTransfer(GetName(), it->first.first, it->first.second, it->second);
	}
	
	// And flush
	Send();
	
	// Clear it
	m_moves.clear();
	m_to_regions.clear();
}
