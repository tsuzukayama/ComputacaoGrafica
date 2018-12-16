#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>


#include "light.h"
#include "model.h"
#include "camera.h"
#include "worldbox.h"

#define NUM_MAX_ENEMIES 10000
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
    void changeScore(QString);
    void changeMaxScore(QString);

public slots:
    void animate();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    std::shared_ptr<Model> model, enemy, bullet, enemyBoss, aim;
    std::shared_ptr<WorldBox> worldBox;


    enum XState { XIdle, TurningLeft, StabilizingLeft, TurningRight, StabilizingRight };
    enum YState { YIdle, TurningUp, StabilizingUp, TurningDown, StabilizingDown };

    QVector3D modelPos = QVector3D(0, 0, 0);
    QVector3D modelRotation = QVector3D(90, 0, 90);

    XState modelXState;
    YState modelYState;

    QVector3D enemyPos[NUM_MAX_ENEMIES];

    std::vector<QVector3D> bulletPos;

    // QVector3D worldBoxPos = QVector3D(0, -645.2f, -1270.0f);
    QVector3D worldBoxPos = QVector3D(0, 0, 0);
    QVector3D worldBoxRotation = QVector3D(0, 0, 0);


    float numEnemies; // number of enemies
    float score; // Number of hits
    float maxScore;
    float speed; // game speed
    float bulletSpeed; // bullet speed
    float playerSpeed; // player speed

    boolean isPlayerDead; // cehck if player is dead
    boolean shooting; // check if new bullet was fired

    Light light;
    Camera camera;

    float playerPosXOffsetLeft = 0, playerPosXOffsetRight = 0, playerPosYOffsetDown = 0, playerPosYOffsetUp = 0; // offset Right for player

    void updateScene(int);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resetGame();
    float randomFloat(float min, float max);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);    
};

#endif // OPENGLWIDGET_H
