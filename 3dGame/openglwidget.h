#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>


#include "light.h"
#include "model.h"
#include "camera.h"
#include "worldbox.h"

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
    std::shared_ptr<WorldBox> worldBox;

    QVector3D enemyPos[NUM_MAX_ENEMIES];
    std::vector<QVector3D> bulletPos;
    QVector3D modelPos = QVector3D(0, 0, 0);
    QVector3D worldBoxPos = QVector3D(0, -645.2f, -1016.0f);

    float numEnemies; // number of enemies
    float score; // Number of hits
    float maxScore;
    float speed; // game speed
    float bulletSpeed; // bullet speed

    boolean isPlayerDead; // cehck if player is dead
    boolean shooting; // check if new bullet was fired

    Light light;
    Camera camera;

    float playerPosXOffsetLeft = 0, playerPosXOffsetRight = 0, playerPosYOffsetDown = 0, playerPosYOffsetUp = 0; // offset Right for player

    void updateScene(int);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resetGame();

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // OPENGLWIDGET_H
