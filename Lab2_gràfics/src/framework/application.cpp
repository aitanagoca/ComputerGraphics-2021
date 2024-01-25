#include "application.h"
#include "utils.h"
#include "image.h"

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

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;

	//here add your init stuff
}

//render one frame
void Application::render( Image& framebuffer )
{
	//here you can add your code to fill the framebuffer

	// Si el nombre de vèrtexs necessaris per dur a terme una figura és correcta.
	// Per cada valor de render_task cridem a la funció per la tasca corresponent; tot netejant la pantalla prèviament (pintem de negre).
	// Després de relitzar cada tasca fem click false per resetejar el nombre de clicks.
	if(click){
        if(render_task == task1){
            framebuffer.fill(Color::BLACK);
            framebuffer.drawLineDDA(initial_pos.x, initial_pos.y, final_pos.x, final_pos.y, Color::PURPLE);
            click = false;
        }

        else if(render_task == task2) {
            framebuffer.fill(Color::BLACK);
            framebuffer.drawLineBresenham(initial_pos.x, initial_pos.y, final_pos.x, final_pos.y, Color::CYAN);
            click = false;
        }

        else if(render_task == task3v){
            framebuffer.fill(Color::BLACK);
            float radius = sqrt(pow(final_pos.x - initial_pos.x, 2) + pow(final_pos.y - initial_pos.y, 2));
            framebuffer.drawCircle(initial_pos.x, initial_pos.y, radius, Color::BLUE, false);
            click = false;
        }

        else if(render_task == task3f){
            framebuffer.fill(Color::BLACK);
            float radius = sqrt(pow(final_pos.x - initial_pos.x, 2) + pow(final_pos.y - initial_pos.y, 2));
            framebuffer.drawCircle(initial_pos.x, initial_pos.y, radius, Color::BLUE, true);
            click = false;
        }

        else if(render_task == task4av){
            framebuffer.fill(Color::BLACK);
            framebuffer.drawTriangle(initial_pos.x, initial_pos.y, middle_position.x, middle_position.y, final_pos.x, final_pos.y, Color::GRAY, false);
            click = false;
        }

        else if(render_task == task4af){
            framebuffer.fill(Color::BLACK);
            framebuffer.drawTriangle(initial_pos.x, initial_pos.y, middle_position.x, middle_position.y, final_pos.x, final_pos.y, Color::GRAY, true);
            click = false;
        }

        else if(render_task == task4b){
            framebuffer.fill(Color::BLACK);
            framebuffer.drawTriangleInterpolated(initial_pos.x, initial_pos.y, middle_position.x, middle_position.y, final_pos.x, final_pos.y, Color::RED, Color::GREEN, Color::BLUE);
            click = false;
        }
	}
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (keystate[SDL_SCANCODE_SPACE]) //if key space is pressed
	{
		//...
	}

	//to read mouse position use mouse_position
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch(event.keysym.scancode)
	{
	    // Per cada cas, les posicions inicials i finals, i mitjanes, en el cas del triangle,
	    // prenen el valor de NULL perquè no es quedin gravats al canviar de mode o fer una altra figura de la mateixa tasca.
	    case SDL_SCANCODE_1:
            framebuffer.fill(Color::BLACK);
            initial_pos.x = NULL;
            initial_pos.y = NULL;
            final_pos.x = NULL;
            final_pos.y = NULL;
	        render_task = task1;
	        break;
	    case SDL_SCANCODE_2:
            framebuffer.fill(Color::BLACK);
            initial_pos.x = NULL;
            initial_pos.y = NULL;
            final_pos.x = NULL;
            final_pos.y = NULL;
	        render_task = task2;
	        break;
	    case SDL_SCANCODE_3:
            framebuffer.fill(Color::BLACK);
            initial_pos.x = NULL;
            initial_pos.y = NULL;
            final_pos.x = NULL;
            final_pos.y = NULL;
	        render_task = task3v;
	        break;
        case SDL_SCANCODE_4:
            framebuffer.fill(Color::BLACK);
            initial_pos.x = NULL;
            initial_pos.y = NULL;
            final_pos.x = NULL;
            final_pos.y = NULL;
            render_task = task3f;
            break;
	    case SDL_SCANCODE_5:
            framebuffer.fill(Color::BLACK);
            initial_pos.x = NULL;
            initial_pos.y = NULL;
            middle_position.x = NULL;
            middle_position.y = NULL;
            final_pos.x = NULL;
            final_pos.y = NULL;
	        render_task = task4av;
	        break;
        case SDL_SCANCODE_6:
            framebuffer.fill(Color::BLACK);
            initial_pos.x = NULL;
            initial_pos.y = NULL;
            middle_position.x = NULL;
            middle_position.y = NULL;
            final_pos.x = NULL;
            final_pos.y = NULL;
            render_task = task4af;
            break;
	    case SDL_SCANCODE_7:
            framebuffer.fill(Color::BLACK);
            initial_pos.x = NULL;
            initial_pos.y = NULL;
            middle_position.x = NULL;
            middle_position.y = NULL;
            final_pos.x = NULL;
            final_pos.y = NULL;
	        render_task = task4b;
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

		// Declarem les condicions per obtenir la posició del mouse per cada click.
		// Aquestes posicions són les que passem per paràmetre en les diferents fucnions de dibuix.
		// Per fer-les diferents unes de les altres, utilitzem un comptador que augmenta en 1 per cada click,
		// i defineix si la posició del mouse és la inicial, la del mig o la del final.
		if(render_task == task1 || render_task == task2 || render_task == task3v || render_task == task3f){ //for lines & circles
            if(counter == 0){
                initial_pos = mouse_position;
                counter += 1;
            }
            else if(counter == 1){
                final_pos = mouse_position;
                click = true;
                counter = 0;
            }
		}
		else{ // for triangles
            if(counter == 0){
                initial_pos = mouse_position;
                counter += 1;
            }
            else if(counter == 1){
                middle_position = mouse_position;
                counter += 1;
            }
            else if(counter == 2){
                final_pos = mouse_position;
                click = true;
                counter = 0;
            }
		}
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
