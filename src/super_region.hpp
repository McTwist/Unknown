#pragma once
#ifndef SUPER_REGION_HPP
#define SUPER_REGION_HPP

#include <map>

class Region;
class Bot;

// Super Region (Continent)

class SuperRegion
{
public:
	SuperRegion(int id, int bonus);
	
	int GetId() const;
	int GetBonus() const;
	float GetPriority(unsigned int taken = 0) const;
	
	void AddRegion(Region * region);
	Region * GetRegion(int id) const;
	
	int GetBotRegionCount(const Bot * bot) const;
	
private:
	int m_id;
	int m_bonus;

	typedef std::map<int, Region *> RegionMap;
	
	RegionMap m_regions;
};

#endif // SUPER_REGION_HPP