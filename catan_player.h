#ifndef __CATAN_PLAYER_H__
#define __CATAN_PLAYER_H__
//header file content
#include <unordered_set>
#include "hex.h"



class Player {

public:
    PlayerPolicy player_policy;

    static std::unordered_set<std:: pair<Hex,Hex> > roads;


};

#endif