#include <iostream>
#include "game.hpp"

void print_help()
{
    std::cout << "Usage: ./othello [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --autoplay    AI vs AI mode\n";
    std::cout << "  --pvp         Player vs Player mode\n";
    std::cout << "  --pve         Player vs AI mode\n";
    std::cout << "  --help        Show this help message\n";
}

int main(int argc, char *argv[])
{
    Game game;
    if (argc == 1)
    {
        // Default: show help
        print_help();
        return 0;
    }

    std::string arg = argv[1];
    if (arg == "--autoplay")
    {
        game.autoplay();
    }
    else if (arg == "--pvp")
    {
        game.play_pvp();
    }
    else if (arg == "--pve")
    {
        game.play_pve();
    }
    else if (arg == "--help")
    {
        print_help();
    }
    else
    {
        std::cout << "Unknown option: " << arg << "\n";
        print_help();
        return 1;
    }
    return 0;
}