#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>


#include "light.h"
#include "model.h"
#include "camera.h"

#define NUM_MAX_ENEMIES 1000

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr);

    QTimer timer;
    QTime time;

signals:
    void statusBarMessage(QString);
    void enableComboShaders(bool);

public slots:
    void animate();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    std::shared_ptr<Model> model, enemy;

    std::vector<QVector3D> enemyPos;
    QVector3D modelPos = QVector3D(0, 0, 0);

    float numEnemies; // number of enemies
    float score; // Number of hits
    float speed; // game speed

    Light light;
    Camera camera;

    float playerPosXOffsetLeft = 0, playerPosXOffsetRight = 0, playerPosYOffsetDown = 0, playerPosYOffsetUp = 0; // offset Right for player

    void updateScene(int);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // OPENGLWIDGET_H
