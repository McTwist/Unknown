#include "super_region.hpp"

#include "region.hpp"

SuperRegion::SuperRegion(int id, int bonus)
: m_id(id)
, m_bonus(bonus)
{
}

// Get id
int SuperRegion::GetId() const
{
	return m_id;
}

// Get current bonus
int SuperRegion::GetBonus() const
{
	return m_bonus;
}

// Get possible priority value
// Higher value is depending on the bonus and the bonus
// Cost to get it could also be an extra addition to reduce priority
float SuperRegion::GetPriority(unsigned int taken) const
{
	int left = m_region_map.size() - taken;
	// Taken over priority
	float priority = (taken >= m_region_map.size()) ? m_bonus : (float)m_bonus / (float)left;
	// Wasteland suffering
	priority -= (float)m_wasteland.size();
	return priority;
}

// Add region to super region
void SuperRegion::AddRegion(Region * region)
{
	// Add to id list
	m_region_map.insert(std::make_pair(region->GetId(), region));
	// Add to region list
	m_regions.push_back(region);
	// Set the current super region
	region->SetSuperRegion(this);
	// Calculate super region neighbors
	const Regions & neighbors = region->GetNeighbors();
	for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
	{
		Region * neighbor = *it;
		// Not itself and not added before
		if (neighbor->GetSuperRegion() != this && 
			std::find(m_neighbors.begin(), m_neighbors.end(), neighbor->GetSuperRegion()) == m_neighbors.end())
		{
			m_neighbors.push_back(neighbor->GetSuperRegion());
		}
	}
}

// Get region if it exists
Region * SuperRegion::GetRegion(int id) const
{
	RegionMap::const_iterator it = m_region_map.find(id);
	if (it == m_region_map.end())
		return 0;
	return it->second;
}

// Get regions in the super region
const Regions & SuperRegion::GetRegions() const
{
	return m_regions;
}

// Get super region neighbors
const SuperRegions & SuperRegion::GetNeighbors() const
{
	return m_neighbors;
}

// Add wasteland to calculations
void SuperRegion::AddWasteland(Region * region)
{
	m_wasteland.push_back(region);
}

// Remove active wasteland
void SuperRegion::RemoveWasteland(Region * region)
{
	m_wasteland.erase(std::remove(m_wasteland.begin(), m_wasteland.end(), region), m_wasteland.end());
}

// Get amount of regions for a bot
int SuperRegion::GetBotRegionCount(const Bot * bot) const
{
	int count = 0;
	for (Regions::const_iterator it = m_regions.begin(); it != m_regions.end(); ++it)
		if ((*it)->GetOwner() == bot)
			++count;
	return count;
}
