// GLEW
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <sstream>

#include "GameApp.h"

void exitProgram()
{
    SDL_Quit();

    std::cout << "Press a button to exit..." << std::endl;
    std::cin.get();
}

int main( int argc, char* args[] )
{
    atexit( exitProgram );

    // Initialize SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        std::cout << "[ SDL init error ] " << SDL_GetError() << std::endl;
        return 1;
    }
            
    // Create OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Enable double buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // Depth depth buffer bit length
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1); // Antialiasing buffers
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2); // Antialiasing samples

    // Create window
    SDL_Window *win = 0;
    win = SDL_CreateWindow("Lego Racer", // Title
                            100, // X
                            100, // Y
                            640, // W
                            480, // H
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE // Options
    );

    // Error if window could not be created
    if (win == 0)
    {
        std::cout << "[ Window creation error ] " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create OpenGL context
    SDL_GLContext context = SDL_GL_CreateContext(win);
    if (context == 0)
    {
        std::cout << "[ OGL init error ] " << SDL_GetError() << std::endl;
        return 1;
    }    
    SDL_GL_SetSwapInterval(1); // Endable wait for vsync

    // Initialize GLEW
    GLenum error = glewInit();
    if ( error != GLEW_OK )
    {
        std::cout << "[ GLEW init error ] " << std::endl;
        return 1;
    }

    // Verify OpenGL version
    int glVersion[2] = {-1, -1}; 
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); 
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); 
    std::cout << "Running OpenGL " << glVersion[0] << "." << glVersion[1] << std::endl;

    if ( glVersion[0] == -1 && glVersion[1] == -1 )
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow( win );

        std::cout << "[ OGL context init error ] " << std::endl;

        return 1;
    }

	// Set window title to the OGL version in debug
	#if _DEBUG
    std::stringstream window_title;
    window_title << "OpenGL " << glVersion[0] << "." << glVersion[1];
    SDL_SetWindowTitle(win, window_title.str().c_str());
	#endif

    // Create game app
    bool quit = false;
    SDL_Event ev;
    GameApp app;

	// Check app initialization
    if (!app.Init())
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(win);
        std::cout << "[ App init error ] " << std::endl;
        return 1;
    }

	// Begin main game loop
    while (!quit)
    {
        while ( SDL_PollEvent(&ev) )
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if ( ev.key.keysym.sym == SDLK_ESCAPE )
                    quit = true;
                app.KeyboardDown(ev.key);
                break;
            case SDL_KEYUP:
                app.KeyboardUp(ev.key);
                break;
            case SDL_MOUSEBUTTONDOWN:
                app.MouseDown(ev.button);
                break;
            case SDL_MOUSEBUTTONUP:
                app.MouseUp(ev.button);
                break;
            case SDL_MOUSEWHEEL:
                app.MouseWheel(ev.wheel);
                break;
            case SDL_MOUSEMOTION:
                app.MouseMove(ev.motion);
                break;
            case SDL_WINDOWEVENT:
                if ( ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
                {
                    app.Resize(ev.window.data1, ev.window.data2);
                }
                break;
            }
        }

        app.Update();
        app.Render();

        SDL_GL_SwapWindow(win);
    }

    // Cleanup after exit
    app.Clean();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow( win );

    return 0;
}