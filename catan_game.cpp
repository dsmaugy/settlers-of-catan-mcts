#include "catan_game.h"
#include "catan_game_board.h"
#include "catan_player.h"

// Keep a list of possible road locations, update as you build roads
// Same for settlement sites

// Finding adjacent roads: depending on orientation of built road, you can figure out the other two
// that are connected to it.

// Possible moves
// Build a road
// Build a settlemnt
// Upgrade a settlment -> City
// Build a dev card (but not use it right away)
// Use prev held dev cards


// Turn possibility
// 
// CAN build up to 1 item
// CAN play a dev card
// or you can do nothing for either of those

// Next move func

//  roll die, collect resources for all players
//  for current player, call get_all_moves(),
//  feed into that player's policy, to get that
//  player's move. Update the game state to the one
// returned from the player's policy