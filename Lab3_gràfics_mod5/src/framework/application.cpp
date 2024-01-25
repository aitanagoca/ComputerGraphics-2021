#include "application.h"
#include "utils.h"
#include "image.h"
#include "mesh.h"


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

	zbuffer.resize(w, h);
	framebuffer.resize(w, h);
}

//Here we have already GL working, so we can create meshes and textures
//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,10,20),Vector3(0,10,0),Vector3(0,1,0)); //define eye,center,up
	camera->perspective(60, window_width / (float)window_height, 0.1, 10000); //define fov,aspect,near,far

	//load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ("../res/lee.obj") )
		std::cout << "FILE Lee.obj NOT FOUND" << std::endl;

	//load the texture
	texture = new Image();
	texture->loadTGA("../res/color.tga");
}

//this function fills the triangle by computing the bounding box of the triangle in screen space and using the barycentric interpolation
//to check which pixels are inside the triangle. It is slow for big triangles, but faster for small triangles 
void Application::fillTriangle(Image& colorbuffer, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2) {
	//compute triangle bounding box in screen space
	//clamp to screen area
	//loop all pixels inside bounding box
	//we must compute the barycentrinc interpolation coefficients, weights of pixel P(x,y)
	//here add your code to test occlusions based on the Z of the vertices and the pixel (TASK 5)
	//here add your code to compute the color of the pixel (barycentric interpolation) (TASK 4)
	//draw the pixels in the colorbuffer x,y

}

//render one frame
void Application::render(Image& framebuffer) {

    framebuffer.fill(Color(40, 45, 60)); //clear
    //Donem un valor inicial al zbuffer (molt gran) per assegurar-nos que actualitzarà la profunditat màxima.
    zbuffer.fill(1000);

	//for every point of the mesh (to draw triangles take three points each time and connect the points between them (1,2,3,   4,5,6,   ...)
    for (int i = 0; i < mesh->vertices.size(); i=i+3) {
        Vector3 vertex1 = mesh->vertices[i]; //extract vertex from mesh
        Vector3 vertex2 = mesh->vertices[i+1];
        Vector3 vertex3 = mesh->vertices[i+2];

        Vector2 texcoord1 = mesh->uvs[i]; //texture coordinate of the vertex (they are normalized, from 0,0 to 1,1)
        Vector2 texcoord2 = mesh->uvs[i+1];
        Vector2 texcoord3 = mesh->uvs[i+2];

        //project every point in the mesh to normalized coordinates using the viewprojection_matrix inside camera
        Vector3 normalized_point1 = camera->projectVector(vertex1);
        Vector3 normalized_point2 = camera->projectVector(vertex2);
        Vector3 normalized_point3 = camera->projectVector(vertex3);


        //convert from normalized (-1 to +1) to framebuffer coordinates (0,W)
        normalized_point1.x = (normalized_point1.x + 1.0) * (framebuffer.width / 2.0);
        normalized_point1.y = (normalized_point1.y + 1.0) * (framebuffer.height / 2.0);

        normalized_point2.x = (normalized_point2.x + 1.0) * (framebuffer.width / 2.0);
        normalized_point2.y = (normalized_point2.y + 1.0) * (framebuffer.height / 2.0);

        normalized_point3.x = (normalized_point3.x + 1.0) * (framebuffer.width / 2.0);
        normalized_point3.y = (normalized_point3.y + 1.0) * (framebuffer.height / 2.0);

        //Establim el valor de cada coordenada de les variables de textura a les dimensions del framebuffer.

        texcoord1.set(texcoord1.x  * texture->width, texcoord1.y * texture->height);
        texcoord2.set(texcoord2.x * texture->width, texcoord2.y * texture->height);
        texcoord3.set(texcoord3.x * texture->width, texcoord3.y * texture->height);

        //paint points in framebuffer (using your drawTriangle function or the fillTriangle function)
        //Determinem els casos de les diferents tasques per poder realitzar la interacció.

        if(render_task == task4a){
            framebuffer.drawTriangle(normalized_point1.x, normalized_point1.y, normalized_point2.x, normalized_point2.y, normalized_point3.x, normalized_point3.y, Color::WHITE, false);
        }
        else if(render_task == task4b){
            framebuffer.drawTriangleInterpolated(normalized_point1.x, normalized_point1.y, normalized_point2.x, normalized_point2.y, normalized_point3.x, normalized_point3.y, Color::RED, Color::GREEN, Color::BLUE);
        }
        else if(render_task == task5and6){
            framebuffer.drawTriangleInterpolatedTexture(framebuffer, normalized_point1, normalized_point2, normalized_point3, texcoord1, texcoord2, texcoord3, zbuffer, texture);
        }


    }
}

//called after render
void Application::update(double seconds_elapsed) {
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (keystate[SDL_SCANCODE_SPACE]) {
		//...
	}

    //Establim diferents casos per moure les figures en totes les direccions demanades.
    //Per fer-ho, modifiquem els valors (de forma positiva o negativa) de les variables eye i center.
	if (keystate[SDL_SCANCODE_LEFT])
		camera->eye.x -= 5 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye.x += 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_UP])
        camera->eye.y -= 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_DOWN])
        camera->eye.y += 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_A])
        camera->center.x -= 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_D])
        camera->center.x += 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_W])
        camera->center.y += 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_S])
        camera->center.y -= 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_F])
        camera->eye.z += 5 * seconds_elapsed;
    if (keystate[SDL_SCANCODE_G])
        camera->eye.z -= 5 * seconds_elapsed;

	//if we modify the camera fields, then update matrices
	camera->updateViewMatrix();
	camera->updateProjectionMatrix();
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event ) {
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
        //Fem servir les tecles 1, 2 i 3 per canviar els diferents modes, que fan referència a les tasques demanades de la pràctica.
		case SDL_SCANCODE_ESCAPE:
		    exit(0);
		    break; //ESC key, kill the app
        case SDL_SCANCODE_1:
            render_task = task4a;
            break;
        case SDL_SCANCODE_2:
            render_task = task4b;
            break;
        case SDL_SCANCODE_3:
            render_task = task5and6;
            break;
	}
}

//keyboard released event 
void Application::onKeyUp(SDL_KeyboardEvent event) {
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event ) {
    if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
    {

    }
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event ) {
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start() {
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
