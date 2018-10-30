#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    playerPosY = -0.9f;
    playerPosYOffset = 0;

    playerPosX = 0;
    playerPosXOffset = 0;

    targetPosYOffset = 2.0f;
    targetPosY = 0;


    projectilePosX = 0;
    projectilePosY = -0.09f;

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
    playerPosX += playerPosXOffset * elapsedTime;

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
    update();
}

// Strong focus is required
void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right) playerPosXOffset = 2.0f;
    if (event->key() == Qt::Key_Left) playerPosXOffset = -2.0f;

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

}
