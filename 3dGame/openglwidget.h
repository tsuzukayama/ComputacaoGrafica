#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>


#include "light.h"
#include "model.h"
#include "camera.h"

#define NUM_MAX_ENEMIES 1000
#define NUM_MAX_BULLETS 10

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget *parent = nullptr);

    QTimer timer;
    QTime time, bulletTime;

signals:
    void statusBarMessage(QString);
    void enableComboShaders(bool);

public slots:
    void animate();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    std::shared_ptr<Model> model, enemy, bullet;

    std::vector<QVector3D> enemyPos;
    std::vector<QVector3D> bulletPos;
    QVector3D modelPos = QVector3D(0, 0, 0);

    float numEnemies; // number of enemies
    float numBullets; // number of bullets
    float score; // Number of hits
    float speed; // game speed

    boolean shooting; // check if new bullet was fired

    Light light;
    Camera camera;

    float playerPosXOffsetLeft = 0, playerPosXOffsetRight = 0, playerPosYOffsetDown = 0, playerPosYOffsetUp = 0; // offset Right for player

    void updateScene(int);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // OPENGLWIDGET_H
