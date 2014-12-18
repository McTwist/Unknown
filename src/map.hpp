#pragma once
#ifndef MAP_HPP
#define MAP_HPP

#include <map>
#include <set>

#include "super_region.hpp"
#include "region.hpp"

#include "interpreter.hpp"
#include "game_state.hpp"

class Map : public Reader, public GameState
{
public:
	Map();
	~Map();
	
	void Reset();
	
	void ClearState();
	
	// Get
	SuperRegion * GetSuperRegion(int id) const;
	Region * GetRegion(int id) const;
	
	// Locate best way towards a common goal
	std::vector<Region *> FindBestWay(const Region * start, const Region * end) const;
	std::vector<Region *> FindClosestRegion(Region * start, const std::set<Region *> &) const;
	
protected:
	
	// Input
	void onSetupMapSuperRegion(int super_region, int bonus);
	void onSetupMapRegion(int region, int super_region);
	void onSetupMapNeighbor(int region, const std::vector<int> & neighbors);
	void onUpdateMap(int region, const std::string & name, int armies);
	void onOpponentPlaceArmies(const std::string & name, int region, int amount);
	
	void onEndRound();
	
private:
	
	SuperRegion * CreateSuperRegion(int id, int bonus);
	void DestroySuperRegion(SuperRegion * super_region);
	
	Region * CreateRegion(int id);
	void DestroyRegion(Region * region);
	
	typedef std::map<int, SuperRegion *> SuperRegionMap;
	typedef std::map<int, Region *> RegionMap;
	
	SuperRegionMap m_super_regions;
	RegionMap m_regions;
};

#endif // MAP_HPP