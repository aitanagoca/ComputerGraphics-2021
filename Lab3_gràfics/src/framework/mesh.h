/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	The Mesh contains the info about how to render a mesh and also how to parse it from a file.
*/

#ifndef MESH_H
#define MESH_H

#include <vector>
#include "framework.h"
#include "camera.h"
#include "image.h"

class Mesh
{
public:
	std::vector< Vector3 > vertices; //here we store the vertices
	std::vector< Vector3 > normals;	 //here we store the normals
	std::vector< Vector2 > uvs;	 //here we store the texture coordinates

	Mesh();
	void clear();
	void render(Camera* camera, Image* framebuffer); //TODO

	void createPlane(float size);
	bool loadOBJ(const char* filename);
};



#endif