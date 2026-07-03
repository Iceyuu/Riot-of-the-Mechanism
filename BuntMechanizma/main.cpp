#include <iostream>
#include "Game.h"
#include "Menu.h"

int main()
{
    //init Game engine
    Game game;

    while (game.running())
    {
        //Update
        game.update();

        //Render
        game.render();

    }
    //End of application
    return 0;
}