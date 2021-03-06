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
    speed = 1;
    numEnemies = 10;

    hasLoad = false;

    for(int i = 0; i < NUM_MAX_ENEMIES; ++i) {
        QVector3D pos;

        obstaclesPos.insert(obstaclesPos.begin() + i, pos);
    }

    for(int i = 0; i < NUM_STARS; ++i) {
        QVector3D pos;

        starsPos.insert(starsPos.begin() + i, pos);
    }
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));    

    star = std::make_shared<Star>(this);
    player = std::make_shared<Player>(this);
    bullet = std::make_shared<Bullet>(this);
    enemy = std::make_shared<Enemy>(this);


    // set stars
    for(int i=0; i<NUM_STARS; ++i)
    {
        QVector3D pos = starsPos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = cos(ang) * radius;
        float y = sin(ang) * radius;
        pos.setX(x);
        pos.setY(y);
        starsPos[i] = pos;
    }

    // set obstacles
    for(int i=0; i<NUM_MAX_ENEMIES; ++i)
    {
        QVector3D pos = obstaclesPos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = cos(ang) * radius;
        float y = 10 + sin(ang) * radius;
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
    QPainter painter(this);

    for(int i = 0; i < NUM_STARS; ++i) {
        star->drawModel(0.009, starsPos[i].x(), starsPos[i].y());
    }

    player->drawModel(0.05, playerPosX, playerPosY);

    for(int i = 0; i < floor(numEnemies); ++i) {
        enemy->drawModel(0.05, obstaclesPos[i].x(), obstaclesPos[i].y());
    }



    // Projectile
    if (shooting)
    {
        bullet->drawModel(0.009, projectilePosX, projectilePosY);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(rect(), Qt::AlignRight,
                     QString("%1").arg(QString::number(
                        static_cast<int>(std::floor(score)))));

    painter.drawText(rect(), Qt::AlignLeft,
                     QString("Max score: %1").arg(QString::number(
                        static_cast<int>(std::floor(maxScore)))));

    if (player->isDead) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 36));
        painter.drawText(rect(), Qt::AlignCenter, "Enter to restart");
    }

}

void OpenGLWidget::animate()
{
    if (!player->isDead) {
        float elapsedTime = time.restart() / 1000.0f;

        score += elapsedTime;

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
        for(int i = 0; i < floor(numEnemies); ++i) {

            // check colision with player
            if (playerPosY > obstaclesPos[i].y() - 0.1f &&
                playerPosY < obstaclesPos[i].y() + 0.1f &&
                playerPosX > obstaclesPos[i].x() - 0.1f &&
                playerPosX < obstaclesPos[i].x() + 0.1f)
            {
                player->isDead = true;
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

        // update stars
        for(int i = 0; i < NUM_STARS; ++i) {
            if(starsPos[i].y() <= -1.0f){
                float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
                float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
                float x = cos(ang) * radius;
                float y = 5 + (sin(ang) * radius);
                starsPos[i].setX(x);
                starsPos[i].setY(y);

            }
            else starsPos[i].setY(starsPos[i].y() - speed * elapsedTime/5);
        }

        speed += 0.0001;
        if (numEnemies < NUM_MAX_ENEMIES)
            numEnemies += 0.01;

        update();
    }
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

    if (event->key() == Qt::Key_Return) this->resetGame();
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right )
        playerPosXOffsetRight = 0;

    if (event->key() == Qt::Key_Left)
        playerPosXOffsetLeft = 0;
}

void OpenGLWidget::resetGame() {
    // update max score
    if(score > maxScore){
        maxScore = score;
    }

    score = 0;
    speed = 1;
    numEnemies = 10;
    player->isDead = false;

    for(int i=0; i<NUM_MAX_ENEMIES; ++i)
    {
        QVector3D pos = obstaclesPos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = cos(ang) * radius;
        float y = 10 + sin(ang) * radius;
        pos.setX(x);
        pos.setY(y);
        obstaclesPos[i] = pos;
    }
}
