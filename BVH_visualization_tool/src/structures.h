#ifndef STRUCTURES
#define STRUCTURES

#include <QVector3D>

struct DirLight {
    QVector3D position;
    QVector3D ambient, diffuse, specular;
};

struct Material {
    float shininess;
    QVector3D ambient, diffuse, specular;
};

#endif // STRUCTURES

