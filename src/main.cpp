#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cmath>

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    const SDL_Rect* screenRect;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};

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

    SDL_Window* window = SDL_CreateWindow("Test", 800, 600, SDL_WINDOW_RESIZABLE);
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

    SDL_Rect *screenRect = new SDL_Rect(0, 0, 800, 600);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
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

    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) 
{
    auto* app = (AppContext*)appstate;
    auto time = SDL_GetTicks() / 1000.f;
    auto red = (std::sin(time) + 1) / 2.0 * 255;
    auto green = (std::sin(time / 2) + 1) / 2.0 * 255;
    auto blue = (std::sin(time) * 2 + 1) / 2.0 * 255;

    int pitch;
    uint32_t *pixels;
    SDL_LockTexture(app->texture, app->screenRect, (void**)&pixels, &pitch);

    for (int y = 0; y < 600; ++y) {
        for (int x = 0; x < 800; ++x) {
            uint8_t a = 0xFF;
            uint8_t r = x;
            uint8_t g = y;
            uint8_t b = 0;
            uint32_t argb = (a << 24) | (r << 16) | (g << 8) | (b << 0);
            pixels[y * 800 + x] = argb;
        }
    }
    SDL_UnlockTexture(app->texture);
    
    SDL_SetRenderDrawColor(app->renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);
    SDL_FRect *fScreenRect;
    SDL_RectToFRect(app->screenRect, fScreenRect);
    SDL_RenderTexture(app->renderer, app->texture, fScreenRect, fScreenRect);
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