#include "ant.h"
#include "application.h"
#include "random.h"

#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#define DEG_TO_RAD 0.0174533f
#define RAD_TO_DEG 57.2958f

static std::ostream& operator<<(std::ostream& stream, const sf::Vector2f& vec)
{
	stream << '(' << vec.x << ", " << vec.y << ')';
	return stream;
}

static sf::Vector2f normalize(const sf::Vector2f& vec)
{
	float norm = std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
	return vec / norm;
}

static float dot(const sf::Vector2f& vec1, const sf::Vector2f& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

static float clampDegAngle(float a)
{
	while (a < 0.0f)
		a += 360.0f;
	while (a > 360.0f)
		a -= 360.0f;
	return a;
}

sf::Texture* Ant::ant_texture = new sf::Texture();
std::vector<sf::Vertex> Ant::ant_vertex_array;
std::vector<sf::Vector2f> Ant::single_ant_vertex_positions;
std::vector<sf::Vector2i> Ant::pheromone_look_offsets;
std::vector<sf::Vector2i> Ant::pheromone_decrease_offsets;
std::vector<sf::Vector2i> Ant::food_look_offsets;
std::vector<sf::Vector2i> Ant::food_collision_offsets;
std::vector<Ant*> Ant::full_list;

void Ant::init()
{
	ant_texture->loadFromFile(TEXTURE_PATH("ant.png"));
	single_ant_vertex_positions.push_back(sf::Vector2f(-ANT_SIZE, -ANT_SIZE));
	single_ant_vertex_positions.push_back(sf::Vector2f(ANT_SIZE, -ANT_SIZE));
	single_ant_vertex_positions.push_back(sf::Vector2f(ANT_SIZE, ANT_SIZE));
	single_ant_vertex_positions.push_back(sf::Vector2f(-ANT_SIZE, ANT_SIZE));

	for (int y = -ANT_PHEROMONE_DETECTION_RADIUS; y < ANT_PHEROMONE_DETECTION_RADIUS; ++y)
	{
		for (int x = -ANT_PHEROMONE_DETECTION_RADIUS; x < ANT_PHEROMONE_DETECTION_RADIUS; ++x)
		{
			float dist2 = x * x + y * y;
			if (dist2 < ANT_PHEROMONE_DETECTION_RADIUS * ANT_PHEROMONE_DETECTION_RADIUS &&
				dist2 > ANT_PHEROMONE_DETECTION_DEADZONE_RADIUS * ANT_PHEROMONE_DETECTION_DEADZONE_RADIUS)
			{
				pheromone_look_offsets.push_back(sf::Vector2i(x, y));
			}
		}
	}

	for (int y = -ANT_PHEROMONE_DECREASE_RADIUS; y < ANT_PHEROMONE_DECREASE_RADIUS; ++y)
	{
		for (int x = -ANT_PHEROMONE_DECREASE_RADIUS; x < ANT_PHEROMONE_DECREASE_RADIUS; ++x)
		{
			float dist2 = x * x + y * y;
			if (dist2 < ANT_PHEROMONE_DECREASE_RADIUS * ANT_PHEROMONE_DECREASE_RADIUS)
			{
				pheromone_decrease_offsets.push_back(sf::Vector2i(x, y));
			}
		}
	}

	for (int y = -ANT_FOOD_DETECTION_RADIUS; y < ANT_FOOD_DETECTION_RADIUS; ++y)
	{
		for (int x = -ANT_FOOD_DETECTION_RADIUS; x < ANT_FOOD_DETECTION_RADIUS; ++x)
		{
			float dist2 = x * x + y * y;
			if (dist2 < ANT_FOOD_DETECTION_RADIUS * ANT_FOOD_DETECTION_RADIUS)
			{
				food_look_offsets.push_back(sf::Vector2i(x, y));
			}
		}
	}

	for (int y = -ANT_FOOD_COLLISION_RADIUS; y < ANT_FOOD_COLLISION_RADIUS; ++y)
	{
		for (int x = -ANT_FOOD_COLLISION_RADIUS; x < ANT_FOOD_COLLISION_RADIUS; ++x)
		{
			float dist2 = x * x + y * y;
			if (dist2 < ANT_FOOD_COLLISION_RADIUS * ANT_FOOD_COLLISION_RADIUS)
			{
				food_collision_offsets.push_back(sf::Vector2i(x, y));
			}
		}
	}
}

void Ant::tickPortion(unsigned int portion_count, unsigned int portion_index, float dt, bool drop_pheromones, World* world)
{
	// credit https://stackoverflow.com/a/6861195/14678287
	int n_el = full_list.size();
	int size = n_el / portion_count;
	int mod = n_el % portion_count;

	int iterating_portion_index = 0;
	int iterating_item_index = 0;
	for (int i = 0; i < mod; ++i)
	{
		int portion_size = size + 1;

		if (iterating_portion_index == portion_index)
		{
			for (int i = 0; i < portion_size; ++i)
			{
				int index = i + iterating_item_index;
				full_list[index]->tick(dt, drop_pheromones, world);
			}
		}

		iterating_item_index += portion_size;
		++iterating_portion_index;
	}
	for (int i = 0; i < portion_count - mod; ++i)
	{
		int portion_size = size;

		if (iterating_portion_index == portion_index)
		{
			for (int i = 0; i < portion_size; ++i)
			{
				int index = i + iterating_item_index;
				full_list[index]->tick(dt, drop_pheromones, world);
			}
		}

		iterating_item_index += portion_size;
		++iterating_portion_index;
	}
}

void Ant::drawAll(sf::RenderWindow* window)
{
	for (Ant* a : full_list)
	{
		a->draw();
	}

	window->draw(&ant_vertex_array[0], ant_vertex_array.size(), sf::Quads, ant_texture);
}

Ant::Ant()
	: speed{ANT_SPEED}, time_to_pheromone_spawn{ANT_PHEROMONE_SPAWN_INTERVAL}, previous_position(0.0f, 0.0f), position(0.0f, 0.0f)
{
	index = full_list.size();
	full_list.push_back(this);

	sf::Vertex top_left, top_right, bottom_right, bottom_left;
	sf::Vector2f texture_size = sf::Vector2f(ant_texture->getSize().x, ant_texture->getSize().y);
	top_left.texCoords = sf::Vector2f(0.0f, 0.0f);
	top_right.texCoords = sf::Vector2f(texture_size.x, 0.0f);
	bottom_right.texCoords = sf::Vector2f(texture_size.x, texture_size.y);
	bottom_left.texCoords = sf::Vector2f(0.0f, texture_size.y);
	top_left.color = ANT_COLOR;
	top_right.color = ANT_COLOR;
	bottom_right.color = ANT_COLOR;
	bottom_left.color = ANT_COLOR;
	ant_vertex_array.push_back(top_left);
	ant_vertex_array.push_back(top_right);
	ant_vertex_array.push_back(bottom_right);
	ant_vertex_array.push_back(bottom_left);

	rotateDegrees(Random::rangef(-180.0f, 180.0f));
	time_to_random_rotation = Random::rangef(ANT_RANDOM_ROTATION_TIME_MIN, ANT_RANDOM_ROTATION_TIME_MAX);
}

Ant::~Ant()
{
	full_list.erase(std::remove(full_list.begin(), full_list.end(), this), full_list.end());
}

//void Ant::draw(sf::RenderWindow* window)
//{
//	window->draw(rectangle_shape);
//
//	if (carrying_food) // Food carry item
//	{
//		sf::CircleShape food_circle_shape(ANT_SIZE/5.0f);
//		float rad_angle = getRotationDegrees() * DEG_TO_RAD; // not negative angle although positive is clockwise: y axis is reversed
//		float norm = sqrtf((std::cos(rad_angle) * std::cos(rad_angle)) + (std::sin(rad_angle) * std::sin(rad_angle)));
//		sf::Vector2f normalized_direction = sf::Vector2f(std::cos(rad_angle), std::sin(rad_angle)) / norm;
//		food_circle_shape.setPosition(rectangle_shape.getPosition() - sf::Vector2f(ANT_SIZE / 5.0f, ANT_SIZE / 5.0f) + (normalized_direction * (ANT_SIZE / 1.75f)));
//		food_circle_shape.setFillColor(WORLD_FOOD_COLOR);
//		window->draw(food_circle_shape);
//	}
//}

void Ant::tick(float dt, bool drop_pheromones, World* world)
{
	dropFoodAtBase();

	time_to_random_rotation -= dt;
	if (time_to_random_rotation < 0.0f)
	{
		if (!lookForFood(world) && !lookForHome())
		{
			if (bored())
			{
				randomRotate();
			}
			else if (!lookForPheromones(getNormalizedDirection(), world))
				randomRotate();
		}
		time_to_random_rotation = Random::rangef(ANT_RANDOM_ROTATION_TIME_MIN, ANT_RANDOM_ROTATION_TIME_MAX);
	}

	if (drop_pheromones)
		dropPheromones(world);

	walk(dt, getNormalizedDirection(), world);
	decreasePheromones(world);
	previous_position = getPosition();
}

sf::Vector2f Ant::getNormalizedDirection()
{
	float rad_angle = getRotationDegrees() * DEG_TO_RAD; // not negative angle although positive is clockwise: y axis is reversed
	float norm = sqrtf((std::cos(rad_angle) * std::cos(rad_angle)) + (std::sin(rad_angle) * std::sin(rad_angle)));
	return sf::Vector2f(std::cos(rad_angle), std::sin(rad_angle)) / norm;
}

void Ant::randomRotate()
{
	sf::Vector2f pos = getPosition();
	sf::Vector2f pos_n = normalize(pos);
	sf::Vector2f max = sf::Vector2f(WORLD_GRID_CELL_SIZE * WORLD_GRID_WIDTH, WORLD_GRID_CELL_SIZE * WORLD_GRID_HEIGHT);
	float dist2 = pos.x * pos.x + pos.y * pos.y;
	float max_dist2 = max.x * max.x + max.y * max.y;
	float pull_center = dist2 / max_dist2;
	float to_center_angle = clampDegAngle(std::atan2(-pos_n.y, -pos_n.x)* RAD_TO_DEG - getRotationDegrees());
	if (Random::scalef() < (pull_center - ANT_RANDOM_PULL_CENTER_FREEDOM))
	{
		rotateDegrees(to_center_angle);
	}
	else
	{
		rotateDegrees(Random::rangef(-ANT_RANDOM_ROTATION_ANGLE_MAX, ANT_RANDOM_ROTATION_ANGLE_MAX));
	}
}

static bool positionInWorldBounds(const sf::Vector2f& pos, World* world)
{
	int world_x = std::floorf(pos.x / WORLD_GRID_CELL_SIZE);
	int world_y = std::floorf(pos.y / WORLD_GRID_CELL_SIZE);
	if (world_x <= world->minX() ||
		world_x >= world->maxX() ||
		world_y <= world->minX() ||
		world_y >= world->maxY())
		return false;
	return true;
}

static bool cellPositionInWorldBounds(const sf::Vector2i pos, World* world)
{
	if (pos.x <= world->minX() ||
		pos.x >= world->maxX() ||
		pos.y <= world->minX() ||
		pos.y >= world->maxY())
		return false;
	return true;
}

void Ant::walk(float dt, const sf::Vector2f& normalized_direction, World* world)
{
	sf::Vector2f movement = normalized_direction * dt * speed;
	while (true)
	{
		if (positionInWorldBounds(getPosition() + movement, world))
			break;
		randomRotate();
		movement = getNormalizedDirection() * dt * speed;
	}
	move(movement);
}

void Ant::dropFoodAtBase()
{
	sf::Vector2f pos = getPosition();
	if (carrying_food && ((pos.x*pos.x + pos.y*pos.y) < (BASE_SIZE * BASE_SIZE)))
	{
		carrying_food = false;
		rotateDegrees(180.0f);
	}
}

bool Ant::lookForFood(World* world)
{
	if (carrying_food) return false;

	{
		float max_food = 0.0f;
		bool found = false;
		World::PheromoneCell* max_food_cell = nullptr;
		// COLLIDE WITH FOOD
		for (const sf::Vector2i& offset : food_collision_offsets)
		{
			sf::Vector2i pos = getWorldCellPosition() + offset;
			if (cellPositionInWorldBounds(pos, world))
			{
				std::unique_lock<std::mutex> lock((*world)[pos.x][pos.y].mutex);
				float food = (*world)[pos.x][pos.y].getFood();
				if (food > max_food)
				{
					found = true;
					max_food = food;
					max_food_cell = &((*world)[pos.x][pos.y]);
				}
			}
		}
		if (found)
		{
			sf::Vector2f pos(getPosition());
			setRotationDegrees(std::atan2f(-pos.y, -pos.x) * RAD_TO_DEG);
			std::unique_lock<std::mutex> lock(max_food_cell->mutex);
			max_food_cell->decreaseFood(ANT_FOOD_CONSUMPTION_AMOUNT);
			carrying_food = true;
			return true;
		}
	}

	// if didn't collide with food
	float max_food = 0.0f;
	bool found = false;
	sf::Vector2f target;

	for (const sf::Vector2i& offset : food_look_offsets)
	{
		sf::Vector2i pos = getWorldCellPosition() + offset;
		if (cellPositionInWorldBounds(pos, world))
		{
			std::unique_lock<std::mutex> lock((*world)[pos.x][pos.y].mutex);
			float food = (*world)[pos.x][pos.y].getFood();
			if (food > max_food)
			{
				found = true;
				target = sf::Vector2f(pos.x, pos.y) * WORLD_GRID_CELL_SIZE + sf::Vector2f(WORLD_GRID_CELL_SIZE/2.0f, WORLD_GRID_CELL_SIZE/2.0f);
				max_food = food;
			}
		}
	}

	if (found)
	{
		sf::Vector2f to_vec = target - getPosition();
		float to_angle_deg = std::atan2f(to_vec.y, to_vec.x) * RAD_TO_DEG;
		setRotationDegrees(to_angle_deg);
	}

	return found;
}

void Ant::draw()
{
	sf::Transform transform;
	transform.translate(getPosition());
	transform.rotate(rotation, sf::Vector2f(0.0f, 0.0f));

	for (int i = 0; i < 4; ++i)
	{
		ant_vertex_array[(unsigned long long)index*4 + i].position = transform.transformPoint(single_ant_vertex_positions[i]);
	}

	if (carrying_food)
	{
		ant_vertex_array[index * 4 + 1].color = WORLD_FOOD_COLOR;
		ant_vertex_array[index * 4 + 2].color = WORLD_FOOD_COLOR;
	}
	else
	{
		ant_vertex_array[index * 4 + 1].color = ANT_COLOR;
		ant_vertex_array[index * 4 + 2].color = ANT_COLOR;
	}
}

bool Ant::lookForPheromones(const sf::Vector2f& normalized_direction, World* world)
{
	bool found = false;
	float target_amount = 0.0f;
	sf::Vector2f target(0.0f, 0.0f);
	
	for (sf::Vector2i& offset : pheromone_look_offsets)
	{
		sf::Vector2i pos = getWorldCellPosition() + offset;
		if (cellPositionInWorldBounds(pos, world))
		{
			if (carrying_food)
			{
				std::unique_lock<std::mutex> lock((*world)[pos.x][pos.y].mutex);
				float amount = (*world)[pos.x][pos.y].getToBase();
				sf::Vector2f this_target = sf::Vector2f(pos.x * WORLD_GRID_CELL_SIZE + WORLD_GRID_CELL_SIZE / 2.0f, pos.y * WORLD_GRID_CELL_SIZE + WORLD_GRID_CELL_SIZE / 2.0f);
				sf::Vector2f to_this_target = normalize(this_target - getPosition());
				if (amount > target_amount && dot(normalized_direction, to_this_target) > 0.0f)
				{
					(*world)[pos.x][pos.y].decreaseToBase(ANT_PHEROMONE_TOBASE_DECREASE_AMOUNT);
					found = true;
					target = this_target;
					target_amount = amount;
				}
			}
			else
			{
				std::unique_lock<std::mutex> lock((*world)[pos.x][pos.y].mutex);
				float amount = (*world)[pos.x][pos.y].getToFood();
				sf::Vector2f this_target = sf::Vector2f(pos.x * WORLD_GRID_CELL_SIZE + WORLD_GRID_CELL_SIZE / 2.0f, pos.y * WORLD_GRID_CELL_SIZE + WORLD_GRID_CELL_SIZE / 2.0f);
				sf::Vector2f to_this_target = normalize(this_target - getPosition());
				if (amount > target_amount && dot(normalized_direction, to_this_target) > 0.0f)
				{
					(*world)[pos.x][pos.y].decreaseToFood(ANT_PHEROMONE_TOFOOD_DECREASE_AMOUNT);
					found = true;
					target = this_target;
					target_amount = amount;
				}
			}
		}
	}
	if (found)
	{
		target += sf::Vector2f(Random::rangef(-WORLD_GRID_CELL_SIZE* ANT_PHEROMONE_FOLLOW_RANDOMNESS, WORLD_GRID_CELL_SIZE* ANT_PHEROMONE_FOLLOW_RANDOMNESS), Random::rangef(-WORLD_GRID_CELL_SIZE* ANT_PHEROMONE_FOLLOW_RANDOMNESS, WORLD_GRID_CELL_SIZE* ANT_PHEROMONE_FOLLOW_RANDOMNESS));
		sf::Vector2f to_target = target - getPosition();
		float to_angle_deg = std::atan2f(to_target.y, to_target.x) * RAD_TO_DEG;
		setRotationDegrees(to_angle_deg);
	}
	return found;
}

bool Ant::lookForHome()
{
	if (!carrying_food)
		return false;
	sf::Vector2f position = getPosition();
	float dist2 = position.x * position.x + position.y * position.y;
	if (dist2 - BASE_SIZE * BASE_SIZE < ANT_HOME_DETECTION_RADIUS* ANT_HOME_DETECTION_RADIUS)
	{
		sf::Vector2f to_target = -position;
		float to_angle_deg = std::atan2f(to_target.y, to_target.x) * RAD_TO_DEG;
		setRotationDegrees(to_angle_deg);
		return true;
	}
	return false;
}

void Ant::dropPheromones(World* world)
{
	sf::Vector2i world_pos = getWorldCellPosition();
	//if (!cellPositionInWorldBounds(world_pos, world)) return;
	std::unique_lock<std::mutex> lock((*world)[world_pos.x][world_pos.y].mutex);
	if(carrying_food)
		(*world)[world_pos.x][world_pos.y].increaseToFood(ANT_PHEROMONE_INCREASE_AMOUNT);
	else
		(*world)[world_pos.x][world_pos.y].increaseToBase(ANT_PHEROMONE_INCREASE_AMOUNT);
}

void Ant::decreasePheromones(World* world)
{
	sf::Vector2i pos = getWorldCellPosition();
	
	sf::Vector2f prev_pos_f = (1.0f / WORLD_GRID_CELL_SIZE) * previous_position;
	sf::Vector2i prev_pos_i = sf::Vector2i(std::floor(prev_pos_f.x), std::floor(prev_pos_f.y));

	if (prev_pos_i != pos)
	{
		sf::Vector2i my_world_pos = getWorldCellPosition();
		for (const sf::Vector2i& offset : pheromone_decrease_offsets)
		{
			sf::Vector2i world_pos = my_world_pos + offset;

			if (!cellPositionInWorldBounds(world_pos, world)) continue;
			std::unique_lock<std::mutex> lock((*world)[world_pos.x][world_pos.y].mutex);
			(*world)[world_pos.x][world_pos.y].decreaseToBase(ANT_PHEROMONE_TOBASE_DECREASE_AMOUNT);
			(*world)[world_pos.x][world_pos.y].decreaseToFood(ANT_PHEROMONE_TOFOOD_DECREASE_AMOUNT);
		}
		
	}
}

bool Ant::bored()
{
	return Random::scalef() < ANT_BOREDOM_PERCENTAGE;
}

void Ant::setPosition(const sf::Vector2f& vec)
{
	position = vec;
}

sf::Vector2f Ant::getPosition()
{
	return position;
}

sf::Vector2i Ant::getWorldCellPosition()
{
	sf::Vector2f pos_f = (1.0f / WORLD_GRID_CELL_SIZE) * getPosition();
	return sf::Vector2i(std::floor(pos_f.x), std::floor(pos_f.y));
}

void Ant::setRotationDegrees(float deg)
{
	rotation = deg;
}

float Ant::getRotationDegrees()
{
	return rotation;
}

void Ant::rotateDegrees(float deg)
{
	rotation += deg;
}

void Ant::move(const sf::Vector2f& vec)
{
	position += vec;
}