#include "opponent.hpp"

#include "game.hpp"

// Update current armies in regions owned
void Opponent::onOpponentPlaceArmies(const std::string & name, int region, int amount)
{
	// Avoid it being called more than once
	if (GetName() != name)
		return;

	g_game->GetHistory()->AddPlacement(region, amount);
}

void Opponent::onOpponentAttackTransfer(const std::string & name, int source_region, int target_region, int amount)
{
	// Avoid it being called more than once
	if (GetName() != name)
		return;

	g_game->GetHistory()->AddMovement(
		g_game->GetMap()->GetRegion(source_region),
		g_game->GetMap()->GetRegion(target_region),
		amount);
}
