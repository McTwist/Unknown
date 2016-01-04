#pragma once
#ifndef RULES_HPP
#define RULES_HPP

// Values that are set to define the rules to change how bot react

class Rules
{
public:
	// These rules are applied to the game in itself
	static int max_rounds;
	static int default_armies_per_round;
	static bool fog_of_war;
	static float luck;
	static float chance_of_attack;
	static float chance_of_defense;
	
	// These rules are applies depending on several factors
	static int neutral_armies;
	static int wasteland_armies;
};

#endif // RULES_HPP
