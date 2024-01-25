#include "application.h"
#include "utils.h"
#include "image.h"

#include <iostream>
#include <list>

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);

	framebuffer.resize(w, h);
}

// Instàncies de image
Image foto_globus;
Image foto_luigi;
Image foto_foquita;

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;

	//here add your init stuff

	//Inicialitzem les fotografies del globus, Luigi i foca.
	foto_globus.loadTGA("../res/globus.tga");
	if(foto_globus.loadTGA("../res/globus.tga") == false){
	    std::cout << "image not found" << std::endl;
	}
    foto_luigi.loadTGA("../res/luigipixels.tga");
    if(foto_luigi.loadTGA("../res/luigipixels.tga") == false){
        std::cout << "image not found" << std::endl;
    }

    foto_foquita.loadTGA("../res/foquita.tga");
    if(foto_foquita.loadTGA("../res/foquita.tga") == false){
        std::cout << "image not found" << std::endl;
    }

    /*Amb la constant total_flakes definida al application.h, creem amb un for els flakes de forma random,
    especificant els seus paràmetres (posició x, posició y, tamany, color i velocitat). Perquè cada snowflake tingui
    les seves pròpies característiques, hem fet ús de la funció randomValue().*/
    for(int i = 0; i < total_flakes; i++){
        snow[i].x = randomValue() * (window_width-1);
        snow[i].y = randomValue() * (window_height-1);
        snow[i].size = (randomValue()*5)+1;
        snow[i].color = Color(255, 255, 230);
        snow[i].velocity = randomValue()*4;
    }

}

//render one frame
void Application::render( Image& framebuffer )
{
	//clear framebuffer if we want to start from scratch
	//framebuffer.fill(Color::BLACK);

	//here you can add your code to fill the framebuffer

    // Function to draw HELLO (and a star) using simple figures.
	if(render_task == task1){
        framebuffer.fill(Color::BLACK);
        framebuffer.workWithBasicFigures(framebuffer);
	}

	// Function to draw a Gradient -> from red (left) to blue.
    else if(render_task == task2a){
        framebuffer.fill(Color::BLACK);
        framebuffer.drawGradientBtoR();
	}

    // Function to draw a Gradient with sinusoidal in the middle --> from black (below) to green and backwards.
    else if(render_task == task2e){
        framebuffer.fill(Color::BLACK);
        framebuffer.sinusoidalGradient();
    }

    // Function to draw a Radial Gradient --> from black (in the middle) to white.
    else if(render_task == task2b){
        framebuffer.fill(Color::BLACK);
        framebuffer.radialGradient();
    }

    // Function to draw a chess board (black and white).
    else if(render_task == task2f){
        framebuffer.fill(Color::BLACK);
        framebuffer.bandwBoard();
    }

    // Function to show the original image1.
    else if(render_task == image1){
        framebuffer.fill(Color::BLACK);
        framebuffer.originalPhoto(foto_globus);
    }

    // Function to show the image1 with a grayscale filter.
    else if(render_task == task3a){
        framebuffer.fill(Color::BLACK);
        framebuffer.grayscalePhoto(foto_globus);
    }

    // Function to show the image1 with an inverted filter.
    else if(render_task == task3b){
        framebuffer.fill(Color::BLACK);
        framebuffer.invertedPhoto(foto_globus);
    }

    // Function to show the image2.
    else if(render_task == image2){
        framebuffer.fill(Color::BLACK);
        framebuffer.originalPhoto(foto_luigi);
    }

    // Function to show the image2 rotated.
    else if(render_task == task4a){
        framebuffer.fill(Color::BLACK);
        framebuffer.rotateImage(foto_luigi, -30);
    }

    // Function to show the image2 scaled.
    else if(render_task == task4b){
        framebuffer.fill(Color::BLACK);
        framebuffer.scaleImage(foto_luigi, 0.45);
    }

    //Function to show the animated snowflakes with a seal in the background.
    else if(render_task == task5a){
        framebuffer.fill(Color::BLACK);
        framebuffer.originalPhoto(foto_foquita);
        for(int i = 0; i < total_flakes; i++){
            framebuffer.drawCircle(snow[i].x, snow[i].y, snow[i].size, snow[i].color, true);
        }
    }

    //fill every pixel of the image with some random data
	/*for (unsigned int x = 0; x < framebuffer.width; x++)
	{
		for (unsigned int y = 0; y < framebuffer.height; y++)
		{
			framebuffer.drawCircle(x, y, size; //random color
		}
	}*/
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	/*if (keystate[SDL_SCANCODE_SPACE]) //if key space is pressed
	{

	}*/

    /*Actualizem les partícules a partir de la fórmula donada en el PDF, de manera que quan una partícula arriba al
    final de la pantalla (en l'eix Y), es generi una altra al principi d'aquesta.*/
    for(int i = 0; i < total_flakes; i++){

        snow[i].y -= snow[i].velocity * seconds_elapsed * 50;

        if(snow[i].y <= 0){
            snow[i].x = randomValue() * (window_width - 1);
            snow[i].y = window_height-1;
            snow[i].size = (randomValue()*7)+1;
            snow[i].color = Color(255, 255, 230);
            snow[i].velocity = randomValue()*4;
        }
    }

	//to read mouse position use mouse_position
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{

    //Carreguem totes les tasques en tecles diferents del keyboard (llegir el README.md).
    //to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch(event.keysym.scancode)
	{
		case SDL_SCANCODE_1:
		    render_task = task1;
            break;
        case SDL_SCANCODE_A:
            render_task = task2a;
            break;
        case SDL_SCANCODE_B:
            render_task = task2e;
            break;
        case SDL_SCANCODE_C:
            render_task = task2b;
            break;
        case SDL_SCANCODE_D:
            render_task = task2f;
            break;
        case SDL_SCANCODE_X:
            render_task = image1;
            break;
        case SDL_SCANCODE_Y:
            render_task = task3a;
            break;
        case SDL_SCANCODE_Z:
            render_task = task3b;
            break;
        case SDL_SCANCODE_Q:
            render_task = image2;
            break;
        case SDL_SCANCODE_R:
            render_task = task4a;
            break;
        case SDL_SCANCODE_S:
            render_task = task4b;
            break;
        case SDL_SCANCODE_M:
            render_task = task5a;
            break;
        case SDL_SCANCODE_ESCAPE:
            exit(0);
            break; //ESC key, kill the app
    }
}

//keyboard key up event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//...
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		//if you read mouse position from the event, careful, Y is reversed, use mouse_position instead
	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}