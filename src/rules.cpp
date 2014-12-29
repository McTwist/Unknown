#include "rules.hpp"

/*
 * These rules are changed depending on type of game
 * More can be added as complicated moves are developed
 */

// How many rounds before a draw is occured
int Rules::max_rounds = 100;

// Whether or not a fog of war is applied to the map
bool Rules::fog_of_war = true;

// Determines how battles luck are determined
float Rules::luck = 0.16f;

// How high probability an attack will destroy one defending army
float Rules::chance_of_attack = 0.6f;

// How high probability a defense will destroy one attacking army
float Rules::chance_of_defense = 0.7f;

// How many armies neutral regions have at start
int Rules::neutral_armies = 2; // Default: 2