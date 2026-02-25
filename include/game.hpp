#ifndef GAME_HPP
#define GAME_HPP

#include "board.hpp"

class Game
{
public:
    Game();
    void play_pvp();
    void autoplay();
    void play_pve();
    // Add play_pve() and other game flow methods as needed
};

#endif
