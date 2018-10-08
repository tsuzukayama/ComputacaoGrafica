#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtOpenGL>
#include <memory>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

    GLuint vboVertices = 0;
    GLuint vboColors = 0;
    GLuint vboIndices = 0;
    GLuint vao = 0;

    GLuint shaderProgram;

    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector4D []> colors = nullptr;
    std::unique_ptr<unsigned int []> indices = nullptr;

    int m_width, m_height;
    float m_red = 0, m_green = 0, m_blue = 0;
    bool xAtEnd = false;
    bool yAtEnd = false;
    bool toggleColor = false;

    int indexAtEnd = 5;

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
    void toggleBackgroundColor(bool checked);
    void changeDiagonal(bool checked);
    void changeFirstVertexColor(float r, float g, float b);
    void redSliderChanged(int value);
    void greenSliderChanged(int value);
    void blueSliderChanged(int value);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();    
};

#endif // OPENGLWIDGET_H
