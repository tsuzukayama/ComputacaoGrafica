#ifndef STAR_H
#define STAR_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <memory>

class Star:public QOpenGLExtraFunctions {
public:

    Star(QOpenGLWidget * _glWidget);
    ~Star();

    std::unique_ptr <QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector4D[]> colors = nullptr;
    std::unique_ptr <unsigned int[]> indices = nullptr;

    QOpenGLWidget * glWidget;
    unsigned int numVertices;
    unsigned int numFaces;
    int height;

    QMatrix4x4 transformationMatrix;

    GLuint vao = 0;
    GLuint vboVertices = 0;
    GLuint vboColors = 0;
    GLuint vboIndices = 0;
    GLuint shaderProgram = 0;
    void createVBOs();
    void createShaders();
    void destroyVBOs();
    void destroyShaders();
    void drawModel(float size, float x, float y);
};
#endif // MODEL_H
