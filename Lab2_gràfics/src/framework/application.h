/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include "includes.h"
#include "framework.h"
#include "image.h"

class Application
{
public:
	//window
	SDL_Window* window;
	float window_width;
	float window_height;

	Image framebuffer;

	float time;

	//keyboard state
	const Uint8* keystate;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame

	int counter = 0; // click's counter
	bool click = false; // number of clicks to draw a figure

	// Definim una enumeració i la variable render_task per determinar la tasca en qüestió que ens apareixerà per pantalla
    enum {task1, task2, task3v, task3f, task4av, task4af, task4b};
    int render_task = -1;

	//constructor
	Application(const char* caption, int width, int height);

	//main methods
	void init( void );
	void render( Image& framebuffer );
	void update( double dt );

	//methods for events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp( SDL_MouseButtonEvent event );

	//other methods to control the app
	void setWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
		framebuffer.resize(width,height);
	}

	Vector2 getWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(w,h);
	}
	// Vectors with the position of the mouse
    Vector2 initial_pos;
    Vector2 middle_position;
	Vector2 final_pos;

	void start();
};


#endif 