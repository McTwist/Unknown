#include "game_predictor.hpp"

#include "bot.hpp"
#include "region.hpp"
#include "game.hpp"
#include "rules.hpp"

/*
 * Note about the predictor:
 ** It is divided up in three parts
 ** First part is the current state, which is what we see right now
 ** Second part is what we know that they have had and should own
 ** Final part is the real prediction of what we think they have depending of previous parts
 * Ideas:
 ** A value could be added that could define how much the last one should affect the rest,
 ** that is, depending on how right this predictor is, it will increase the value
 */

// Set maximum army possible
void GamePredictor::SetMaxArmyPerRound(int max_army)
{
	m_max_army_round = max_army;
}

// Get bot current regions
Regions GamePredictor::GetBotRegions(const Bot * bot) const
{
	GameHistory * history = g_game->GetHistory();

	// Get regions you know
	Regions regions = bot->GetRegions();

	// Get known bot regions (Previous information)
	RegionHistoryList history_regions = history->GetBotRegions(bot);
	for (RegionHistoryList::iterator it = history_regions.begin(); it != history_regions.end(); ++it)
		// Take only new ones
		if (std::find(regions.begin(), regions.end(), it->GetRegion()) == regions.end())
			// TODO: Fix const-correctness
			regions.push_back((Region *)it->GetRegion());

	// Get possible bot regions (Calculations...)
	// TODO

	return regions;
}

// Get amount of armies a bot get each round
int GamePredictor::GetBotArmyPerRound(const Bot * bot) const
{
	GameHistory * history = g_game->GetHistory();

	// Starting with default armies
	int armies = Rules::default_armies_per_round;
	// Get predicted regions
	Regions regions = GetBotRegions(bot);

	// Calculate how many super regions the bot owns
	typedef std::map<SuperRegion *, unsigned int> SuperRegionCount;
	SuperRegionCount super_region_amount;
	for (Regions::iterator it = regions.begin(); it != regions.end(); ++it)
	{
		Region * region = *it;
		if (region->GetOwner() == bot)
			super_region_amount[region->GetSuperRegion()] += 1;
	}

	// If owning all regions, it owns the super regions
	// Calculate how much the super regions get each round
	for (SuperRegionCount::iterator it = super_region_amount.begin(); it != super_region_amount.end(); ++it)
		if (it->second == it->first->GetRegions().size())
			armies += it->first->GetBonus();

	// Add in the placements from previous round as a minimum
	int minimum_size = 0;
	if (const RoundHistory * round = history->GetRound(history->GetRound()))
	{
		const RegionHistoryList & list = round->GetHistories();
		for (RegionHistoryList::const_iterator it = list.begin(); it != list.end(); ++it)
		{
			if (it->GetOwner() == bot)
				minimum_size += round->GetRegionPlacement(it->GetRegion());
		}
	}

	return armies > minimum_size ? armies : minimum_size;
}

// Get bot current army a bot could have
int GamePredictor::GetBotCurrentArmy(const Bot * bot) const
{
	GameHistory * history = g_game->GetHistory();

	// Get predicted regions
	Regions regions = GetBotRegions(bot);

	// Get how much armies each region have
	int armies = Region::GetRegionsArmies(regions);

	// Calculate possible amount of armies depending on time
	// TODO

	return armies;
}

// Get bot current active army a bot could have
int GamePredictor::GetBotCurrentActiveArmy(const Bot * bot) const
{
	GameHistory * history = g_game->GetHistory();

	// Get predicted regions
	Regions regions = GetBotRegions(bot);

	// Note: Same calculation as above
	int armies = Region::GetRegionsArmies(regions);

	// Reduce due to amount of regions
	armies -= regions.size();

	return armies;
}

// Get region current army
int GamePredictor::GetRegionCurrentArmy(const Region * region) const
{
	GameHistory * history = g_game->GetHistory();

	// Get region history
	const RoundHistory * round = history->GetRound(region);
	const RegionHistory * region_history = round->GetRegion(region);

	// Check how new it is
	int curr_round = history->GetRound();
	int history_round = region_history->GetRound();

	int diff_round = curr_round - history_round;

	// We got the current round, so just return the current army count
	if (diff_round == 0)
	{
		return region->GetArmies();
	}

	// Calculate how much possible the region could have
	// TODO
	// Predict the amount by calculating the max per round and how long not visible

	return 0;
}
