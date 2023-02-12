#include <iostream>
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <random>
#include <cstdlib>
#include <chrono>

const int MAX_RIGHT = 920;
const int MIN_LEFT = 10;
const int STEP = 20;
const std::string FIGHTER_IMG = "./Resources/Images/fighter.png";
const std::string ALIEN_IMG = "./Resources/Images/alien.png";
const std::string EXPLOSION_IMG1 = "./Resources/Images/explosion/360_F_113148676_25m2BrZbBJ7Pj3LK1VPZaMDAyxEUPdJI.jpg";
const std::string EXPLOSION_IMG2 = "./Resources/Images/explosion/360_F_68741957_Bv0amMRCjorX3rTXdStKM0wbdKuaDe3F.jpg";
const std::string EXPLOSION_IMG3 = "./Resources/Images/explosion/abstract-explosion-of-orange-dust-on-white.JPG";
const std::string EXPLOSION_IMG4 = "./Resources/Images/explosion/istockphoto-114409917-170667a.jpg";
const std::string EXPLOSION_IMG5 = "./Resources/Images/explosion/istockphoto-1147129639-612x612.jpg";
const std::string EXPLOSION_IMG6 = "./Resources/Images/explosion/istockphoto-1352338867-170667a.jpg";
const std::string EXPLOSION_IMG7 = "./Resources/Images/explosion/istockphoto-539093422-170667a.jpg";
const float BULLET_RADIUS = 7.f;
const float SUPER_BULLET_RADIUS = 20.f;
const float PLAYER_INIT_X = 450.0;
const float PLAYER_INIT_Y = 900.0;
const float ALIEN_INIT_X = 350.0;
const float ALIEN_INIT_Y = 10.0;
const float PROJECTILE_WIDTH = 5;
const float PROJECTILE_HEIGHT = 10;
const long int ALIEN_SHOUTING_INTERVAL = 999999999;
const long int FIGHTER_SHOUTING_INTERVAL = 999999999;
const long int WOUND_INTERVAL = 999999999;

class Explosion {
public:
    sf::Sprite sprite;
    std::vector<sf::Texture> textures;
    sf::Texture textureExplosion1;
    sf::Texture textureExplosion2;
    sf::Texture textureExplosion3;
    sf::Texture textureExplosion4;
    sf::Texture textureExplosion5;
    sf::Texture textureExplosion6;
    sf::Texture textureExplosion7;

    Explosion()
    {
        //use a lambda expression
        textureExplosion1.loadFromFile(EXPLOSION_IMG1);
        textureExplosion2.loadFromFile(EXPLOSION_IMG2);
        textureExplosion3.loadFromFile(EXPLOSION_IMG3);
        textureExplosion4.loadFromFile(EXPLOSION_IMG4);
        textureExplosion5.loadFromFile(EXPLOSION_IMG5);
        textureExplosion6.loadFromFile(EXPLOSION_IMG6);
        textureExplosion7.loadFromFile(EXPLOSION_IMG7);
    
        textures.push_back(textureExplosion1);
        textures.push_back(textureExplosion2);
        textures.push_back(textureExplosion3);
        textures.push_back(textureExplosion4);
        textures.push_back(textureExplosion5);
        textures.push_back(textureExplosion6);
        textures.push_back(textureExplosion7);

        sprite.setTexture(textures[0]);
        sprite.setScale(0.2f, 0.2f);
    }

    void setNewTexture() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(0, 6);
        short index = distr(gen);
        sprite.setTexture(textures[index]);
    }

    void setPosition (float x, float y) {
        sprite.setPosition(x, y);
    }

    sf::Vector2f getPosition(){
        return sprite.getPosition();
    }

    void draw(sf::RenderWindow * window) {
        window->draw(sprite);
    }
};

class Player {
public:
    sf::Sprite sprite;
    sf::Texture textureFighterUp;
    Player()
    {
        textureFighterUp.loadFromFile(FIGHTER_IMG);
        sprite.setTexture(textureFighterUp);
        sprite.setScale(0.2f, 0.2f);
        sprite.setPosition(PLAYER_INIT_X, PLAYER_INIT_Y);
    }

    void move(float x, float y){
        sprite.move(x, y);
    }

    sf::Vector2f getPosition(){
        return sprite.getPosition();
    }

    void draw(sf::RenderWindow * window){
        window->draw(sprite);
    }
};

class Alien {
public:
    sf::Sprite sprite;
    sf::Texture textureAlien;
    Alien()
    {
        textureAlien.loadFromFile(ALIEN_IMG);
        sprite.setTexture(textureAlien);
        sprite.setScale(0.05f, 0.05f);
        sprite.setPosition(ALIEN_INIT_X, ALIEN_INIT_Y);
    }

    void move(float x, float y){
        sprite.move(x, y);
    }

    sf::Vector2f getPosition(){
        return sprite.getPosition();
    }

    void draw(sf::RenderWindow * window){
        window->draw(sprite);
    }

    int getNewX() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr(-50, 50);
        float xMove = distr(gen);
        while(xMove + sprite.getPosition().x < MIN_LEFT ||
              xMove + sprite.getPosition().x > MAX_RIGHT)
        {
            xMove = distr(gen);
        }
        
        return xMove;
    }
};

class Bullet {
public:
    sf::CircleShape shape;
    sf::Vector2f currVelocity;
    float maxspeed;

    Bullet(float x, float y, sf::Color color, float radius)
    {
        shape.setPosition(x, y);
        shape.setRadius(radius);
        shape.setFillColor(color);
    }
};

class AlienProjectile {
public:
    sf::RectangleShape shape;
    sf::Vector2f currVelocity;
    float maxspeed;

    AlienProjectile(float x, float y, sf::Color color)
    {
        shape.setPosition(x, y);
        sf::Vector2f v(PROJECTILE_WIDTH, PROJECTILE_HEIGHT);
        shape.setSize(v);
        shape.setFillColor(color);
    }
};

int main() {

    sf::RenderWindow window(sf::VideoMode(1024, 1024), "Space invaders", sf::Style::Close | sf::Style::Resize);
    std::shared_ptr<Player> player = std::make_shared<Player>();
    std::vector<std::shared_ptr<Bullet>> bullets;
    std::vector<std::shared_ptr<AlienProjectile>> projectiles;
    std::shared_ptr<Alien> alien = std::make_shared<Alien>();
    std::shared_ptr<Explosion> explosion = std::make_shared<Explosion>();
    auto start = std::chrono::steady_clock::now();
    auto lastAlienShot = start;
    auto lastFighterShot = start;
    auto lastFighterWound = start;
    auto lastAlienWound = start;
    auto firstExplosion = start;
    short fighterWounds = 0;
    short alienWounds = 0;
    bool fightExplosion = false;
    bool gameOver = false;
    bool win = false;

    while (window.isOpen())
    {
        start = std::chrono::steady_clock::now();
        sf::Event  event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
                std::cout << "Handling closing event" << std::endl;
                exit(EXIT_SUCCESS);
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if(player != nullptr && player->getPosition().x > MIN_LEFT)
                {
                    player->move(-20.f, 0);
                }
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if(player != nullptr && player->getPosition().x < MAX_RIGHT)
                {
                    player->move(20.f, 0);
                }
            }

            if (player != nullptr && sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (start - lastFighterShot).count() > FIGHTER_SHOUTING_INTERVAL)
            {
                sf::Vector2f v = player->getPosition();
                //sprite dimension / 2 
                float x_adjust = (player->sprite.getTexture()->getSize().x * player->sprite.getScale().x)/2.0 - BULLET_RADIUS;
                float y_adjust = BULLET_RADIUS * 2.0;
                std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(v.x + x_adjust,  v.y - y_adjust, sf::Color::Red, BULLET_RADIUS);
                bullets.push_back(bullet);

                lastFighterShot = std::chrono::steady_clock::now();
            }

            if (player != nullptr && sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (start - lastFighterShot).count() > FIGHTER_SHOUTING_INTERVAL)
            {
                sf::Vector2f v = player->getPosition();
                //sprite dimension / 2 
                float x_adjust = (player->sprite.getTexture()->getSize().x * player->sprite.getScale().x)/2.0 - SUPER_BULLET_RADIUS;
                float y_adjust = SUPER_BULLET_RADIUS * 2.0;
                std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(v.x + x_adjust,  v.y - y_adjust, sf::Color::Yellow, SUPER_BULLET_RADIUS);
                bullets.push_back(bullet);

                lastFighterShot = std::chrono::steady_clock::now();
            }    
        }

        //alien move
        if(alien != nullptr && (start - lastAlienShot).count() > ALIEN_SHOUTING_INTERVAL)
        {
            
            alien->move(alien->getNewX(), 0);
            if(player != nullptr && abs(alien->getPosition().x - player->getPosition().x) < 200.00)
            {
                sf::Vector2f v = alien->getPosition();
                float x_adjust = (alien->sprite.getTexture()->getSize().x * alien->sprite.getScale().x)/2.0 + PROJECTILE_WIDTH / 2.0;
                float y_adjust = PROJECTILE_HEIGHT;
                std::shared_ptr<AlienProjectile> projectile = std::make_shared<AlienProjectile>(v.x + x_adjust,  v.y + y_adjust, sf::Color::Blue);
                projectiles.push_back(projectile);
            }

            lastAlienShot = std::chrono::steady_clock::now();
        }

        window.clear();

        if (!win)
        {
            window.draw(alien->sprite);
        }

        if (!gameOver)
        {
            window.draw(player->sprite);
        }

        if (fightExplosion && (start - firstExplosion).count() < ALIEN_SHOUTING_INTERVAL * 2)
        {
            explosion->setNewTexture();
            window.draw(explosion->sprite);
        }
        else
        {
            fightExplosion = false;
        }

        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i]->shape.move(0, -1.5);
            window.draw(bullets[i]->shape);

            if (bullets[i]->shape.getOrigin().y < 0)
            {
                //call destructor ?
                bullets.erase(bullets.begin() + i);
            }

            if (alien != nullptr && bullets[i]->shape.getGlobalBounds().intersects(alien->sprite.getGlobalBounds()) &&
                (start - lastAlienWound).count() > FIGHTER_SHOUTING_INTERVAL) 
            {
                alienWounds++;
                if (alienWounds ==  1)
                {
                    alien->sprite.setColor(sf::Color::Yellow);
                }
                else if (alienWounds == 2)
                {
                    alien->sprite.setColor(sf::Color::Red);
                }
                else
                {
                    fightExplosion = true;                
                    win = true;
                    explosion->setPosition(alien->getPosition().x, alien->getPosition().y);
                    alien = nullptr;    
                    firstExplosion = std::chrono::steady_clock::now();
                }
                lastAlienWound = std::chrono::steady_clock::now();
            }
        }

        for (size_t i = 0; i < projectiles.size(); i++)
        {
            projectiles[i]->shape.move(0, 1.5);
            window.draw(projectiles[i]->shape);

            if (player != nullptr && projectiles[i]->shape.getGlobalBounds().intersects(player->sprite.getGlobalBounds()) &&
                ((start - lastFighterWound).count() > ALIEN_SHOUTING_INTERVAL)) 
            {
                fighterWounds++;
                if (fighterWounds ==  1)
                {
                    player->sprite.setColor(sf::Color::Yellow);
                }
                else if (fighterWounds == 2)
                {
                    player->sprite.setColor(sf::Color::Red);
                }
                else
                {
                    fightExplosion = true;                
                    gameOver = true;
                    explosion->setPosition(player->getPosition().x, player->getPosition().y);
                    player = nullptr;    
                    firstExplosion = std::chrono::steady_clock::now();
                }

                lastFighterWound = std::chrono::steady_clock::now();
            }

            if (projectiles[i]->shape.getPosition().y > window.getSize().y)
            {   //call destructor
                projectiles.erase(projectiles.begin() + i);
            }
        }
        
        window.display();
    }

    return 0;
}