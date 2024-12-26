#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>


#define SCREEN_WIDTH 768
#define SCREEN_HEIGHT 432

struct Vec2 {
    float x, y;
};

struct Vec2I {
    uint32_t x, y;
};

struct AppContext {
    // SDL Context
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    const SDL_Rect* screenRect;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;

    // VARIABLES

    // Time
    float time = 0;
    float oldTime = 0;
    float frameTime = 0;
    
    // Player
    Vec2 pos = {4, 4};      // Player starting position
    Vec2 dir = {-1, 0};     // Initial direction
    float moveSpeed = 1.5f;
    float rotSpeed = 1.0f;

    // Camera
    Vec2 plane = {0, 0.66}; // 2D Raycaster Version of Camera Plane

    // Map details
    const uint8_t MAP[8][8] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 2, 0, 0, 2, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 4, 0, 2, 0, 3, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
    };
};

void verline(uint32_t* pixels, int x, int y0, int y1, uint32_t color) {
    for (int y = y0; y <= y1; y++) {
        pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}

SDL_AppResult SDL_Fail()
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_Fail();
    }

    SDL_Window* window = SDL_CreateWindow("Test", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        return SDL_Fail();
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Fail();
    }

    int width, height, bbwidth, bbheight;
    SDL_GetWindowSize(window, &width, &height);
    SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
    SDL_Log("Window size: %ix%i", width, height);
    SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);

    if (width != bbwidth) {
       SDL_Log("This is a highdpi environment.");
    }

    SDL_Rect *screenRect = new SDL_Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_Texture *texture = SDL_CreateTexture(
                        renderer, 
                        SDL_PIXELFORMAT_ARGB8888, 
                        SDL_TEXTUREACCESS_STREAMING,
                        SCREEN_WIDTH,
                        SCREEN_HEIGHT);
    if(!texture) {
        SDL_Fail();
    }

    SDL_ShowWindow(window);

    // Initialize using `new` keyword
    *appstate = new AppContext{
        window,
        renderer,
        texture,
        screenRect,
    };

    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;

}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event)
{
    auto* app = (AppContext*)appstate;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_KEY_DOWN: {
            SDL_Keycode sym = event->key.key;
            if (sym == SDLK_W) {
                SDL_Log("W Pressed");
                SDL_Log("Before Pos: {%.2f %.2f}", app->pos.x, app->pos.y);
                SDL_Log("Before Dir: {%.2f %.2f}", app->dir.x, app->dir.y);
                SDL_Log("Before Plane: {%.2f %.2f}", app->plane.x, app->plane.y);
                SDL_Log("Frame Time: %.3f", app->frameTime);
                SDL_Log("X: %d", int(app->pos.x + app->dir.x * app->moveSpeed * app->frameTime));
                SDL_Log("Y: %d", int(app->pos.y + app->dir.y * app->moveSpeed * app->frameTime));
                // if no wall in front
                if (app->MAP[int(app->pos.x + app->dir.x * app->moveSpeed * app->frameTime)][int(app->pos.y)] == false)
                    app->pos.x += app->dir.x * app->moveSpeed * app->frameTime;
                    SDL_Log("Added pos.x");
                if (app->MAP[int(app->pos.x)][int(app->pos.y + app->dir.y * app->moveSpeed * app->frameTime)] == false)
                    app->pos.y += app->dir.y * app->moveSpeed * app->frameTime;
                    SDL_Log("Added pos.y");
                SDL_Log("After Pos: {%.2f %.2f}", app->pos.x, app->pos.y);
                SDL_Log("After Dir: {%.2f %.2f}", app->dir.x, app->dir.y);
                SDL_Log("After Plane: {%.2f %.2f}", app->plane.x, app->plane.y);
            }
            else if (sym == SDLK_S) {
                // if no wall behind
                if (app->MAP[int(app->pos.x + app->dir.x * app->moveSpeed * app->frameTime)][int(app->pos.y)] == 0)
                    app->pos.x -= app->dir.x * app->moveSpeed * app->frameTime;
                if (app->MAP[int(app->pos.x)][int(app->pos.y + app->dir.y * app->moveSpeed * app->frameTime)] == 0)
                    app->pos.y -= app->dir.y * app->moveSpeed * app->frameTime;
            }
            else if (sym == SDLK_A) {
                // rotate to the left
                Vec2 oldDir = app->dir;
                app->dir.x = app->dir.x * cos(app->rotSpeed * app->frameTime) - app->dir.y * sin(app->rotSpeed * app->frameTime);
                app->dir.y = oldDir.x * sin(app->rotSpeed * app->frameTime) + app->dir.y * cos(app->rotSpeed * app->frameTime);
                Vec2 oldPlane = app->plane;
                app->plane.x = app->plane.x * cos(app->rotSpeed * app->frameTime) - app->plane.y * sin(app->rotSpeed * app->frameTime);
                app->plane.y = oldPlane.x * sin(app->rotSpeed * app->frameTime) + app->plane.y * cos(app->rotSpeed * app->frameTime);
            }
            else if (sym == SDLK_D) {
                Vec2 oldDir = app->dir;
                app->dir.x = app->dir.x * cos(-app->rotSpeed * app->frameTime) - app->dir.y * sin(-app->rotSpeed * app->frameTime);
                app->dir.y = oldDir.x * sin(-app->rotSpeed * app->frameTime) + app->dir.y * cos(-app->rotSpeed * app->frameTime);
                Vec2 oldPlane = app->plane;
                app->plane.x = app->plane.x * cos(-app->rotSpeed * app->frameTime) - app->plane.y * sin(-app->rotSpeed * app->frameTime);
                app->plane.y = oldPlane.x * sin(-app->rotSpeed * app->frameTime) + app->plane.y * cos(-app->rotSpeed * app->frameTime);
            }
            break;
        } 
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) 
{
    auto* app = (AppContext*)appstate;
    int pitch;
    uint32_t* pixels;

    app->oldTime = app->time;
    app->time = SDL_GetTicks();
    app->frameTime = (app->time - app->oldTime) / 1000.0f;

    // Update

    // Render

    // Implemented based on: https://lodev.org/cgtutor/raycasting.html
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        Vec2 camera;
        Vec2 rayDir;
        camera.x = 2 * x / (float)SCREEN_WIDTH - 1; // X-coordinate in camera space
        rayDir.x = app->dir.x + app->plane.x * camera.x;
        rayDir.y = app->dir.y + app->plane.y * camera.x;

        // Which box of the map we're in
        Vec2I map;
        map.x = (int)app->pos.x;
        map.y = (int)app->pos.y;

        Vec2 deltaDist;
        // deltaDist.x = sqrt(1 + (rayDir.y * rayDir.y) / (rayDir.x * rayDir.x));
        // deltaDist.y = sqrt(1 + (rayDir.x * rayDir.x) / (rayDir.y * rayDir.y));
        // Equations above can be simplified to:
        deltaDist.x = (rayDir.x == 0) ? 1e30 : std::abs(1 / rayDir.x);
        deltaDist.y = (rayDir.y == 0) ? 1e30 : std::abs(1 / rayDir.y);

        int hit = 0; // Was a wall hit?
        int side; // NS or EW wall hit?

        Vec2 sideDist;
        Vec2I step; // Step in X or Y direction

        // Calculate step and initial sideDist
        if (rayDir.x < 0) {
            step.x = -1;
            sideDist.x = (app->pos.x - map.x) * deltaDist.x;
        }
        else {
            step.x = 1;
            sideDist.x = (map.x + 1.0 - app->pos.x) * deltaDist.x;
        }

        if (rayDir.y < 0) {
            step.y = -1;
            sideDist.y = (app->pos.y - map.y) * deltaDist.y;
        }
        else {
            step.y = 1;
            sideDist.y = (map.y + 1.0 - app->pos.y) * deltaDist.y;
        }
        
        // DDA
        while (hit == 0) {
            if (sideDist.x < sideDist.y) {
                sideDist.x += deltaDist.x;
                map.x += step.x;
                side = false;
            }
            else {
                sideDist.y += deltaDist.y;
                map.y += step.y;
                side = 1;
            }

            if (app->MAP[map.x][map.y] > 0) hit = 1;
        }

        const float perpWallDist =
            (side == 0) ?
                (sideDist.x - deltaDist.x)
                : (sideDist.y - deltaDist.y);
        
        const int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT-1;
        
        uint32_t color;
        switch (app->MAP[map.x][map.y]) {
            case 1: color = 0xFF00FFFF; break;
            case 2: color = 0xFF00FF00; break;
            case 3: color = 0xFFFF0000; break;
            case 4: color = 0xFFFF00FF; break;
        }
        if (side == 1)
            color = color / 1.2;

        SDL_LockTexture(app->texture, app->screenRect, (void**)&pixels, &pitch);
        verline(pixels, x, 0, drawStart, 0xFF181818);
        verline(pixels, x, drawStart, drawEnd, color);
        verline(pixels, x, drawEnd, SCREEN_HEIGHT - 1, 0xFF505050);
        SDL_UnlockTexture(app->texture);
    }

    SDL_SetRenderDrawColor(app->renderer, 0x18, 0x18, 0x18, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(app->renderer); 
    SDL_RenderTexture(app->renderer, app->texture, NULL, NULL);
    SDL_RenderPresent(app->renderer);


    return app->app_quit;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    auto* app = (AppContext*)appstate;
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        // New -> Delete 
        delete app;
    }

    SDL_Log("Application quit successfully!");

}