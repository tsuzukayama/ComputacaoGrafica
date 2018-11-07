#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // player position
    playerPosX = 0;
    playerPosY = -0.8f;

    playerPosXOffsetLeft = 0;
    playerPosXOffsetRight = 0;

    projectilePosX = 0;
    projectilePosY = -0.8f;

    score = 0;

    hasLoad = false;
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

    if(!hasLoad) player->loadTexture();

    for(int i = 0; i < NUM_BLOCKS; ++i) {
        block->drawModel(0.05, obstaclesPos[i].x(), obstaclesPos[i].y());
    }

    // Projectile
    if (shooting)
    {
        bullet->drawModel(projectilePosX, projectilePosY);
    }

    hasLoad = true;
}

void OpenGLWidget::animate()
{
    float elapsedTime = time.restart() / 1000.0f;

    score += elapsedTime;
    emit updateHitsLabel(QString("Score: %1").arg(floor(score)));

    // Change player Y position

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

        // check colision with player
        if (playerPosY > obstaclesPos[i].y() - 0.1f &&
            playerPosY < obstaclesPos[i].y() + 0.1f &&
            playerPosX > obstaclesPos[i].x() - 0.05f &&
            playerPosX < obstaclesPos[i].x() + 0.05f)
        {
            score = 0;
        }
        // check colision with bullet
        if (shooting &&
           (projectilePosY > obstaclesPos[i].y() - 0.05f &&
            projectilePosY < obstaclesPos[i].y() + 0.05f &&
            projectilePosX > obstaclesPos[i].x() - 0.05f &&
            projectilePosX < obstaclesPos[i].x() + 0.05f &&
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

            score += 10;
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
