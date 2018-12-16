#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>

class Camera
{
public:

    Camera();

    QVector3D eye = QVector3D(0, 0.2f, 1.5f);
    QVector3D center = QVector3D(0, 0, 0);
    QVector3D up = QVector3D(0, 1, 0);

    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;

    void computeViewMatrix();
    void resizeViewport(int width, int height);
    void moveCamera(float x = 0, float y = 0, float z = 0);
    float width, height;
};

#endif // CAMERA_H
