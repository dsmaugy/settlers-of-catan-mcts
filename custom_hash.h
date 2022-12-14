#ifndef __CUSTOM_HASH_GUARD__
#define __CUSTOM_HASH_GUARD__
//header file content


// forward defs
class Hex;
class HexPath;
class HexIntersection;
class Player;
class GameState;


class HashHex {
    public:
        size_t operator()(const Hex& hex) const;
};

class HashPath {
    public:
        size_t operator()(const HexPath& road) const;
};

class HashIntersection {
    public:
        size_t operator()(const HexIntersection& building) const;
};

class HashPlayer {
    public:
        size_t operator()(const Player& player) const;
};

class HashGameState {
    public:
        size_t operator()(const GameState& state) const;
};
#endif 