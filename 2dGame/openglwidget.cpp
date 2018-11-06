#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    playerPosY = -0.8f;
    playerPosYOffset = 0;

    playerPosX = 0;
    playerPosXOffset = 0;

    playerPosXOffsetLeft = 0;
    playerPosXOffsetRight = 0;

    targetPosYOffset = 2.0f;
    targetPosY = 0;


    projectilePosX = 0;
    projectilePosY = -0.8f;

    numHits = 0;
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));    

    player = std::make_shared<Player>(this);
    bullet = std::make_shared<Bullet>(this);
    block = std::make_shared<Block>(this);

    for(int i=0; i<NUM_BLOCKS; ++i)
    {
        QVector3D pos = obstaclesPos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = cos(ang) * radius;
        float y = sin(ang) * radius;
        pos.setX(x);
        pos.setY(y);
        pos.setZ(0);
        obstaclesPos[i] = pos;
    }

    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer.start(0);

    time.start();
}

void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLWidget::paintGL()
{    
    player->drawModel(0.05, playerPosX, playerPosY);

    for(int i = 0; i < NUM_BLOCKS; ++i) {
        block->drawModel(obstaclesPos[i].x(), obstaclesPos[i].y());
    }

    // Projectile
    if (shooting)
    {
        bullet->drawModel(projectilePosX, projectilePosY);
    }
}

void OpenGLWidget::animate()
{
    float elapsedTime = time.restart() / 1000.0f;

    // Change player Y position

    playerPosY += playerPosYOffset * elapsedTime;
    playerPosX += (playerPosXOffsetLeft + playerPosXOffsetRight) * elapsedTime;

    if (playerPosX < -1) {
        playerPosX = -1;
    } else if (playerPosX > 1){
        playerPosX = 1;
    }

    // Update projectile
    if (shooting)
    {
        // Move projectile
        projectilePosY += 3.0f * elapsedTime;

        // Check whether the projectile missed the target
        if (projectilePosY > 1.0f)
        {
            qDebug("Missed");
            shooting = false;
        }

    }

    // update obstacles
    float speed = 1;
    for(int i = 0; i < NUM_BLOCKS; ++i) {

        // check colision with bullet
        if (shooting &&
           (projectilePosY > obstaclesPos[i].y() - 0.1f &&
            projectilePosY < obstaclesPos[i].y() + 0.1f &&
            projectilePosX > obstaclesPos[i].x() - 0.1f &&
            projectilePosX < obstaclesPos[i].x() + 0.1f &&
            projectilePosY <= 1.0f))
        {
            float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
            float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
            float x = cos(ang) * radius;
            float y = 5 + (sin(ang) * radius);
            obstaclesPos[i].setX(x);
            obstaclesPos[i].setY(y);
            projectilePosY = -0.8f;
            shooting = false;
        }

        // check if reached the end of screen
        if(obstaclesPos[i].y() <= -1.0f){
            float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
            float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
            float x = cos(ang) * radius;
            float y = 5 + (sin(ang) * radius);
            obstaclesPos[i].setX(x);
            obstaclesPos[i].setY(y);

        }
        else obstaclesPos[i].setY(obstaclesPos[i].y() - speed * elapsedTime);
    }

    update();
}

// Strong focus is required
void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right) playerPosXOffsetRight = 1.0f;
    if (event->key() == Qt::Key_Left) playerPosXOffsetLeft = -1.0f;

    if (event->key() == Qt::Key_Space)
    {
        if (!shooting)
        {
            shooting = true;
            projectilePosY = playerPosY;
            projectilePosX = playerPosX;
        }

    }
    qDebug() << QString("%1").arg(playerPosY);

    if (event->key() == Qt::Key_Escape)
    {
        QApplication::quit();
    }
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right )
        playerPosXOffsetRight = 0;

    if (event->key() == Qt::Key_Left)
        playerPosXOffsetLeft = 0;
}
