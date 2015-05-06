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
	// Copied from below
	std::vector<int> ret;
	{
		Regions sorted;
		sorted.reserve(regions.size());
		for (std::vector<int>::const_iterator it = regions.begin(); it != regions.end(); ++it)
		{
			sorted.push_back(g_game->GetMap()->GetRegion(*it));
		}
		std::sort(sorted.begin(), sorted.end(), CompareBotSuperRegionPriority(this));
		for (unsigned int i = 0; i < 6 && i < sorted.size(); ++i)
		{
			ret.push_back(sorted[i]->GetId());
		}
	}
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
	// * Less neighbors (Choke point)
	// * Smaller super region (Bonus army)
	// * Close regions (Better mobility)
	// * Bigger super regions (Spread fast)
	// First version of making something somewhat when picking regions
	int picked_region = regions.front();
	{
		Regions sorted;
		sorted.reserve(regions.size());
		for (std::vector<int>::const_iterator it = regions.begin(); it != regions.end(); ++it)
		{
			sorted.push_back(g_game->GetMap()->GetRegion(*it));
		}
		std::sort(sorted.begin(), sorted.end(), CompareBotSuperRegionPriority(this));
		picked_region = sorted.front()->GetId();
	}

	PickStartingRegion(picked_region);
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
		// Sort depending on super region priority
		std::sort(aff.begin(), aff.end(), compare_region_super_region_priority);
		// Sort depending on army differences
		std::stable_sort(aff.begin(), aff.end(), compare_region_neighbor_army_diff);
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
				int troops = int(m_armies * effective_troop_placer);
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
				int troops = int(m_armies * effective_troop_placer);
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
		
		std::set<SuperRegion *> 
			// Super regions that you are on but do not own completely
			contested_regions, 
			// Super regions that you are neighbor with but are not on
			desired_neighbors;
		// Get super regions already on that there is neighbors on (Don't own that super region)
		std::set_intersection(
			check_regions.begin(), check_regions.end(),
			check_neighbors.begin(), check_neighbors.end(),
			inserter(contested_regions, contested_regions.begin()));
		
		// Regions to place on
		Regions place;
		
		// Got whole super region(s)
		// There is no neighbors on your super region
		if (contested_regions.empty())
		{
			// Get super regions that are next to the ones you are on
			std::set_difference(
				check_neighbors.begin(), check_neighbors.end(),
				check_regions.begin(), check_regions.end(),
				inserter(desired_neighbors, desired_neighbors.begin()));
			
			// Somehow you just destroyed the world...
			if (desired_neighbors.empty())
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
						if (desired_neighbors.find(region->GetSuperRegion()) != desired_neighbors.end())
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
				if (contested_regions.find(region->GetSuperRegion()) != contested_regions.end())
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
		while (m_armies > 0)
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
	
	// Just to ensure everything is placed
	if (m_armies > 0)
	{
		// Get the front
		if (!affected.empty())
		{
			Debug::Log("Had to place %d armies on %d.\n", m_armies, affected.front()->GetId());
			PlaceArmy(affected.front(), m_armies);
		}
		// Place randomly
		else if (!m_regions.empty())
		{
			Debug::Log("Warn: Had to place %d armies on %d.\n", m_armies, m_regions.front()->GetId());
			PlaceArmy(m_regions.front(), m_armies);
		}
		// Why did this happen?
		else
		{
			Debug::Log("Error: Unable to place %d armies.\n", m_armies);
		}
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

	// Strategy variables
	// Chance for region to attack neutral regions
	float neutral_army_attack = 1.5f;
	
	Region * region = 0;
	
	// Get Neighbors
	//const Regions & neighbors = GetNeighbors();
	//Regions hostile = GetHostileRegions(neighbors);
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

			// The region that will be attacked
			Region * attack_region = host.front();
			
			// Note: Attack probability could change depending on several interactions
			
			// Chance of they attacking, do not attack
			// Note: Might change depending on bot
			if (Region::CalculateAttackProbability(armies - count, region->GetArmies()) < 1.0f)
			{
				// Attack if enough power
				if (Region::CalculateAttackProbability(region, attack_region) > 1.0f)
				{
					// Note: How many armies moved should be changed later on
					MoveArmy(region, attack_region, region->GetArmies() - 1);
				}
				else
				{
					// Get assistance
					Regions assistance = GetRegions(attack_region->GetNeighbors());
					int movement_armies =  Region::GetRegionsArmies(assistance) + m_movements.GetArmiesToRegion(attack_region);
					// Attack if enough collaborated manpower
					if (Region::CalculateAttackProbability(movement_armies - assistance.size(), attack_region->GetArmies()) > 2.0f)
					{
						MoveArmy(region, attack_region, region->GetArmies() - 1);
					}
					else
					{
						stand_ground.insert(region);
					}
				}
			}
			else
			{
				stand_ground.insert(region);
			}
		}

		// Prepare sorting algorithm
		CompareBotSuperRegionPriority compare_regions_bot_priority(this);
		
		Region * attack_region = 0;

		// Attack neutrals
		for (Regions::iterator it = affective.begin(); it != affective.end(); ++it)
		{
			region = *it;
			const Regions & neigh = region->GetNeighbors();
			Regions neutrals = GetNeutralRegions(neigh);
			int count = neutrals.size();
			// Somehow found none (Should not be possible due to affected region)
			if (count == 0)
				continue;
			
			// Should probably not move
			if (stand_ground.find(region) != stand_ground.end())
			{
				// If in a pinch, but still enough armies, attack neutrals close by
				// Find out hostiles
				Regions host = GetHostileRegions(neigh);
				// Calculate their strength
				int defense_armies = Region::GetRegionsArmies(host) - host.size();
				
				// Sort for further use
				//std::sort(host.begin(), host.end(), compare_region_neighbor_army_diff);
				
				for (Regions::iterator nt = neutrals.begin(); nt != neutrals.end(); ++nt)
				{
					attack_region = *nt;
					int attack_armies = int(attack_region->GetArmies() * neutral_army_attack);
					// Attack if enough power, but not if it might have affections towards our own army
					if (Region::CalculateAttackProbability(region, attack_region) > neutral_army_attack &&
						Region::CalculateAttackProbability(defense_armies, region->GetArmies() - attack_armies) <= 0.8f)
					{
						MoveArmy(region, attack_region, attack_armies);
					}
				}
				
				continue;
			}

#if 1
			// Sort the neutrals according to super region priority
			std::sort(neutrals.begin(), neutrals.end(), compare_regions_bot_priority);

			// Note: Fix this better later on
			{
				Regions attack;
				// Come together
				for (Regions::iterator nt = neutrals.begin(); nt != neutrals.end(); ++nt)
				{
					attack_region = *nt;
					// Attack if enough power
					// Note: Double attack power is used to ensure a success
					if (Region::CalculateAttackProbability(region, attack_region) > neutral_army_attack)
					{
						MoveArmy(region, attack_region, int(attack_region->GetArmies() * neutral_army_attack));
						attack.push_back(attack_region);
					}
				}

				// Add most of it
				if (!attack.empty())
				{
					int share_armies = region->GetArmies() / attack.size();
					for (Regions::iterator nt = attack.begin(); nt != attack.end(); ++nt)
					{
						attack_region = *nt;
						MoveArmy(region, attack_region, share_armies);
					}

					// Add the rest
					Regions::iterator nt = attack.begin();
					while (region->GetArmies() > 1)
					{
						attack_region = *nt;
						// Add one to each region
						MoveArmy(region, attack_region, 1);
						++nt;
						// Go around
						if (nt == attack.end())
							nt = attack.begin();
					}
				}
			}

#else

			// Get amount of armies around the region
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

			// Attack if enough power
			// Note: This was set to 1.5 in previous project. Probably not needed now when the luck factor is so low.
			if (Region::CalculateAttackProbability(region, optimal) > 1.0f)
			{
				// Note: How many armies moved should be changed later on
				MoveArmy(region, optimal, region->GetArmies()-1);
			}
#endif
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
		// Keep in mind that splitting up an army is mostly a bad idea due to the
		// randomness in this engine
		region = *it;
		// Get connected regions
		Regions landmass = GetConnectedRegions(region);
		// Get regions that have trouble
		Regions use = Region::GetDifferenceUnsorted(landmass, inner);
		// Find closest regions for this landmass
		// Note: Should return multiple paths to weight down moving only to lower count region(Or keep it as it is, a neat "feature")
		std::vector<Region *> path = g_game->GetMap()->FindClosestRegion(region, use);
			
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
	(void)round;
	// Changing strategy depending on how long it has gone
}

// Place armies on a region
// Smart placement
// Note: More checks could be made
void Unknown::PlaceArmy(Region * region, int amount)
{
	// Don't even dare go there
	if (amount > m_armies)
		amount = m_armies;
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
	// Don't go too big
	if (amount >= source->GetArmies())
		amount = source->GetArmies() - 1;
	// Move temporary armies here
	source->MoveArmies(amount);

	// New system
	m_movements.AddMovement(source, target, amount);
}

// Gets how much is attacked on a single region
int Unknown::GetAttackRegion(const Region * region) const
{
	return m_movements.GetArmiesToRegion(region);
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
	if (m_movements.GetMovements().empty())
		NoMoves();
	// Move everything
	else
	{
		const ArmyMovementList & movements = m_movements.GetMovements();
		for (ArmyMovementList::const_iterator it = movements.begin(); it != movements.end(); ++it)
			AttackTransfer(GetName(), it->from->GetId(), it->to->GetId(), it->armies);
	}
	
	// And flush
	Send();
	
	// Store own movements
	g_game->GetHistory()->AddMovements(m_movements);

	// Reset previous moves
	m_movements.Reset();
}
