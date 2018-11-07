#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "bullet.h"
#include "player.h"
#include "block.h"

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

#include <memory>

#define NUM_MAX_ENEMIES 1000

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

    GLuint vboVertices = 0;
    GLuint vboColors = 0;
    GLuint vboIndices = 0;

    GLuint vao = 0;

    std::unique_ptr<QVector4D []> vertices = nullptr;
    std::unique_ptr<QVector4D []> colors = nullptr;
    std::unique_ptr<unsigned int[]> indices = nullptr;

    std::shared_ptr<Player>player= nullptr;
    std::shared_ptr<Bullet>bullet= nullptr;
    std::shared_ptr<Block>block= nullptr;

    std::vector<QVector3D> obstaclesPos;

    GLuint shaderProgram;

    float playerPosX; // Current player X position
    float playerPosY; // Current player Y position

    float playerPosXOffsetLeft; // offset Left for player
    float playerPosXOffsetRight; // offset Right for player

    bool shooting; // Whether the projectile is being animated
    float projectilePosX; // Projectile X position
    float projectilePosY; // Projectile Y position
    float score; // Number of hits
    float speed; // game speed
    float numEnemies; // number of enemies

    float maxScore;

    bool hasLoad;


    QTimer timer;
    QTime time;

public:
    explicit OpenGLWidget (QWidget *parent = 0);

protected:
    void initializeGL();
    void resizeGL (int width, int height);
    void paintGL();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);    

    void resetGame();

signals:
    void updateHitsLabel(QString);
    void updateMaxScoreLabel(QString);

public slots:
    void animate();
};
#endif
