#include "region.hpp"

#include "super_region.hpp"
#include "bot.hpp"
#include "rules.hpp"

#include "debug.hpp"

Region::Region(int id)
: m_id(id)
, m_super_region(0)
, m_armies(Rules::neutral_armies)
, m_owner(0)
, m_calculated_armies(0)
{
}

int Region::GetId() const
{
	return m_id;
}

// Add a neighbor
void Region::AddNeighbor(Region * region)
{
	m_neighbors.push_back(region);
}

// Get neighbors
const Regions & Region::GetNeighbors() const
{
	return m_neighbors;
}

// Set super region
void Region::SetSuperRegion(SuperRegion * super_region)
{
	m_super_region = super_region;
}

// Set current amount of armies
void Region::SetArmies(int armies)
{
	m_armies = armies;
	// Reset due to change of value
	m_calculated_armies = 0;
}

// Add armies to the region
void Region::AddArmies(int armies)
{
	m_armies += armies;
}

// Move armies elsewhere
void Region::MoveArmies(int armies)
{
	m_calculated_armies += armies;
}

// Set current bot owner
void Region::SetOwner(Bot * owner)
{
	m_owner = owner;
}

// Get super region
SuperRegion * Region::GetSuperRegion() const
{
	return m_super_region;
}

// Get amount of armies
int Region::GetArmies() const
{
	return m_armies - m_calculated_armies;
}

// Get real amount of armies before any move
int Region::GetCurrentArmies() const
{
	return m_armies;
}

// Get current owner
Bot * Region::GetOwner() const
{
	return m_owner;
}

// Get region priority
float Region::GetPriority() const
{
	// Note: Close super region, less neighbors, could increase this value
	return 1.0f;
}

// Checks if two regions are neighbors
bool Region::IsNeighbor(const Region * regionl, const Region * regionr)
{
	const Regions & neighbors = regionl->GetNeighbors();
	return std::find(neighbors.begin(), neighbors.end(), regionr) != neighbors.end();
}

// Checks if two regions have a chokepoint
bool Region::IsChokepoint(const Region * regionl, const Region * regionr)
{
	// Note: Could check for neighbor, but this is up to the user
	// As the connected regions have no common neighbor, they are the only connection to each other
	return GetSameNeighbors(regionl, regionr).empty();
}

// Checks if two regions have the same neighbors
Regions Region::GetSameNeighbors(const Region * regionl, const Region * regionr)
{
	return GetIntersectionUnsorted(regionl->GetNeighbors(), regionr->GetNeighbors());
}

// Calculates the current attack probability
// Note: Removes one army from the source region
float Region::CalculateAttackProbability(const Region * from, const Region * to)
{
	// Remove one army from attacking as it is not used
	return CalculateAttackProbability(from->GetArmies() - 1, to->GetArmies());
}

// Calculates the current attack probability
float Region::CalculateAttackProbability(int from, int to)
{
	// This should not happen, but is checked to avoid crash
	if (to <= 0)
		return 9000.1f;
	return (from  * Rules::chance_of_attack) / (to * Rules::chance_of_defense);
}

// Combine two lists
Regions Region::GetCombine(const Regions & regionsl, const Regions & regionsr)
{
	Regions result(regionsl);
	result.insert(result.end(), regionsr.begin(), regionsr.end());
	return result;
}

// Get region total army count
int Region::GetRegionsArmies(const Regions & regions)
{
	int army = 0;
	for (Regions::const_iterator it = regions.begin(); it != regions.end(); ++it)
		army += (*it)->GetArmies();
	return army;
}

// Merge left with right
Regions Region::GetUnion(const Regions & regionsl, const Regions & regionsr)
{
	Regions result;
	std::set_union(
		regionsl.begin(), regionsl.end(),
		regionsr.begin(), regionsr.end(),
		back_inserter(result));
	return result;
}

// Get equals
Regions Region::GetIntersection(const Regions & regionsl, const Regions & regionsr)
{
	Regions result;
	std::set_intersection(
		regionsl.begin(), regionsl.end(),
		regionsr.begin(), regionsr.end(),
		back_inserter(result));
	return result;
}

// Difference
Regions Region::GetDifference(const Regions & regionsl, const Regions & regionsr)
{
	Regions result;
	std::set_difference(
		regionsl.begin(), regionsl.end(),
		regionsr.begin(), regionsr.end(),
		back_inserter(result));
	return result;
}

// Symmetric difference
Regions Region::GetSymmetricDifference(const Regions & regionsl, const Regions & regionsr)
{
	Regions result;
	std::set_symmetric_difference(
		regionsl.begin(), regionsl.end(),
		regionsr.begin(), regionsr.end(),
		back_inserter(result));
	return result;
}

// Merge left with right
Regions Region::GetUnionUnsorted(const Regions & regionsl, const Regions & regionsr)
{
	Regions rhl = regionsl, rhr = regionsr;
	Sort(rhl);
	Unique(rhl);
	Sort(rhr);
	Unique(rhr);
	return GetUnion(rhl, rhr);
}

// Get equals
Regions Region::GetIntersectionUnsorted(const Regions & regionsl, const Regions & regionsr)
{
	Regions result, rhl = regionsl, rhr = regionsr;
	Sort(rhl);
	Unique(rhl);
	Sort(rhr);
	Unique(rhr);
	return GetIntersection(rhl, rhr);
}

// Difference
Regions Region::GetDifferenceUnsorted(const Regions & regionsl, const Regions & regionsr)
{
	Regions result, rhl = regionsl, rhr = regionsr;
	Sort(rhl);
	Unique(rhl);
	Sort(rhr);
	Unique(rhr);
	return GetDifference(rhl, rhr);
}

// Symmetric difference
Regions Region::GetSymmetricDifferenceUnsorted(const Regions & regionsl, const Regions & regionsr)
{
	Regions result, rhl = regionsl, rhr = regionsr;
	Sort(rhl);
	Unique(rhl);
	Sort(rhr);
	Unique(rhr);
	return GetSymmetricDifference(rhl, rhr);
}

// Sort regions
void Region::Sort(Regions & regions)
{
#if 1
	std::sort(regions.begin(), regions.end());
#else
	regions.sort();
#endif
}

// Unique regions
void Region::Unique(Regions & regions)
{
#if 1
	regions.erase(std::unique(regions.begin(), regions.end()), regions.end());
#else
	regions.unique();
#endif
}

RegionNode::RegionNode()
: m_parent(0)
{}
RegionNode::RegionNode(const Region * region)
: m_parent(region)
{}

// Get region placed in the node
const Region * RegionNode::GetRegion() const
{
	return m_parent;
}

// Get distance estimated
// Note: This is currently not possible to estimate, so keep it at one.
float RegionNode::GoalDistanceEstimate(RegionNode & nodeGoal)
{
	return 1.0f;
}

// Check if goal
bool RegionNode::IsGoal(RegionNode & nodeGoal)
{
	return m_parent == nodeGoal.m_parent;
}

// Get sucessors
bool RegionNode::GetSuccessors(AStarSearch<RegionNode> * astarsearch, RegionNode * parent_node)
{
	const Region * parent = (parent_node) ? parent_node->m_parent : 0;
	
	RegionNode new_node;
	
	bool corr = true;
	
	const Regions & neighbors = m_parent->GetNeighbors();
	const Region * reg;
	
	// Iterate through neighbors
	for (Regions::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
	{
		reg = *it;
		// Avoid going back
		// Avoid unknown areas
		if (reg != parent && reg->GetOwner())
		{
			// Generate node
			new_node = RegionNode(*it);
			corr = astarsearch->AddSuccessor(new_node);
			// check if allocation went alright
			if (!corr)
				break;
		}
	}
	
	return corr;
}

// Get cost on node
// This changes depending on who owns the region
float RegionNode::GetCost(RegionNode & successor)
{
	float cost = 1.0f;
	// Apply extra cost if does not own it
	if (m_parent->GetOwner() != successor.m_parent->GetOwner())
		cost += successor.m_parent->GetArmies();
	return cost;
}

// Check if same state
bool RegionNode::IsSameState(RegionNode & rhs)
{
	return m_parent == rhs.m_parent;
}
