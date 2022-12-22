#include <iostream>
#include <SFML/Graphics.hpp>
#include "Player.h" 
#include "zombieArena.h"
#include "textureHolder.h"
#include "bullet.h"


using namespace sf;

int main()
{

    TextureHolder holder;

    //The game will always be in one of four states
    enum class State {PAUSED, LEVELLING_UP, GAME_OVER, PLAYING };

    //Start with the GAME_OVER state
    State state = State::GAME_OVER;

    //Get the screen resolution and create an sfml window
    Vector2f resolution;

    resolution.x = VideoMode::getDesktopMode().width;

    resolution.y = VideoMode::getDesktopMode().height;

    RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

    //Create an sfml view for the main action
    View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

    //Clock for timing everything
    Clock clock;

    //How long has the PLAYING  state been active?
    Time gameTimeTotal;

    //Where is the mouse in relation to world coordinates?
    Vector2f mouseWorldPosition;

    //Where is the mouse in relation to screen coordinates?
    Vector2i mouseScreenPosition;

    //Create an instance of the player class
    Player player;

    //The boundaries of the arena
    IntRect arena;

    //Create the background
    VertexArray background;

    ////Load the texture for our background vertex array
    //Texture textureBackground;

    //textureBackground.loadFromFile("graphics/background_sheet.png");

    Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

    //Prepare for a horde of zombies
    int numZombies = 0; //Number at start of wave
    int numZombiesAlive = 0; //Number to be killed

    Zombie* zombies = nullptr;

    //An array of bullets. 100 bullets should do
    Bullet bullets[100];

    int currentBullet = 0;

    int bulletsSpare = 24;

    int bulletsInClip = 6;

    int clipSize = 6;

    float fireRate = 1;

    //When was the fire button last pressed;
    Time lastPrssed;

    //Hide the mouse pointer and replace it with crosshair
    window.setMouseCursorVisible(false);

    Sprite spriteCrosshair;

    Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");

    spriteCrosshair.setTexture(textureCrosshair);

    spriteCrosshair.setOrigin(25, 25);

    //The main game loop
    while (window.isOpen())
    {

        /*
        ###############################
        Handle Input
        ###############################
        */

        //Handle events
        Event event;

        while (window.pollEvent(event))
        {

            if (event.type == Event::KeyPressed)
            {

                //Pause game while playing
                if (event.key.code == Keyboard::Return && state == State::PLAYING)
                {

                    state = State::PAUSED;

                }

                else if (event.key.code == Keyboard::Return && state == State::PAUSED)
                {

                    state = State::PLAYING;

                    //Reset the clock so there isnt a frame jump
                    clock.restart();

                }
                //Start a new game while in GAME_OVER state
                else if (event.key.code == Keyboard::Return && state == State::GAME_OVER)
                {

                    state = State::LEVELLING_UP;

                }

                if (state == State::PLAYING)
                {

                    //populated later - if a key other than a return is pressed during a game play

                    //Reloading
                    if (event.key.code == Keyboard::R)
                    {

                        if (bulletsSpare >= clipSize)
                        {

                            //Plenty of bullets. Reload.
                            bulletsInClip = clipSize;
                            bulletsSpare -= clipSize;

                        }

                        else if (bulletsSpare > 0)
                        {

                            //Only few bullets left
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;

                        }

                        else
                        {

                            //More here later - reload failed sound

                        }

                    } //End if (event.key.code == Keyboard::R)

                }

            }//END if (event.type == Event::KeyPressed)
           
        }//End event polling

        //Handle the player quitting 
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {

            window.close();

        }

        if (state == State::PLAYING)
        {

            //Handle the pressing and releasing of the wasd keys
            if (Keyboard::isKeyPressed(Keyboard::W))
            {

                player.moveUp();

            }

            else
            {

                player.stopUp();

            }

            if (Keyboard::isKeyPressed(Keyboard::S))
            {

                player.moveDown();

            }

            else
            {

                player.stopDown();

            }

            if (Keyboard::isKeyPressed(Keyboard::A))
            {

                player.moveLeft();

            }

            else
            {

                player.stopLeft();

            }

            if (Keyboard::isKeyPressed(Keyboard::D))
            {

                player.moveRight();

            }

            else
            {

                player.stopRight();

            }

            //Fire a bullet
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

                if (gameTimeTotal.asMilliseconds() - lastPrssed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0)
                {

                    //Pass the centre of the player and the centre of the cross-hair to the shoot function
                    bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y,
                        mouseWorldPosition.x, mouseWorldPosition.y);

                    currentBullet++;

                    if (currentBullet > 99)
                    {

                        currentBullet = 0;

                    }

                    lastPrssed = gameTimeTotal;
                    bulletsInClip--;

                }

            } //End fire a bullet

        }//End wasd while playing

        //Handle the levelling up state
        if (state == State::LEVELLING_UP)
        {

            if (event.key.code == Keyboard::Num1)
            {
                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num2)
            {
                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num3)
            {
                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num4)
            {
                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num5)
            {
                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num6)
            {
                state = State::PLAYING;

            }

            if (state == State::PLAYING)
            {

                //Prepare the level - we will update this later
                arena.width = 500;

                arena.height = 500;

                arena.left = 0;

                arena.top = 0;

                //int tileSize = 50; //We will update this later

                int tileSize = createBackground(background, arena);

                //Spawn the player in the middle of the arena
                player.spawn(arena, resolution, tileSize);

                //Create a horde of zombies
                numZombies = 10;

                //Delete the previously allocated memory (if it exists)
                delete[] zombies; //Note use of delete[] - should use [] when deleting arrays from heap

                zombies = createHorde(numZombies, arena);

                numZombiesAlive = numZombies;

                //Reset the clock so there isnt a frame jump
                clock.restart();

            }

        }//End levelling up

        /*
        ######################
        UPDATE THE FRAME
        ######################
        */

        if (state == State::PLAYING)
        {

            //Update the delta time
            Time dt = clock.restart();

            //Update the total game time
            gameTimeTotal += dt;

            //Make a decimal fraction of 1 from the delta time
            float dtAsSeconds = dt.asSeconds();

            //Find the mouse pointer
            mouseScreenPosition = Mouse::getPosition(window); //Note the call to a static function

            //Convert mouse position to world coordinates of mainview
            mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

            //Set the crosshair to the mouse world location

            spriteCrosshair.setPosition(mouseWorldPosition.x, mouseWorldPosition.y);

            //Update the player
            player.update(dtAsSeconds, Mouse::getPosition());

            //Make a note of the players new position
            Vector2f playerPosition(player.getCenter());

            //Make the view centre around the player
            mainView.setCenter(player.getCenter());

            //Loop through each Zombie and update them if alive
            for (int i = 0; i < numZombies; i++)
            {

                if (zombies[i].isAlive()) {

                    zombies[i].update(dt.asSeconds(), playerPosition);

                }

            }

            //Update any bullets that are in-flight
            for (int i = 0; i < 100; i++)
            {

                if (bullets[i].isInFlight())
                {

                    bullets[i].update(dtAsSeconds);

                }

            }

        }//End updating the scene

        /*
        ######################
        DRAW THE SCENE
        ######################
        */

        if (state == State::PLAYING)
        {

            window.clear();

            //set the mainView to be displayed in the window

            //And draw everything related to it
            window.setView(mainView);

            //Draw the background
            window.draw(background, &textureBackground);

            //Draw the zombies
            for (int i = 0; i < numZombies; i++) {

                window.draw(zombies[i].getSprite());

            }

            //Draw the bullets
            for (int i = 0; i < 100; i++)
            {

                if (bullets[i].isInFlight())
                {

                    window.draw(bullets[i].getShape());

                }

            }

            //Draw the player
            window.draw(player.getSprite());

            //Draw the crosshair
            window.draw(spriteCrosshair);

        }

        if (state == State::LEVELLING_UP)
        {


        }

        if (state == State::PAUSED)
        {


        }

        if (state == State::GAME_OVER)
        {


        }

        window.display();

    }

    //Delete the previously allocated memory (if it exists)
    delete[] zombies;

    return 0;
}

