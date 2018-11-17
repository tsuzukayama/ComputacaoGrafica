#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>


#include "light.h"
#include "model.h"
#include "camera.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

    QTimer timer;

signals:
    void statusBarMessage(QString);
    void enableComboShaders(bool);

public slots:
    void showFileOpenDialog();
    void loadTexture();
    void loadTextureLayer();
    void changeShader(int);
    void animate();
    void toggleBackgroundColor(bool);

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    std::shared_ptr<Model> model;
    Light light;
    Camera camera;

    void updateScene(int);
};

#endif // OPENGLWIDGET_H
