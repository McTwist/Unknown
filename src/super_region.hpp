#pragma once
#ifndef SUPER_REGION_HPP
#define SUPER_REGION_HPP

#include <map>
#include <vector>

class Region;
class SuperRegion;
typedef std::vector<Region *> Regions;
class Bot;
typedef std::vector<SuperRegion *> SuperRegions;

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
	const Regions & GetRegions() const;
	const SuperRegions & GetNeighbors() const;
	void AddWasteland(Region * region);
	void RemoveWasteland(Region * region);
	
	int GetBotRegionCount(const Bot * bot) const;
	
private:
	int m_id;
	int m_bonus;

	typedef std::map<int, Region *> RegionMap;
	
	RegionMap m_region_map;
	Regions m_regions;
	Regions m_wasteland;
	SuperRegions m_neighbors;
};

#endif // SUPER_REGION_HPP