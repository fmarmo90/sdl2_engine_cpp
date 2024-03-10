#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_ttf/SDL_ttf.h>

const int WIDTH = 800, 
          HEIGHT = 600,
          targetFPS = 60,
          frameDelay = 1000 / targetFPS;

int init() {
    return SDL_Init(SDL_INIT_VIDEO) || TTF_Init();
}

TTF_Font* loadFont(const char* fontFile, int fontSize) {
    TTF_Font* font = TTF_OpenFont(fontFile, fontSize);

    if (font == NULL) {
        std::cout << "Error loading font" << TTF_GetError() << std::endl;
    }

    return font;
}

SDL_Texture* renderText(const char* text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);

    if (surface == NULL) {
        std::cout << "Error rendering text surface" << TTF_GetError() << std::endl;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL) {
        std::cout << "Error creating text texture" << TTF_GetError() << std::endl;
    }

    SDL_FreeSurface(surface);

    return texture;
}

SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        std::cout << "Render creation failed" << TTF_GetError() << std::endl;
    }

    return renderer;
}

void renderFpsOnScreen(int frameTime, int* frameCount, TTF_Font* font, SDL_Renderer* renderer) {
    int fps = *frameCount * 1000 / frameTime;

    char fpsText[16];

    snprintf(fpsText, sizeof(fpsText), "FPS: %d", fps);

    SDL_Color color = {255, 255, 255, 255};
    SDL_Texture* fpsTexture = renderText(fpsText, font, color, renderer);

    if (fpsTexture != NULL) {
        SDL_Rect dstRect = {10, 10, 0, 0};

        SDL_QueryTexture(fpsTexture, NULL, NULL, &dstRect.w, &dstRect.h);
        SDL_RenderCopy(renderer, fpsTexture, NULL, &dstRect);
        SDL_DestroyTexture(fpsTexture);
    }
    
    *frameCount = 0;
}

void cleanUp(TTF_Font* font, SDL_Renderer* renderer, SDL_Window* window) {
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

SDL_Window* createMainWindow() {
    SDL_Window *window = SDL_CreateWindow(
        "Hello SDL World", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        WIDTH, 
        HEIGHT,
        SDL_WINDOW_ALLOW_HIGHDPI
    );

    return window;
}

void keepFpsFixed(int* frameTime, Uint32* frameStart) {
    *frameTime = SDL_GetTicks() - *frameStart;

    if (*frameTime < frameDelay) {
        SDL_Delay(frameDelay - *frameTime);

        *frameTime = frameDelay;
    }
}

int main(int argc, char *argv[]) {
    Uint32 frameStart;
    int frameTime;
    int frameCount = 0;

    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    SDL_Window* mainWindow = nullptr;
    SDL_Event event;

    if (init() != 0) {
        std::cout << "Could not initialzie SDL2 library" << SDL_GetError() << std::endl;
        return 1;
    }

    mainWindow = createMainWindow();

    if (NULL == mainWindow) {
        std::cout << "Could not create window" <<   SDL_GetError() << std::endl;
        return 1;
    }

    renderer = createRenderer(mainWindow);
    font = loadFont("arial.ttf", 24);

    bool gameIsRunning = true;

    while(gameIsRunning) {
        frameStart = SDL_GetTicks();
        
        bool eventHandled = false;

        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    gameIsRunning = false;
                    eventHandled = true;
                break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            gameIsRunning = false;
                            eventHandled = true;
                        break;
                    }
                break;
            }

            // If no events were handled, delay briefly to reduce CPU usage
            if (!eventHandled) {
                SDL_Delay(10); // Delay for 10 milliseconds
            }
        }
        
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        //Game logic delay
        SDL_Delay(10);

        // Keep 60 fps as maximum
        keepFpsFixed(&frameTime, &frameStart);

        frameCount++;

        renderFpsOnScreen(frameTime, &frameCount, font, renderer);

        // Present the renderer
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    cleanUp(font, renderer, mainWindow);

    return EXIT_SUCCESS;
}
