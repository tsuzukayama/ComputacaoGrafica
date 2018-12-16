#ifndef MODEL_H
#define MODEL_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QTextStream>
#include <QFile>

#include <fstream>
#include <limits>
#include <iostream>
#include <memory>

#include "light.h"
#include "material.h"
#include "trackball.h"
#include "camera.h"

class Model : public QOpenGLExtraFunctions
{
public:
    Model(QOpenGLWidget *_glWidget);
    ~Model();

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

    void createShaders(QString shaderName);
    void destroyShaders();

    void readOFFFile(const QString &fileName, QString shaderName = "phong");
    void loadTexture(const QImage &image);
    void loadTextureLayer(const QImage &image);
    void loadCubeMapTexture();

    void setLightAndCamera(Light light, Camera camera);
    void drawModel(float x, float y, float z, QVector3D scale = QVector3D(1, 1, 1), QVector3D rotation = QVector3D(0, 0, 0), boolean isToon = true);

    QMatrix4x4 modelMatrix;
    QVector3D midPoint;
    double invDiag;
    // double z, x, y = 0;

    Material material;

    unsigned int shaderIndex;
    int numShaders;

    GLuint shaderProgram;

    TrackBall trackBall;
};

#endif // MODEL_H
