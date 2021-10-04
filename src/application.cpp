#include "application.h"
#include "ant.h"
#include "random.h"
#include "settings.h"

#include <iostream>

Application::Application()
{
	init();
}

Application::~Application()
{
    
}

void Application::run()
{
    time_scale = DEFAULT_TIMESCALE;
    Random::init();
    ant_population = new Ant[ANT_POPULATION_COUNT];
    sf::Clock dt_clock;
    startTickingThreads();
    base_shape = sf::CircleShape(BASE_SIZE);
    base_shape.setFillColor(BASE_COLOR);
    base_shape.setOrigin(sf::Vector2f(BASE_SIZE, BASE_SIZE));
    base_shape.setPosition(0.0f, 0.0f);
    window->setView(*view);
    while (window->isOpen())
    {
        float dt = dt_clock.restart().asSeconds();
        mainLoop(dt * time_scale, dt);
    }

    terminate();
}

void Application::mainLoop(float dt, float realtime_dt) // called once per frame
{
    handleEvents();
    window->clear(sf::Color(50.0f, 50.0f, 50.0f));
    
    window->setView(*view);
    if(!block_clicks)
        spawnFood();
    drawObjects();

    window->display();
}

void Application::handleEvents()
{
    sf::Event event;
    while (window->pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            window->close();
            break;
        case sf::Event::Resized:
            resizeWindow();
            break;
        case sf::Event::MouseWheelMoved:
            zoomView(event);
            break;
        case sf::Event::MouseMoved:
            moveView(event);
            break;
        case sf::Event::MouseButtonPressed:
            changeTimescale();
            break;
        case sf::Event::MouseButtonReleased:
            block_clicks = false;
            break;
        }
    }
}

void Application::resizeWindow()
{
    float aspect_ratio = (float)window->getSize().x / (float)window->getSize().y;
    view->setSize(sf::Vector2f(view_size.x*aspect_ratio, view_size.y));
    UIview->setSize(sf::Vector2f(VIEW_WIDTH * aspect_ratio, VIEW_HEIGHT));
}

void Application::init()
{
    Settings::load();
    world = new World(WORLD_GRID_WIDTH, WORLD_GRID_HEIGHT);
    view_size = sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT);
    window = new sf::RenderWindow(sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), WINDOW_NAME);
    float aspect_ratio = (float)window->getSize().x / (float)window->getSize().y;
    view = new sf::View(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(view_size.x * aspect_ratio, view_size.y));
    UIview = new sf::View(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(view_size.x * aspect_ratio, view_size.y));
    font.loadFromFile(FONTS_PATH("Pixeboy.ttf"));
    Ant::init();

    for (int y = -WORLD_FOOD_MOUSE_RADIUS; y < WORLD_FOOD_MOUSE_RADIUS; ++y)
    {
        for (int x = -WORLD_FOOD_MOUSE_RADIUS; x < WORLD_FOOD_MOUSE_RADIUS; ++x)
        {
            float dist2 = x * x + y * y;
            if (dist2 < WORLD_FOOD_MOUSE_RADIUS * WORLD_FOOD_MOUSE_RADIUS)
            {
                food_spawn_offsets.push_back(sf::Vector2i(x, y));
            }
        }
    }
}

void Application::drawObjects()
{
    world->draw(window);
    Ant::drawAll(window);
    window->draw(base_shape);
    drawUI();
}

void Application::terminate()
{
    for (std::thread& t : ant_tick_threads)
    {
        t.join();
    }
    delete[] ant_population;
    delete view;
    delete UIview;
    delete window;
}

void Application::startTickingThreads()
{
    for (int i = 0; i < ANT_TICK_THREADS_COUNT; ++i)
    {
        ant_tick_threads.push_back(std::thread(&Application::threaded_tickAnts, this, i));
    }
}

void Application::threaded_tickAnts(int section_index)
{
    float time_to_drop = ANT_PHEROMONE_SPAWN_INTERVAL;
    sf::Clock dt_clock;
    while (window->isOpen())
    {   
        float dt = dt_clock.restart().asSeconds() * time_scale;
        time_to_drop -= dt;
        bool drop_pheromones = time_to_drop < 0.0f;
        if (drop_pheromones)
            time_to_drop = ANT_PHEROMONE_SPAWN_INTERVAL;
        Ant::tickPortion(ANT_TICK_THREADS_COUNT, section_index, dt, drop_pheromones, world);
    }
}

static bool cellPositionInWorldBounds(const sf::Vector2i pos, World* world)
{
    // copy paste from ant.cpp, should be in the world class but it works
    if (pos.x <= world->minX() ||
        pos.x >= world->maxX() ||
        pos.y <= world->minX() ||
        pos.y >= world->maxY())
        return false;
    return true;
}

void Application::spawnFood()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
        sf::Vector2f world_pos = window->mapPixelToCoords(mouse_pos);
        sf::Vector2i grid_pos(std::floorf(world_pos.x / WORLD_GRID_CELL_SIZE), std::floorf(world_pos.y / WORLD_GRID_CELL_SIZE));

        for (const sf::Vector2i& offset : food_spawn_offsets)
        {
            sf::Vector2i pos = grid_pos + offset;
            if (cellPositionInWorldBounds(pos, world))
            {
                std::unique_lock<std::mutex> lock((*world)[pos.x][pos.y].mutex);
                (*world)[pos.x][pos.y].setFood(WORLD_FOOD_MAX_AMOUNT);
            }
        }
    }
}

void Application::zoomView(const sf::Event& ev)
{
    view_size.x += -ev.mouseWheel.delta * VIEW_WIDTH * ZOOM_PER_SCROLL;
    view_size.x = std::max(ZOOM_PER_SCROLL * VIEW_WIDTH, view_size.x);

    view_size.y += -ev.mouseWheel.delta * VIEW_WIDTH * ZOOM_PER_SCROLL;
    view_size.y = std::max(ZOOM_PER_SCROLL * VIEW_WIDTH, view_size.y);

    resizeWindow();
}

void Application::moveView(const sf::Event& ev)
{
    static sf::Vector2f prev;
    static bool wasPressed = false;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        if (wasPressed)
        {
            sf::Vector2f delta = (sf::Vector2f(ev.mouseMove.x, ev.mouseMove.y) - prev) * (view_size.x / VIEW_WIDTH);
            view->setCenter(sf::Vector2f(view->getCenter().x - delta.x, view->getCenter().y - delta.y));
            window->setView(*view);
        }
        else
        {
            wasPressed = true;
        }
            prev = sf::Vector2f(ev.mouseMove.x, ev.mouseMove.y);
    }
    else
        wasPressed = false;
}

void Application::drawUI()
{
    window->setView(*UIview);

    sf::Text population_text("population: ", font);
    sf::Text population_amount(std::to_string(Ant::full_list.size()), font);
    population_amount.setFillColor(UI_VALUE_COLOR);
    population_text.setPosition(-UIview->getSize().x / 2.0f + 10.0f, -UIview->getSize().y / 2.0f);
    population_amount.setPosition(-UIview->getSize().x / 2.0f + 10.0f + population_text.getLocalBounds().width, -UIview->getSize().y / 2.0f);

    sf::Text timescale_text("time scale: ", font);
    sf::Text timescale_amount(std::to_string(time_scale), font);
    timescale_text.setPosition(-UIview->getSize().x/ 2.0f + 10.0f, -UIview->getSize().y / 2.0f + population_text.getLocalBounds().height + 10.0f);
    timescale_amount.setPosition(-UIview->getSize().x / 2.0f + 10.0f, -UIview->getSize().y / 2.0f + population_text.getLocalBounds().height + timescale_text.getLocalBounds().height + 15.0f);
    timescale_amount.setFillColor(UI_VALUE_COLOR);
    
    slow_down_text = sf::Text("<<", font);
    slow_down_text.setPosition(-UIview->getSize().x / 2.0f + 10.0f, -UIview->getSize().y / 2.0f + timescale_text.getLocalBounds().height + population_text.getLocalBounds().height + timescale_amount.getLocalBounds().height + 20.0f);
    speed_up_text = sf::Text(">>", font);
    speed_up_text.setPosition(-UIview->getSize().x / 2.0f + 10.0f + timescale_amount.getLocalBounds().width - speed_up_text.getLocalBounds().width, -UIview->getSize().y / 2.0f + timescale_text.getLocalBounds().height + timescale_amount.getLocalBounds().height + population_text.getLocalBounds().height + 20.0f);

    window->draw(slow_down_text);
    window->draw(speed_up_text);
    window->draw(timescale_text);
    window->draw(timescale_amount);
    window->draw(population_text);
    window->draw(population_amount);
}

static bool aabb(const sf::Vector2f& vec, const sf::FloatRect& rect, float padding)
{
    bool x_col = vec.x > rect.left - padding && vec.x < rect.left + rect.width + padding;
    bool y_col = vec.y > rect.top - padding && vec.y < rect.top + rect.height + padding;
    return y_col && x_col;
}

void Application::changeTimescale()
{
    window->setView(*UIview);
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(*window);
    sf::Vector2f world_pos = window->mapPixelToCoords(mouse_pos);

    if (aabb(world_pos, speed_up_text.getGlobalBounds(), 10.0f))
    {
        block_clicks = true;
        time_scale = std::min(time_scale*2, 64.0f);
    }
    else if (aabb(world_pos, slow_down_text.getGlobalBounds(), 10.0f))
    {
        block_clicks = true;
        time_scale = std::max(time_scale / 2, 0.125f);
    }
    //if()
}