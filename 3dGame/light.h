#ifndef LIGHT_H
#define LIGHT_H

#include <QVector4D>

class Light
{
public :
    Light();

    QVector4D position = QVector4D (15, 15, -50, 0);
    QVector4D ambient  = QVector4D (5, 5, 1, 1);
    QVector4D diffuse  = QVector4D (2, 2, 2, 2);
    QVector4D specular = QVector4D (4, 4, 2, 2);
};

#endif // LIGHT_H
