#include <iostream>
#include <SFML/Graphics.hpp>
#include "Player.h" 
#include "zombieArena.h"
#include "textureHolder.h"
#include "bullet.h"
#include "pickup.h"
#include <sstream>
#include <fstream>
#include <SFML/Audio.hpp>

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

    //Create a couple of pickups
    Pickup healthPickup(1);
    Pickup ammoPickup(2);

    //About the game
    int score = 0;

    int hiScore = 0;

    //For the home/game over screen
    Sprite spriteGameOver;

    Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");

    spriteGameOver.setTexture(textureGameOver);

    spriteGameOver.setPosition(0, 0);

    //Image is 1920x1080 - needs scaling for large displays
    spriteGameOver.setScale(resolution.x / 1920, resolution.y / 1080);

    //Create a view for the HUD
    View hudView(FloatRect(0, 0, resolution.x, resolution.y));

    //Create a sprite for the ammo icon
    Sprite spriteAmmoIcon;

    Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");

    spriteAmmoIcon.setTexture(textureAmmoIcon);

    spriteAmmoIcon.setPosition(20, resolution.y - 200);

    //Load the font
    Font font;

    font.loadFromFile("fonts/zombiecontrol.ttf");

    //Paused
    Text pausedText;

    pausedText.setFont(font);

    pausedText.setCharacterSize(155);

    pausedText.setFillColor(Color::White);

    pausedText.setString("Press Enter \nto continue");

    //Place in middle of screen
    FloatRect pausedRect = pausedText.getLocalBounds();

    pausedText.setOrigin(pausedRect.left + pausedRect.width / 2.0f, pausedRect.top + pausedRect.height / 2.0f);

    pausedText.setPosition(resolution.x / 2, resolution.y / 2);

    // Game Over
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(125);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setString("Press Enter to play");

    FloatRect gameOverRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gameOverRect.left + gameOverRect.width / 2.0f, gameOverRect.top + gameOverRect.height / 2.0f);
    gameOverText.setPosition(resolution.x / 2, resolution.y / 2);

    // Levelling up
    Text levelUpText;
    levelUpText.setFont(font);
    levelUpText.setCharacterSize(80);
    levelUpText.setFillColor(Color::White);
    levelUpText.setPosition(150, 250);
    std::stringstream levelUpStream;
    levelUpStream <<
        "1- Increased rate of fire" <<
        "\n2- Increased clip size(next reload)" <<
        "\n3- Increased max health" <<
        "\n4- Increased run speed" <<
        "\n5- More and better health pickups" <<
        "\n6- More and better ammo pickups";
    levelUpText.setString(levelUpStream.str());

    FloatRect levelUpRect = levelUpText.getLocalBounds();
    levelUpText.setOrigin(levelUpRect.left + levelUpRect.width / 2.0f, levelUpRect.top + levelUpRect.height / 2.0f);
    levelUpText.setPosition(resolution.x / 2, resolution.y / 2);

    // Ammo
    Text ammoText;
    ammoText.setFont(font);
    ammoText.setCharacterSize(55);
    ammoText.setFillColor(Color::White);
    ammoText.setPosition(200, resolution.y - 200);

    // Score
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(55);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(20, 0);

    //Load the high score from a text file
    std::ifstream inputFile("gamedata/scores.txt");

    if (inputFile.is_open())
    {

        inputFile >> hiScore;
        inputFile.close();

    }

    // Hi Score
    Text hiScoreText;
    hiScoreText.setFont(font);
    hiScoreText.setCharacterSize(55);
    hiScoreText.setFillColor(Color::White);
    hiScoreText.setPosition(resolution.x - 400, 0);
    std::stringstream s;
    s << "Hi Score:" << hiScore;
    hiScoreText.setString(s.str());

    // Zombies remaining
    Text zombiesRemainingText;
    zombiesRemainingText.setFont(font);
    zombiesRemainingText.setCharacterSize(55);
    zombiesRemainingText.setFillColor(Color::White);
    zombiesRemainingText.setPosition(resolution.x - 400, resolution.y - 200);
    zombiesRemainingText.setString("Zombies: 100");

    // Wave number
    int wave = 0;
    Text waveNumberText;
    waveNumberText.setFont(font);
    waveNumberText.setCharacterSize(55);
    waveNumberText.setFillColor(Color::White);
    waveNumberText.setPosition(resolution.x * 0.66, resolution.y - 200);
    waveNumberText.setString("Wave: 0");

    // Health bar
    RectangleShape healthBar;
    healthBar.setFillColor(Color::Red);
    healthBar.setPosition(resolution.x * 0.33, resolution.y - 200);

    //When did we last update the HUD?
    int framesSinceLastHUDUpdate = 0;

    //How often (in frames) should we update the HUD?
    int fpsMeasurementFrameInterval = 1000;

    //Prepare the hit sound
    SoundBuffer hitBuffer;

    hitBuffer.loadFromFile("sound/hit.wav");

    Sound hit;

    hit.setBuffer(hitBuffer);

    // Prepare the splat sound
    SoundBuffer splatBuffer;
    splatBuffer.loadFromFile("sound/splat.wav");
    sf::Sound splat;
    splat.setBuffer(splatBuffer);

    // Prepare the shoot sound
    SoundBuffer shootBuffer;
    shootBuffer.loadFromFile("sound/shoot.wav");
    Sound shoot;
    shoot.setBuffer(shootBuffer);

    // Prepare the reload sound
    SoundBuffer reloadBuffer;
    reloadBuffer.loadFromFile("sound/reload.wav");
    Sound reload;
    reload.setBuffer(reloadBuffer);

    // Prepare the failed sound
    SoundBuffer reloadFailedBuffer;
    reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
    Sound reloadFailed;
    reloadFailed.setBuffer(reloadFailedBuffer);

    // Prepare the powerup sound
    SoundBuffer powerupBuffer;
    powerupBuffer.loadFromFile("sound/powerup.wav");
    Sound powerup;
    powerup.setBuffer(powerupBuffer);

    // Prepare the pickup sound
    SoundBuffer pickupBuffer;
    pickupBuffer.loadFromFile("sound/pickup.wav");
    Sound pickup;
    pickup.setBuffer(pickupBuffer);

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

                    wave = 0;

                    score = 0;

                    //Prepare the gun and ammo for next game
                    currentBullet = 0;
                    bulletsSpare = 24;
                    bulletsInClip = 6;
                    clipSize = 6;
                    fireRate = 1;

                    //Reset the players stats
                    player.resetPlayerStats();

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

                            reload.play();

                        }

                        else if (bulletsSpare > 0)
                        {

                            //Only few bullets left
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;

                            reload.play();

                        }

                        else
                        {

                            reloadFailed.play();

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
                    shoot.play();
                    bulletsInClip--;

                }

            } //End fire a bullet

        }//End wasd while playing

        //Handle the levelling up state
        if (state == State::LEVELLING_UP)
        {

            if (event.key.code == Keyboard::Num1)
            {

                //Increase fire rate
                fireRate++;

                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num2)
            {

                //Increase clip size
                clipSize += clipSize;

                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num3)
            {

                //Increase health
                player.upgradeHealth();

                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num4)
            {

                //Increase speed
                player.upgradeSpeed();

                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num5)
            {

                //Upgrade health pickup
                healthPickup.upgrade();

                state = State::PLAYING;

            }

            if (event.key.code == Keyboard::Num6)
            {

                //Upgrade ammo pickup 
                ammoPickup.upgrade();

                state = State::PLAYING;

            }

            if (state == State::PLAYING)
            {

                //Increase the wave number
                wave++;

                //Prepare the level - we will update this later
                arena.width = 500 * wave;

                arena.height = 500 * wave;

                arena.left = 0;

                arena.top = 0;

                //int tileSize = 50; //We will update this later

                int tileSize = createBackground(background, arena);

                //Spawn the player in the middle of the arena
                player.spawn(arena, resolution, tileSize);

                //Configure the pick-ups
                healthPickup.setArena(arena);
                ammoPickup.setArena(arena);

                //Create a horde of zombies
                numZombies = 5 * wave;

                //Delete the previously allocated memory (if it exists)
                delete[] zombies; //Note use of delete[] - should use [] when deleting arrays from heap

                zombies = createHorde(numZombies, arena);

                numZombiesAlive = numZombies;

                //Play the powerup sound
                powerup.play();

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

            //Update the pickups
            healthPickup.update(dtAsSeconds);
            ammoPickup.update(dtAsSeconds);

            // Collision detection
// Have any zombies been shot?
            for (int i = 0; i < 100; i++)
            {
                for (int j = 0; j < numZombies; j++)
                {
                    if (bullets[i].isInFlight() &&
                        zombies[j].isAlive())
                    {
                        if (bullets[i].getPosition().intersects
                        (zombies[j].getPosition()))
                        {
                            // Stop the bullet
                            bullets[i].stop();

                            // Register the hit and see if it was a kill
                            if (zombies[j].hit()) {
                                // Not just a hit but a kill too
                                score += 10;
                                if (score >= hiScore)
                                {
                                    hiScore = score;
                                }

                                numZombiesAlive--;

                                // When all the zombies are dead (again)
                                if (numZombiesAlive == 0) {
                                    state = State::LEVELLING_UP;
                                }
                            }

                            //Make a splat sound
                            splat.play();

                        }
                    }

                }
            }// End zombie being shot


            // Have any zombies touched the player?
            for (int i = 0; i < numZombies; i++)
            {
                if (player.getPosition().intersects
                (zombies[i].getPosition()) && zombies[i].isAlive())
                {

                    if (player.hit(gameTimeTotal))
                    {
                        hit.play();
                    }

                    if (player.getHealth() <= 0)
                    {
                        state = State::GAME_OVER;

                        std::ofstream outputFile("gamedata/scores.txt");
                        outputFile << hiScore;
                        outputFile.close();

                    }
                }
            }// End player touched


            // Has the player touched health pickup?
            if (player.getPosition().intersects
            (healthPickup.getPosition()) && healthPickup.isSpawned())
            {
                player.increaseHealthLevel(healthPickup.gotIt());

                //Play a sound
                pickup.play();

            }// End player touch health


            // Has the player touched ammo pickup?
            if (player.getPosition().intersects
            (ammoPickup.getPosition()) && ammoPickup.isSpawned())
            {
                bulletsSpare += ammoPickup.gotIt();

                //Play a sound
                reload.play();

            } // End player touch ammo

        //Size up the health bar
        healthBar.setSize(Vector2f(player.getHealth() * 3, 70));

        //Increment the number of frames since the last HUD calculation
        framesSinceLastHUDUpdate++;

        //Calculate FPS every fpsMeasurementFrameInterval frames
        if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
        {

            //Update game HUD text 
            std::stringstream ssAmmo;
            std::stringstream ssScore;
            std::stringstream ssHiScore;
            std::stringstream ssWave;
            std::stringstream ssZombiesAlive;

            //Update the ammo text
            ssAmmo << bulletsInClip << " /" << bulletsSpare;

            ammoText.setString(ssAmmo.str());

            //Update the score text
            ssScore << "Score:" << score;

            scoreText.setString(ssScore.str());

            //Update the high score text
            ssHiScore << "Hi Score:" << hiScore;

            hiScoreText.setString(ssHiScore.str());

            //Update the wave
            ssWave << "Wave:" << wave;

            waveNumberText.setString(ssWave.str());

            //Update the high score text
            ssZombiesAlive << "Zombies:" << numZombiesAlive;

            zombiesRemainingText.setString(ssZombiesAlive.str());

            framesSinceLastHUDUpdate = 0;


        } //End HUD Update

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

            //Draw the pick-ups, if currently spawned
            if (ammoPickup.isSpawned())
            {

                window.draw(ammoPickup.getSprite());

            }

            if (healthPickup.isSpawned())
            {

                window.draw(healthPickup.getSprite());

            }

            //Draw the crosshair
            window.draw(spriteCrosshair);

            //Switch to the HUD view
            window.setView(hudView);

            //Draw all the HUD elements
            window.draw(spriteAmmoIcon);
            
            window.draw(ammoText);

            window.draw(scoreText);

            window.draw(hiScoreText);

            window.draw(healthBar);

            window.draw(waveNumberText);

            window.draw(zombiesRemainingText);

        }

        if (state == State::LEVELLING_UP)
        {

            window.draw(spriteGameOver);

            window.draw(levelUpText);

        }

        if (state == State::PAUSED)
        {

            window.draw(pausedText);

        }

        if (state == State::GAME_OVER)
        {

            window.draw(spriteGameOver);

            window.draw(gameOverText);

            window.draw(scoreText);

            window.draw(hiScoreText);

        }

        window.display();

    }

    //Delete the previously allocated memory (if it exists)
    delete[] zombies;

    return 0;
}

