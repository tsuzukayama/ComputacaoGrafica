#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtOpenGL>
#include <memory>

#define NUM_SQUARE 100

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

    GLuint vboVertices = 0;
    GLuint vboIndices = 0;
    GLuint vao = 0;

    GLuint shaderProgram;

    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<unsigned int []> indices = nullptr;

    QVector3D squarePos[NUM_SQUARE];

    int m_width, m_height;
    bool xAtEnd[NUM_SQUARE];
    bool yAtEnd[NUM_SQUARE];

    QTimer timer;
    QTime time;

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget();

    void createVBOs();
    void createShaders();
    void destroyVBOs();
    void destroyShaders();

signals:

public slots:
    void animate();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();    
};

#endif // OPENGLWIDGET_H
