//
// Created by Aitana González Cárdenas on 6/3/22.
//

#include "material.h"



Material::Material()
{
    ambient.set(1,1,1); //reflected ambient light
    diffuse.set(1, 1, 1); //reflected diffuse light
    specular.set(1, 1, 1); //reflected specular light
    shininess = 30.0; //glosiness coefficient (plasticity)
}