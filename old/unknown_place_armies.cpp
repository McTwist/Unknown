
// Note: This strategy is quite a defensive one, could be used depending on the situation

// Create temporary list
Regions regions = m_regions;
// Remove regions with no enemy neighbor
regions.remove_if(remove_region_no_enemy_neighbor);
// Sort according to neighbor effective armies
regions.sort(compare_region_neighbor_army_diff);

// Add armies to regions, but be aware to not get stuck in a loop
for (int i = m_armies; i > 0 && m_armies > 0; --i)
{
	// Locate best regions to place on
	for (Regions::iterator it = regions.begin(); it != regions.end(); ++it)
	{
		region = *it;
	}
	
	// Place armies
	if (amount > 0)
	{
		PlaceArmies(GetName(), region->GetId(), amount);
		region->AddArmies(-amount);
		m_armies -= amount;
		amount = 0;
	}
}
