#ifndef STARBG_H
#define STARBG_H


class StarBG
{
public:

    StarBG(QOpenGLWidget * _glWidget);
    ~StarBG();

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
