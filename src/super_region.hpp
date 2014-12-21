#pragma once
#ifndef SUPER_REGION_HPP
#define SUPER_REGION_HPP

#include <map>
#include <vector>

class Region;
typedef std::vector<Region *> Regions;
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
	void AddWasteland(Region * region);
	void RemoveWasteland(Region * region);
	
	int GetBotRegionCount(const Bot * bot) const;
	
private:
	int m_id;
	int m_bonus;

	typedef std::map<int, Region *> RegionMap;
	
	RegionMap m_regions;
	Regions m_wasteland;
};

#endif // SUPER_REGION_HPP