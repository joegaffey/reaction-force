#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include "box2d/box2d.h"

static int winWidth = 800;
static int winHeight = 600;
static int running = 1;

static const int physToPixel = 20;
static const char *title = "Reaction Force";
static const int speed = 5;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *winSurface;
SDL_GameController *controller;

b2WorldId worldId;
b2BodyId groundId;
std::vector<b2BodyId> bodies;

// Startup ////////////////////////////////////////////////////////////////////////////////////////

void initControllers()
{
    int numJoy = SDL_NumJoysticks();
    std::cout << numJoy << " joysticks detected." << std::endl;

    for (int i = 0; i < numJoy; ++i)
    {
        if (SDL_IsGameController(i))
        {
            controller = SDL_GameControllerOpen(i);
            if (controller)
            {
                std::cout << "Controller connected: " << SDL_GameControllerName(controller) << std::endl;
                break;
            }
            else
            {
                std::cerr << "Could not open controller %i: " << i << SDL_GetError() << std::endl;
            }
        }
    }

    if (!controller)
        std::cout << "No controllers found." << std::endl;
}

int initDisplay()
{
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    window = SDL_CreateWindow(title,
                              (DM.w / 2) - (winWidth / 2),
                              (DM.h / 2) - (winHeight / 2),
                              winWidth,
                              winHeight,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return 0;
    }
    return 1;
}

int initPhysics() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -1.0f};
    worldId = b2CreateWorld(&worldDef);

    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -5.0f};
    groundId = b2CreateBody(worldId, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    return 1;
}

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK != 0))
    {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return 0;
    }
    else
    {
        initControllers();
        if(!initPhysics())
            return 0;
        if (!initDisplay())
            return 0;
    }
    return 1;
}

// Physics logic //////////////////////////////////////////////////////////////////////////////////

void addBody() {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){0.0f, 80.0f};
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;
    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
    
    bodies.push_back(bodyId);
}

// Event Handling /////////////////////////////////////////////////////////////////////////////////

void handleButtonPress(SDL_Event event)
{
    SDL_GameControllerButton button = (SDL_GameControllerButton)event.cbutton.button;
    switch (button)
    {
        case SDL_CONTROLLER_BUTTON_A:
            addBody();
            break;
        case SDL_CONTROLLER_BUTTON_B:
            break;
        case SDL_CONTROLLER_BUTTON_X:
            break;
        case SDL_CONTROLLER_BUTTON_Y:
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            break;
    }
}

void handleKeyInput(SDL_Event event)
{
    switch (event.key.keysym.sym)
    {
        case SDLK_UP:
            break;
        case SDLK_LEFT:
            break;
        case SDLK_RIGHT:
            break;
        case SDLK_DOWN:
            break;
        case SDLK_w:
            break;
        case SDLK_a:
            break;
        case SDLK_d:
            break;
        case SDLK_s:
            break;
        case SDLK_SPACE:
            addBody();
    }
}

void handleWindowEvent(SDL_Event event)
{
    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        winWidth = event.window.data1;
        winHeight = event.window.data2;
    }
}

// Game loop //////////////////////////////////////////////////////////////////////////////////////

SDL_Vertex triangleVertex[3] =
{
    {{0, 0}, {255, 0, 0, 255}},
    {{0, 50}, {0, 255, 0, 255}},
    {{50, 0}, {0, 0, 255, 255}}
};

void paint()
{
    SDL_SetRenderDrawColor(renderer, 5, 5, 5, 255);
    SDL_RenderClear(renderer);
    SDL_RenderGeometry(renderer, nullptr, triangleVertex, 3, nullptr, 0);
    
    for (int i = 0; i < bodies.size(); i++) {
        SDL_SetRenderDrawColor(renderer, 250, 250, 250, 128);
        b2Vec2 position = b2Body_GetPosition(bodies[i]);
        // b2Rot rotation = b2Body_GetRotation(bodyId);
        // float angle = b2Rot_GetAngle(rotation);
        SDL_Rect box = {winWidth / 2 + (int)(position.x * physToPixel / 2), (int)(winHeight - position.y * physToPixel / 2), physToPixel, physToPixel};
        SDL_RenderDrawRect(renderer, &box);
    }

    SDL_RenderPresent(renderer);
}

void checkEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        running = 0;
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        handleButtonPress(event);
        break;
    case SDL_KEYDOWN:
        handleKeyInput(event);
        break;
    case SDL_WINDOWEVENT:
        handleWindowEvent(event);
        break;
    }
}

// Main ///////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const *argv[])
{
    running = init();

    float timeStep = 1.0f / 60.0f;
    int subStepCount = 4;

    while (running)
    {
        b2World_Step(worldId, timeStep, subStepCount);
        checkEvents();
        paint();
    }

    if (controller)
        SDL_GameControllerClose(controller);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}