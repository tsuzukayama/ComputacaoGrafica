#ifndef WORLDBOX_H
#define WORLDBOX_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QTextStream>
#include <QFile>

#include <fstream>
#include <limits>
#include <iostream>
#include <memory>

#include "camera.h"
#include "light.h"
#include "material.h"

class WorldBox : public QOpenGLExtraFunctions
{
public:
    WorldBox(QOpenGLWidget *_glWidget);
    ~WorldBox();

    std::unique_ptr<QVector4D []> vertices;
    std::unique_ptr<QVector3D []> normals;
    std::unique_ptr<unsigned int[]> indices;
    std::unique_ptr<QVector2D []> texCoords;
    std::unique_ptr<QVector4D []> tangents;

    QOpenGLWidget *glWidget;

    unsigned int numVertices;
    unsigned int numFaces;

    void createNormals();
    void createTexCoords();
    void createTangents();

    GLuint vao = 0;

    GLuint vboVertices = 0;
    GLuint vboIndices = 0;
    GLuint vboNormals = 0;
    GLuint vboTexCoords = 0;
    GLuint vboTangents = 0;
    GLuint textureID = 0;
    GLuint textureLayerID = 0;
    GLuint textureCubeMapID = 0;

    void createVBOs();
    void destroyVBOs();

    void createShaders();
    void destroyShaders();

    void readOFFFile(const QString &fileName);
    void loadTexture(const QImage &image);
    void loadTextureLayer(const QImage &image);
    void loadCubeMapTexture();

    void drawModel(float x, float y, float z, float scale = 1, QVector3D rotation = QVector3D(0, 0, 0));
    void setLightAndCamera(Light light, Camera camera);

    QMatrix4x4 modelMatrix;
    QVector3D midPoint;
    double invDiag;
    // double z, x, y = 0;

    unsigned int shaderIndex;
    int numShaders;

    GLuint shaderProgram;

    Material material;
};

#endif // MODEL_H
