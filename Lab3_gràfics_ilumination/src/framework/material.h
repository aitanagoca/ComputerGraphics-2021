//
// Created by Aitana González Cárdenas on 6/3/22.
//

#ifndef COMPUTERGRAPHICS_MATERIAL_H
#define COMPUTERGRAPHICS_MATERIAL_H

#pragma once

#include "framework.h"

class Material
{
public:

    //you can access this variables directly, do not need a setter/getter

    Vector3 ambient; //reflected ambient light
    Vector3 diffuse; //reflected diffuse light
    Vector3 specular; //reflected specular light
    float shininess; //glosiness coefficient (plasticity)

    Material();

};

#endif //COMPUTERGRAPHICS_MATERIAL_H