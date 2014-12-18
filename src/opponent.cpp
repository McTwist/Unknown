#include "opponent.hpp"


// Update current armies in regions owned
void Opponent::onOpponentPlaceArmies(const std::string & name, int region, int amount)
{
	(void)name;
	(void)region;
	(void)amount;
}

void Opponent::onOpponentAttackTransfer(const std::string & name, int source_region, int target_region,int amount)
{
	(void)name;
	(void)source_region;
	(void)target_region;
	(void)amount;
}