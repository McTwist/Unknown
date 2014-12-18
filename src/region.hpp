#pragma once
#ifndef REGION_HPP
#define REGION_HPP

#include <list>
#include <vector>

class Region;
class SuperRegion;

// Regions list
typedef std::vector<Region *> Regions;

// Region

class Region
{
	friend class RegionNode;
public:
	Region(int id);
	
	int GetId() const;
	void AddNeighbor(Region * region);
	const Regions & GetNeighbors() const;
	
	void SetSuperRegion(SuperRegion *);
	void SetArmies(int armies);
	void AddArmies(int armies);
	void MoveArmies(int armies);
	void SetOwner(class Bot * owner);
	
	SuperRegion * GetSuperRegion() const;
	int GetArmies() const;
	int GetCurrentArmies() const;
	class Bot * GetOwner() const;
	float GetPriority() const;
	
	// Checks
	static bool IsNeighbor(const Region * regionl, const Region * regionr);
	static bool IsChokepoint(const Region * regionl, const Region * regionr);
	static Regions GetSameNeighbors(const Region * regionl, const Region * regionr);
	static float CalculateAttackProbability(const Region * from, const Region * to);
	static float CalculateAttackProbability(int from, int to);
	
	// Multiple
	static Regions GetCombine(const Regions & regionsl, const Regions & regionsr);
	static int GetRegionsArmies(const Regions & regions);
	
	// Advanced math
	// Needs to be sorted and unique
	static Regions GetUnion(const Regions & regionsl, const Regions & regionsr);
	static Regions GetIntersection(const Regions & regionsl, const Regions & regionsr);
	static Regions GetDifference(const Regions & regionsl, const Regions & regionsr);
	static Regions GetSymmetricDifference(const Regions & regionsl, const Regions & regionsr);
	// Does not be sorted or unique
	static Regions GetUnionUnsorted(const Regions & regionsl, const Regions & regionsr);
	static Regions GetIntersectionUnsorted(const Regions & regionsl, const Regions & regionsr);
	static Regions GetDifferenceUnsorted(const Regions & regionsl, const Regions & regionsr);
	static Regions GetSymmetricDifferenceUnsorted(const Regions & regionsl, const Regions & regionsr);
	
private:
	
	static void Sort(Regions & regions);
	static void Unique(Regions & regions);
	
	int m_id;
	SuperRegion * m_super_region;
	int m_armies;
	class Bot * m_owner;
	
	int m_calculated_armies;
	
	Regions m_neighbors;
};

// Compare function
inline bool compare_id(const Region * a, const Region * b)
{
	return a->GetId() < b->GetId();
}
inline bool compare_army(const Region * a, const Region * b)
{
	return a->GetArmies() < b->GetArmies();
}

// Searchable region node
#include "stlastar.h"

class RegionNode : public AStarState<RegionNode>
{
public:
	RegionNode();
	RegionNode(const Region * region);
	
	const Region * GetRegion() const;
	
	float GoalDistanceEstimate(RegionNode & nodeGoal);
	bool IsGoal(RegionNode & nodeGoal);
	bool GetSuccessors(class AStarSearch<RegionNode> * astarsearch, RegionNode * parent_node);
	float GetCost(RegionNode & successor);
	bool IsSameState(RegionNode & rhs);
	
private:
	
	const Region * m_parent;
};

#endif // REGION_HPP