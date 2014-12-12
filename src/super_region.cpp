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
	return (taken >= m_regions.size()) ? m_bonus : (float)m_bonus / (float)(m_regions.size() - taken);
}

// Add region to super region
void SuperRegion::AddRegion(Region * region)
{
	m_regions.insert(std::make_pair(region->GetId(), region));
	region->SetSuperRegion(this);
}

// Get region if it exists
Region * SuperRegion::GetRegion(int id)
{
	std::map<int, Region *>::iterator it = m_regions.find(id);
	if (it == m_regions.end())
		return 0;
	return it->second;
}

// Get amount of regions for a bot
int SuperRegion::GetBotRegionCount(Bot * bot)
{
	int count = 0;
	for (std::map<int, Region *>::iterator it = m_regions.begin(); it != m_regions.end(); ++it)
		if (it->second->GetOwner() == bot)
			++count;
	return count;
}
