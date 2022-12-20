#ifndef __POLICY_H__
#define __POLICY_H__
//header file content

// forward declare
class GameState;

class PlayerPolicy {
    public:
        virtual GameState *get_best_move(GameState *root_state) = 0;
        // PlayerPolicy() {};  // this should never be called
};

#endif