#include "settings.h"

#include "IniParser.h"

int		Settings::ant_population_count;
int		Settings::ant_tick_threads_count;
float	Settings::ant_speed;
float	Settings::ant_pheromone_spawn_interval;
int		Settings::ant_pheromone_detection_radius;
int		Settings::ant_pheromone_detection_deadzone_radius;
float	Settings::ant_pheromone_increase_amount;
float	Settings::ant_pheromone_tobase_decrease_amount;
float	Settings::ant_pheromone_tofood_decrease_amount;
float	Settings::ant_pheromone_follow_randomness;
float	Settings::ant_food_consumption_amount;
float	Settings::ant_boredom_percentage;
int		Settings::world_grid_width;
int		Settings::world_grid_height;
float	Settings::world_food_max_amount;


void Settings::load()
{
	IniParser parser("settings.ini");
	ant_population_count = std::stoi(parser["ant"]["population_count"]);
	ant_tick_threads_count = std::stoi(parser["ant"]["tick_threads_count"]);
	ant_speed = std::stof(parser["ant"]["speed"]);
	ant_pheromone_spawn_interval = std::stof(parser["ant"]["pheromone_spawn_interval"]);
	ant_pheromone_detection_radius = std::stoi(parser["ant"]["pheromone_detection_radius"]);
	ant_pheromone_detection_deadzone_radius = std::stoi(parser["ant"]["pheromone_detection_deadzone_radius"]);
	ant_pheromone_increase_amount = std::stof(parser["ant"]["pheromone_increase_amount"]);
	ant_pheromone_tobase_decrease_amount = std::stof(parser["ant"]["pheromone_tobase_decrease_amount"]);
	ant_pheromone_tofood_decrease_amount = std::stof(parser["ant"]["pheromone_tofood_decrease_amount"]);
	ant_pheromone_follow_randomness = std::stof(parser["ant"]["pheromone_follow_randomness"]);
	ant_food_consumption_amount = std::stof(parser["ant"]["food_consumption_amount"]);
	ant_boredom_percentage = std::stof(parser["ant"]["boredom_percentage"]);
	world_grid_width = std::stoi(parser["world"]["grid_width"]);
	world_grid_height = std::stoi(parser["world"]["grid_height"]);
	world_food_max_amount = std::stof(parser["world"]["food_max_amount"]);
}