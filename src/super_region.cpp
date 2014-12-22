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
	int left = m_regions.size() - taken;
	// Taken over priority
	float priority = (taken >= m_regions.size()) ? m_bonus : (float)m_bonus / (float)left;
	// Wasteland suffering
	priority -= (float)m_wasteland.size();
	return priority;
}

// Add region to super region
void SuperRegion::AddRegion(Region * region)
{
	m_regions.insert(std::make_pair(region->GetId(), region));
	region->SetSuperRegion(this);
}

// Get region if it exists
Region * SuperRegion::GetRegion(int id) const
{
	RegionMap::const_iterator it = m_regions.find(id);
	if (it == m_regions.end())
		return 0;
	return it->second;
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
	for (RegionMap::const_iterator it = m_regions.begin(); it != m_regions.end(); ++it)
		if (it->second->GetOwner() == bot)
			++count;
	return count;
}
