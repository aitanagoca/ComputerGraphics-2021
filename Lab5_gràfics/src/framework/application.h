/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include "includes.h"
#include "framework.h"

class Application
{
public:
	//window
	SDL_Window* window;
	float window_width;
	float window_height;

	float time;

	//keyboard state
	const Uint8* keystate;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame

    enum{original, task1, task2, task3, task4, task5};
    int render_task = -1;

	//constructor
	Application(const char* caption, int width, int height);

	//main methods
	void init( void );
	void render( void );
	void update( double dt );

	//methods for events
	void onKeyPressed( SDL_KeyboardEvent event );
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp( SDL_MouseButtonEvent event );

	//other methods to control the app
	void setWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
	}

	Vector2 getWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(w,h);
	}

	void start();
	void Original(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window* window);
	void Task1(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window* window);
	void Task2(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window* window);
    void Task3(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window* window);
    void Task4(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window* window);
    void Task5(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window* window);
};


#endif 