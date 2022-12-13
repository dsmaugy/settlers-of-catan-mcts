#ifndef __CATAN_PLAYER_H__
#define __CATAN_PLAYER_H__

#include <unordered_set>
#include "hex.h"

//header file content


class Player {

public:
    PlayerPolicy player_policy;

    static std::unordered_set<std::pair<(Hex, Hex)> roads;


};

#endif