#include "application.h"
#include "utils.h"
#include "image.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

//Incloem els dos fitxers.h restants.
#include "material.h"
#include "light.h"

Camera* camera = NULL;
Mesh* mesh = NULL;
Matrix44 model_matrix;
Shader* shader = NULL;
Texture* texture = NULL;
Texture* texture1 = NULL;

//Com en el laboratori anterior, inicialitzem totes les variables necessàries per donar-li valors. Primer apunten a NULL.
Shader* phong_shader = NULL;
Shader* phong_shader1 = NULL;
Shader* phong_shader2 = NULL;
Shader* phong_shader3 = NULL;
Material* material = NULL;
Light* light = NULL;

//Fixem el valor d'ambient light a x = y = z = 0.5 perquè es vegi més il·luminat i s'assembli més a la foto del Assignment.
Vector3 ambient_light(0.5, 0.5, 0.5);

//Inicialitzem també els dos vectors de materials i diferents figures, així com l'angle per girar cada figura en la
// tasca de diferents figures.
std::vector<Material*>materials;
std::vector<Matrix44> mesh_of_people;

float angle = 0;

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
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	mesh->loadOBJ("../res/meshes/lee.obj");

	//load the texture
	texture = new Texture();
	if(!texture->load("../res/textures/lee_color_specular.tga"))
	{
		std::cout << "Texture not found" << std::endl;
		exit(1);
	}

    texture1 = new Texture();
    if(!texture1->load("../res/textures/lee_normal.tga"))
    {
        std::cout << "Texture not found" << std::endl;
        exit(1);
    }

	//we load a shader
	shader = Shader::Get("../res/shaders/texture.vs","../res/shaders/texture.fs");

	//load whatever you need here
	//Carreguem els diferents fitxers (1 per tasca).
	phong_shader = Shader::Get("../res/shaders/phong.vs","../res/shaders/phong.fs");
    phong_shader1 = Shader::Get("../res/shaders/phong1.vs","../res/shaders/phong1.fs");
    phong_shader2 = Shader::Get("../res/shaders/phong2.vs","../res/shaders/phong2.fs");
    phong_shader3 = Shader::Get("../res/shaders/phong3.vs","../res/shaders/phong3.fs");

    //Inicialitzem la llum.
    light = new Light();

    // MATERIALS //

    //Creem els materials
    material = new Material();
    materials.resize(6);
    for (int i = 0; i < materials.size(); i++) {
        materials[i] = new Material();
    }

    // Pearl
    materials[0]->ambient.set(0.25, 0.20725, 0.20725);
    materials[0]->diffuse.set(1.0, 0.829, 0.829);
    materials[0]->specular.set(0.296648, 0.296648, 0.296648);

    // Ruby
    materials[1]->ambient.set(0.1745, 0.01175, 0.01175);
    materials[1]->diffuse.set(0.61424, 0.04136, 0.04136);
    materials[1]->specular.set(0.727811, 0.626959, 0.626959);

    // Bronze
    materials[2]->ambient.set(0.2125, 0.1275, 0.054);
    materials[2]->diffuse.set(0.714, 0.4284, 0.18144);
    materials[2]->specular.set(0.393548, 0.271906, 0.166721);

    // Gold
    materials[3]->ambient.set(0.24725, 0.1995, 0.0745);
    materials[3]->diffuse.set(0.75164, 0.60648, 0.22648);
    materials[3]->specular.set(0.628281, 0.555802, 0.366065);

    // Silver
    materials[4]->ambient.set(0.19225, 0.19225, 0.19225);
    materials[4]->diffuse.set(0.50754, 0.50754, 0.50754);
    materials[4]->specular.set(0.508273, 0.508273, 0.508273);

    // Turquoise
    materials[5]->ambient.set(0.1, 0.18725, 0.1745);
    materials[5]->diffuse.set(0.396, 0.74151, 0.60102);
    materials[5]->specular.set(0.297254, 0.30829, 0.306678);

    // PEOPLE //

    // Creem les meshes
    mesh_of_people.resize(6);
    Matrix44 aux;
    // Inicialitzem
    for (int i = 0; i < mesh_of_people.size(); i++) {
        mesh_of_people[i] = aux;
        mesh_of_people[i].setIdentity();
    }

    // Les coloquem a gust personal: Diferents rotacions i posicions per cada figura.
    mesh_of_people[0].rotate(angle,Vector3(0, 1, 0));
    mesh_of_people[0].translate(-13,0,-20);

    mesh_of_people[1].rotate(angle-(M_PI/14), Vector3(0, 1, 0));
    mesh_of_people[1].translate(0, 0, -30);

    mesh_of_people[2].rotate(angle-(M_PI/12), Vector3(0, 1, 0));
    mesh_of_people[2].translate(13,0, -40);

    mesh_of_people[3].rotate(angle-(M_PI/10), Vector3(0, 1, 0));
    mesh_of_people[3].translate(26, 0, -50);

    mesh_of_people[4].rotate(angle-(M_PI/8), Vector3(0, 1, 0));
    mesh_of_people[4].translate(39, 0, -60);

    mesh_of_people[5].rotate(angle-(M_PI/6), Vector3(0, 1, 0));
    mesh_of_people[5].translate(52, 0, -70);
}

//render one frame
void Application::render(void)
{
    //glClearColor(0.1, 0.2, 0.3, 1.0);
    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable( GL_DEPTH_TEST );

    //Get the viewprojection
    camera->aspect = window_width / window_height;
    Matrix44 viewprojection = camera->getViewProjectionMatrix();

    //Cridem a les diferents tasques segons el valor de render_task.
    if(render_task == original){
        Original(model_matrix, viewprojection, window);
    }
    else if(render_task == task1){
        Task1(model_matrix, viewprojection, window);
    }
    else if(render_task == task2){
        Task2(model_matrix, viewprojection, window);
    }
    else if(render_task == task3){
        Task3(model_matrix, viewprojection, window);
    }
    else if(render_task == task4){
        Task4(model_matrix, viewprojection, window);
    }
    else if(render_task == task5){
        Task5(model_matrix, viewprojection, window);
    }
}

//called after render
void Application::update(double seconds_elapsed)
{
    //Per cada tecla premuda, donem un valor a render_task, on aquest cridarà a una tasca en concret al render.
    if(keystate[SDL_SCANCODE_1])
        render_task = original;
    else if(keystate[SDL_SCANCODE_2])
        render_task = task1;
    else if(keystate[SDL_SCANCODE_3])
        render_task = task2;
    else if(keystate[SDL_SCANCODE_4])
        render_task = task3;
    else if(keystate[SDL_SCANCODE_5])
        render_task = task4;
    else if(keystate[SDL_SCANCODE_6])
        render_task = task5;

    // ANGLE
    if (keystate[SDL_SCANCODE_X])
        model_matrix.rotateLocal(seconds_elapsed,Vector3(0,1,0));
    else if (keystate[SDL_SCANCODE_Z])
        model_matrix.rotateLocal(seconds_elapsed,Vector3(0,-1,0));

	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;


    // CENTER MOVEMENT - W, A, S, D
    if (keystate[SDL_SCANCODE_W])
        camera->center.y += 10 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_A])
        camera->center.x -= 10 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_S])
        camera->center.y -= 10 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_D])
        camera->center.x += 10 * seconds_elapsed;

    // FOV MOVEMENT - F, G
    if (keystate[SDL_SCANCODE_F])
        camera->fov -= 10 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_G])
        camera->fov += 10 * seconds_elapsed;

    // SINGLE LIGHT for ex 1,2,4
    // LIGHT MOVEMENT B, H, N, M
    if (keystate[SDL_SCANCODE_B])
        light->position.x -= seconds_elapsed * 40.0;
    else if (keystate[SDL_SCANCODE_M])
        light->position.x += seconds_elapsed * 40.0;
    if (keystate[SDL_SCANCODE_H])
        light->position.y += seconds_elapsed * 40.0;
    else if (keystate[SDL_SCANCODE_N])
        light->position.y -= seconds_elapsed * 40.0;
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode){
		case SDL_SCANCODE_R: Shader::ReloadAll(); break;
        case SDL_SCANCODE_ESCAPE: exit(0); break; //ESC key, kill the app
	}

}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{

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

void Application::Original(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window *window){

    //Habilitem el shader donat pel codi
    shader->enable();

    //Establim el model de matriu, la viewprojection i la textura.
    shader->setMatrix44("model", model_matrix); //upload info to the shader
    shader->setMatrix44("viewprojection", viewprojection); //upload info to the shader

    shader->setTexture("color_texture", texture, 0 ); //set texture in slot 0/

    //render the data
    mesh->render(GL_TRIANGLES);

    //disable shader
    shader->disable();

    //swap between front buffer and back buffer
    SDL_GL_SwapWindow(this->window);
}

void Application::Task1(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window *window){

    //Habilitem el primer phong shader (tasca 1),
    phong_shader->enable();

    //Establim el model de matriu, la viewprojection i la textura, i totes les altres uniform que declarem al fitxer phong.fs.
    phong_shader->setMatrix44("model", model_matrix);
    phong_shader->setMatrix44("viewprojection", viewprojection);
    phong_shader->setTexture("u_texture", texture, 0);

    phong_shader->setVector3("light", light->position);
    phong_shader->setVector3("camera", camera->eye);

    phong_shader->setVector3("Ia", ambient_light);
    phong_shader->setVector3("Id", light->diffuse_color);
    phong_shader->setVector3("Is", light->specular_color);
    phong_shader->setFloat("alpha", material->shininess);

    //render the data
    mesh->render(GL_TRIANGLES);

    //disable shader
    phong_shader->disable();

    //swap between front buffer and back buffer
    SDL_GL_SwapWindow(this->window);
}


void Application::Task2(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window *window){

    //Habilitem el phong shader 1 (tasca2).
    phong_shader1->enable();

    //Establim el model de matriu, la viewprojection i la textura, i totes les altres uniform que declarem al fitxer phong1.fs.
    phong_shader1->setMatrix44("model", model_matrix);
    phong_shader1->setMatrix44("viewprojection", viewprojection);
    phong_shader1->setTexture("u_texture", texture, 0);

    phong_shader1->setVector3("light", light->position);
    phong_shader1->setVector3("camera", camera->eye);

    phong_shader1->setVector3("Ia", ambient_light);
    phong_shader1->setVector3("Id", light->diffuse_color);
    phong_shader1->setVector3("Is", light->specular_color);
    phong_shader1->setFloat("alpha", material->shininess);

    //render the data
    mesh->render(GL_TRIANGLES);

    //disable shader
    phong_shader1->disable();

    //swap between front buffer and back buffer
    SDL_GL_SwapWindow(this->window);

}

void Application::Task3(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window *window){

    //Habilitem el phong shader2 (tasca 3).
    phong_shader2->enable();

    //Establim el model de matriu, la viewprojection i la textura, i totes les altres uniform que declarem al fitxer phong2.fs.
    phong_shader2->setMatrix44("model", model_matrix);
    phong_shader2->setMatrix44("viewprojection", viewprojection);
    phong_shader2->setTexture("u_texture", texture, 0);
    phong_shader2->setTexture("u_texture2", texture1, 1);

    phong_shader2->setVector3("light", light->position);
    phong_shader2->setVector3("camera", camera->eye);

    phong_shader2->setVector3("Ia", ambient_light);
    phong_shader2->setVector3("Id", light->diffuse_color);
    phong_shader2->setVector3("Is", light->specular_color);
    phong_shader2->setFloat("alpha", material->shininess);

    //render the data
    mesh->render(GL_TRIANGLES);

    //disable shader
    phong_shader2->disable();

    //swap between front buffer and back buffer
    SDL_GL_SwapWindow(this->window);
}

void Application::Task4(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window *window){

    //Habilitem el phong_shader3 (tasca 4).
    phong_shader3->enable();

    //Establim el model de matriu, la viewprojection i la textura, i totes les altres uniform que declarem al fitxer phong3.fs.
    phong_shader3->setMatrix44("model", model_matrix);
    phong_shader3->setMatrix44("viewprojection", viewprojection);
    phong_shader3->setTexture("u_texture", texture, 0);
    phong_shader3->setTexture("u_texture2", texture1, 1);

    phong_shader3->setVector3("light", light->position);
    phong_shader3->setVector3("camera", camera->eye);

    phong_shader3->setVector3("Ia", ambient_light);
    phong_shader3->setVector3("Id", light->diffuse_color);
    phong_shader3->setVector3("Is", light->specular_color);

    //Com volem diferents materials per cada figura. Implementem del laboratori anterior el bucle per associar un material
    //a cada figura, però amb el phong modificat d'aquesta pràctica (tasca 3 per tenir més característiques de textura).
    for(int i = 0; i < materials.size(); i++){
        phong_shader3->setUniform3("Ka", materials[i]->ambient);
        phong_shader3->setUniform3("Kd", materials[i]->diffuse);
        phong_shader3->setUniform3("Ks", materials[i]->specular);
        phong_shader3->setFloat("alpha", materials[i]->shininess = 1);
        phong_shader3->setMatrix44("model", mesh_of_people[i]);

        //Renderitzem el data per a cada figura
        mesh->render(GL_TRIANGLES);
    }
    //disable shader when we do not need it any more
    phong_shader3->disable();

    //swap between front buffer and back buffer
    SDL_GL_SwapWindow(this->window);
}

void Application::Task5(Matrix44 model_matrix, Matrix44 viewprojection, SDL_Window *window){

}