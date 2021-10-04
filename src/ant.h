#pragma once

#include "SFML/Graphics.hpp"
#include <vector>
#include "world.h"

class Ant
{
public:
    static std::vector<Ant*> full_list;
    static void init();
    static void tickPortion(unsigned int portion_count, unsigned int portion_index, float dt, bool drop_pheromones, World* world);
    static void drawAll(sf::RenderWindow* window);

    Ant();
    ~Ant();
    void tick(float dt, bool drop_pheromones, World* world);
    sf::Vector2f getNormalizedDirection();

private:
    static sf::Texture* ant_texture;
    static std::vector<sf::Vertex> ant_vertex_array;
    static std::vector<sf::Vector2f> single_ant_vertex_positions;
    static std::vector<sf::Vector2i> pheromone_look_offsets;
    static std::vector<sf::Vector2i> pheromone_decrease_offsets;
    static std::vector<sf::Vector2i> food_look_offsets;
    static std::vector<sf::Vector2i> food_collision_offsets;

    float speed;
    float time_to_random_rotation = 0.0f;
    float time_to_pheromone_spawn;
    bool carrying_food = true;
    sf::Vector2f previous_position;
    unsigned int index;

    void randomRotate();
    void walk(float dt, const sf::Vector2f& normalized_direction, World* world);
    void dropFoodAtBase();
    bool lookForFood(World* world);
    bool lookForPheromones(const sf::Vector2f& normalized_direction, World* world);
    bool lookForHome();
    void dropPheromones(World* world);
    void decreasePheromones(World* world);
    bool bored();


    // === rendering ===

    float rotation = 0.0f;
    sf::Vector2f position;
    sf::Transform transform;

    void draw();
    void setPosition(const sf::Vector2f& vec);
    sf::Vector2f getPosition();
    sf::Vector2i getWorldCellPosition();
    void setRotationDegrees(float deg);
    float getRotationDegrees(); // positive = clockwise
    void rotateDegrees(float deg);
    void move(const sf::Vector2f& vec);
};

