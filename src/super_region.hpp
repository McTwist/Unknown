#pragma once
#ifndef SUPER_REGION_HPP
#define SUPER_REGION_HPP

#include <map>

// Super Region (Continent)

// Create a definition to make the process easier
typedef class Region * CRegion;

class SuperRegion
{
public:
	SuperRegion(int id, int bonus);
	
	int GetId() const;
	int GetBonus() const;
	float GetPriority(unsigned int taken = 0) const;
	
	void AddRegion(CRegion region);
	CRegion GetRegion(int id);
	
	int GetBotRegionCount(class Bot * bot);
	
private:
	int m_id;
	int m_bonus;
	std::map<int, CRegion> m_regions;
};

#endif // SUPER_REGION_HPP