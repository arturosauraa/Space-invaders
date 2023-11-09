#include <raylib.h>
#include <iostream>
#include <deque>
#include <vector>
#include <random>

using namespace std;

Texture2D spaceshipTexture;
Texture2D invaderTexture;
const int screen_width = 1000;
const int screen_height = 800;
bool gameOver = false;
bool gameWon = false;
int score = 0;


double lastUpdateTime = 0;

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}



class Spaceship
{
public:

    int x = screen_width / 2 - 50;
    int y = screen_height - 100;
    int speed = 5;
    int width = 65;
    int height = 65;
    deque<Vector2> laserDeque;

    void Draw()
    {
        DrawTextureEx(spaceshipTexture, Vector2{(float)x,(float)y }, 0, 0.2, WHITE);

    for (size_t i = 1; i < laserDeque.size(); i++) {
        // Update the y position of each laser
        laserDeque[i].y -= 3;
        DrawRectangle(laserDeque[i].x, laserDeque[i].y, 10, 30, WHITE);
    }

    }

    Spaceship()
    {
    Image imageShip = LoadImage("image/spaceship.png");
    spaceshipTexture = LoadTextureFromImage(imageShip);
    UnloadImage(imageShip);
    // Initialize the laserDeque with a dummy laser outside the visible area
    laserDeque = {Vector2{-100, -100}};
    }

    ~Spaceship()
    {
        UnloadTexture(spaceshipTexture);
    }

    void Update()
    {
        // Remove lasers that have gone above the screen
        for (size_t i = 0; i < laserDeque.size(); i++) 
        {
            if (laserDeque[i].y < 0) 
            {
                // Remove the laser at index i when its y position is less than 0
                laserDeque.erase(laserDeque.begin() + i);
            }
        }
        
        if(IsKeyDown(KEY_LEFT)){
            x = x - speed;
        }
        if(IsKeyDown(KEY_RIGHT)){
            x = x + speed;
        }
        if(IsKeyPressed(KEY_UP))
        {
            Laser();
        }
        LimitMovement();

    }

    void LimitMovement()
    {
        if(x <= 0){
            x = 0;
        }
        if(x + width >= GetScreenWidth()){
            x = GetScreenWidth() - width;
        } 
    }

    void Laser()
    {
        // Remove the initial dummy laser if it's the first shot
        if (laserDeque.size() == 1 && laserDeque[0].x == -100 && laserDeque[0].y == -100) 
        {
            laserDeque.pop_front();
        }

        // Add a new laser to the deque
        laserDeque.push_back(Vector2{(float)x + width / 2 - 6, (float)y - 30});
    }
};

class Invader
{
public:
    int x, y, width, height;
    deque<Vector2> invaderDeque;
    bool moveDirection;
    deque<Vector2> laserDeque;

    Invader()
    {
    Image imageInvader = LoadImage("image/invader.png");
    invaderTexture = LoadTextureFromImage(imageInvader);
    UnloadImage(imageInvader);
    width = 65;
    height = 65;
    x = 60;
    y = 100;
    moveDirection = false;
    // Initialize the laserDeque with a dummy laser outside the visible area
    laserDeque = {Vector2{-100, -100}};

    for(int i = 0; i <= 26; i++)
    {
        if(x + 100 > GetScreenWidth())
        {
            x = 60;
            y += 100;
        }
        invaderDeque.push_back(Vector2{(float)x,(float) y});
        x += 100;
    }
    }

    ~Invader()
    {
        UnloadTexture(invaderTexture);
    }

    void Draw()
    {
        for(Vector2 invader: invaderDeque)
        {
            DrawTextureEx(invaderTexture, Vector2{invader.x, invader.y}, 0, 0.2, WHITE);
        }

        for (size_t i = 1; i < laserDeque.size(); i++) {
        // Update the y position of each laser
        laserDeque[i].y += 3;
        DrawRectangle(laserDeque[i].x + 40, laserDeque[i].y + 50, 10, 30, GREEN);
    }
        
    }

    void Update()
    {
        Move();
        if(eventTriggered(1.5))
        {
            RandomLaser();
        }
        // Remove lasers that have gone above the screen
        for (size_t i = 0; i < laserDeque.size(); i++) 
        {
            if (laserDeque[i].y > GetScreenHeight()) 
            {
                // Remove the laser at index i when its y position is less than 0
                laserDeque.erase(laserDeque.begin() + i);
            }
        }

        if(invaderDeque.empty())
        {
            gameWon = true;
        }

    }

    void Move()
    {
        for(int i = 0; i <= invaderDeque.size(); i++)
        {
            if(!moveDirection)
            {
                invaderDeque[i].x -= 1;
            }
            if(moveDirection)
            {
                invaderDeque[i].x += 1;
            }
            if(invaderDeque[i].x + 50<= 0){
                moveDirection = true;
            }
            if(invaderDeque[i].x + 80 >= GetScreenWidth())
            {
                moveDirection = false;
            }
        }
    }

    void RandomLaser()
    {
        if (laserDeque.size() == 1 && laserDeque[0].x == -100 && laserDeque[0].y == -100) 
        {
            laserDeque.pop_front();
        }

        int randomIndex = GetRandomValue(0, invaderDeque.size() - 1);
        laserDeque.push_back(Vector2{invaderDeque[randomIndex]});
    }


};



int main()
{
    //Window Variables


    InitWindow(screen_width, screen_height, "Space Invaders");
    SetTargetFPS(60);


    Spaceship spaceship = Spaceship();
    Invader invader = Invader();



    while(WindowShouldClose() == false)
    {
        BeginDrawing();

        if(!gameOver)
        {
            if(!gameWon){
            spaceship.Update();
            invader.Update();
            }

        }

        for(int i = 0; i <= invader.laserDeque.size(); i++)
        {
            Rectangle rec = {(float)invader.laserDeque[i].x, (float)invader.laserDeque[i].y, (float)invader.width, (float)invader.height};
            if(CheckCollisionCircleRec(Vector2{(float)spaceship.x + 20, (float)spaceship.y + 20}, 5, Rectangle(rec)))
            {
                gameOver = true;
            }
        }
        if(!spaceship.laserDeque.empty())
        {
            for(int i = 0; i < spaceship.laserDeque.size(); i++)
            {
                Rectangle laserShipRec = {spaceship.laserDeque[i].x,spaceship.laserDeque[i].y, 10, 30};

                for(int j = 0; j < invader.invaderDeque.size(); j++)
                {
                    
                    Rectangle invaderRec = {invader.invaderDeque[j].x, invader.invaderDeque[j].y, 65, 65};
                    if(CheckCollisionRecs(laserShipRec, invaderRec))
                    {
                        spaceship.laserDeque.erase(spaceship.laserDeque.begin() + i);
                        invader.invaderDeque.erase(invader.invaderDeque.begin() + j);
                        score++;
                        
                    }
                }
            }
        }





        ClearBackground(BLACK);

        spaceship.Draw();
        invader.Draw();
        if(gameWon)
        {
            DrawText("YOU WON", screen_width /2 - 200, screen_height / 2 - 200, 100, WHITE);
        }
        if(gameOver)
        {
            DrawText("GAME OVER", screen_width /2 - 400, screen_height - 200, 100, WHITE);
        }

        DrawText(TextFormat("%i", score), screen_width / 2 - 20, 20, 80, WHITE);


        EndDrawing();

    }
    
    CloseWindow();
    return 0;
} 