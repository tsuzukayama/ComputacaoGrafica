#ifndef BULLET_H
#define BULLET_H

# include <QtOpenGL>
# include <QOpenGLWidget>
# include <QOpenGLExtraFunctions>
# include <memory>

class Bullet:public QOpenGLExtraFunctions {
        public:

            Bullet(QOpenGLWidget * _glWidget);
            ~Bullet();

            std::unique_ptr <QVector4D []> vertices = nullptr;
            std::unique_ptr<QVector4D[]> colors = nullptr;
            std::unique_ptr <unsigned int[]> indices = nullptr;

            QOpenGLWidget * glWidget;
            unsigned int numVertices;
            unsigned int numFaces;
            int height;

            GLuint vao = 0;
            GLuint vboVertices = 0;
            GLuint vboColors = 0;
            GLuint vboIndices = 0;
            GLuint shaderProgram = 0;
            void createVBOs();
            void createShaders();
            void destroyVBOs();
            void destroyShaders();
            void drawModel(float x, float y);
        };
        #endif // MODEL_H
