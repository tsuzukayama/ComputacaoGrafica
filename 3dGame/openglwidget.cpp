#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget * parent) : QOpenGLWidget(parent)
{
    numEnemies = 10;
    score = 0;
    maxScore = 0;
    speed = 5;
    shooting = false;    
    bulletSpeed = 25;
    isPlayerDead = false;
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glEnable(GL_DEPTH_TEST);

    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer.start(0);
    time.start();
    bulletTime.start();

    model = std::make_shared<Model>(this);
    enemy = std::make_shared<Model>(this);
    bullet = std::make_shared<Model>(this);
    worldBox = std::make_shared<Model>(this);

    // set enemies
    for(int i=0; i<NUM_MAX_ENEMIES; ++i)
    {
        QVector3D pos = enemyPos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = ((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f;
        float y = ((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f;
        float z = -((qrand() / (float)RAND_MAX) * 50.0f);
        pos.setX(x);
        pos.setY(y);

        pos.setZ(-50);
        enemyPos[i] = pos;
    }

    enemy->material.ambient = QVector4D(1.0f, 0.02f, 0.02f, 1.0f);
    model->material.ambient = QVector4D(0.02f, 0.02f, 1.0f, 1.0f);

    model->readOFFFile(":/models/models/player_ship_grey.off", "phong");
    enemy->readOFFFile(":/models/models/enemy_ship.off", "phong");
    worldBox->readOFFFile(":/models/models/cube.off", "cubemap");
}

void OpenGLWidget::paintGL()
{
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(1, 1, 1, 1);

    // load player
    if (!model)
        return;

    model->setLightAndCamera(light, camera);

    model->drawModel(modelPos.x(), modelPos.y(), modelPos.z(), 0.5, 90, 0, 90);

    // load enemies
    if (!enemy)
        return;

    enemy->setLightAndCamera(light, camera);
    // make enemies red

    for(int i = 0; i < floor(numEnemies); ++i) {
        enemy->drawModel(enemyPos[i].x(), enemyPos[i].y(), enemyPos[i].z(), 0.3);
    }

    // load bullet
    if (!bullet)
        return;

    bullet->setLightAndCamera(light, camera);
    bullet->readOFFFile(":/models/models/cube.off");

    for ( auto &pos : bulletPos ) {
        bullet->drawModel(pos.x(), pos.y(), pos.z(), 0.1);
    }

    // load worldBox
    // worldBox->loadCubeMapTexture();
    // worldBox->drawModel(0, 0, 0, 2);
}

void OpenGLWidget::resizeGL(int width, int height)
{
    camera.resizeViewport(width, height);

    if (model) model->trackBall.resizeViewport(width, height);

    update();
}

void OpenGLWidget::animate()
{
    if(!isPlayerDead) {
        float elapsedTime = time.restart() / 1000.0f;

        score += elapsedTime;

        float xBorder = 1.8f;
        float yBorder = 0.8f;

        // move player
        if (modelPos.x() < -xBorder) {
            modelPos.setX(-xBorder);
        } else if (modelPos.x() > xBorder){
            modelPos.setX(xBorder);
        }else
            modelPos.setX(modelPos.x() + (playerPosXOffsetLeft + playerPosXOffsetRight) * elapsedTime);

        if (modelPos.y() < -yBorder) {
            modelPos.setY(-yBorder);
        } else if (modelPos.y() > yBorder){
            modelPos.setY(yBorder);
        }else
            modelPos.setY(modelPos.y() + (playerPosYOffsetUp + playerPosYOffsetDown) * elapsedTime);

        // update enemies
        for(int i = 0; i < floor(numEnemies); ++i) {

            // check collision with player
            if (modelPos.y() > enemyPos[i].y() - (0.0f + 0.15f) &&
                modelPos.y() < enemyPos[i].y() + (0.0f + 0.15f) &&
                modelPos.x() > enemyPos[i].x() - (0.0f + 0.3f ) &&
                modelPos.x() < enemyPos[i].x() + (0.0f + 0.3f ) &&
                modelPos.z() > enemyPos[i].z() - (0.0f + 0.3f ) &&
                modelPos.z() < enemyPos[i].z() + (0.0f + 0.3f ))
            {
                isPlayerDead = true;
            }
            // check collision with bullet
            for ( auto &bullet : bulletPos ) {
                if (bullet.y() > enemyPos[i].y() - (0.0f + 0.15f) &&
                    bullet.y() < enemyPos[i].y() + (0.0f + 0.15f) &&
                    bullet.x() > enemyPos[i].x() - (0.0f + 0.35f) &&
                    bullet.x() < enemyPos[i].x() + (0.0f + 0.35f) &&
                    bullet.z() > enemyPos[i].z() - (0.0f + 0.35f) &&
                    bullet.z() < enemyPos[i].z() + (0.0f + 0.35f))
                {
                    QVector3D pos = enemyPos[i];
                    float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
                    float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
                    float x = rand()%(2-(-2) + 1) + (-2);
                    float y = rand()%(2-(-2) + 1) + (-2);

                    float z = -((qrand() / (float)RAND_MAX) * 100.0f);

                    pos.setX(x);
                    pos.setY(y);
                    pos.setZ(z);
                    enemyPos[i] = pos;

                    bullet.setZ(-100);

                    score += 10;
                }
            }

            // check if reached end of screen
            if(enemyPos[i].z() >= 1.0f){
                QVector3D pos = enemyPos[i];
                float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
                float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
                float x = rand()%(2-(-2) + 1) + (-2);
                float y = rand()%(2-(-2) + 1) + (-2);

                //float x = 0;
                //float y = 0;
                float z = -((qrand() / (float)RAND_MAX) * 100.0f);

                pos.setX(x);
                pos.setY(y);
                pos.setZ(z);
                enemyPos[i] = pos;

            }
            else enemyPos[i].setZ(enemyPos[i].z() + speed * elapsedTime);
        }

        // Update projectile

        // Move projectile
        for ( auto &bullet : bulletPos ) {
            bullet.setZ(bullet.z() - bulletSpeed * elapsedTime);

            // Check whether the projectile missed the target
            if (bullet.z() < -50.0f)
            {
                bulletPos.erase(bulletPos.begin());
            }
        }


        speed += 0.001;
        if (numEnemies < NUM_MAX_ENEMIES)
            numEnemies += 0.01;
        update();
    }
}


void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right) playerPosXOffsetRight = 1.0f;
    if (event->key() == Qt::Key_Left) playerPosXOffsetLeft = -1.0f;

    if (event->key() == Qt::Key_Up) playerPosYOffsetDown = 1.0f;
    if (event->key() == Qt::Key_Down) playerPosYOffsetUp = -1.0f;

    if (event->key() == Qt::Key_Space)
    {
        if (bulletTime.elapsed() > 500)
        {
            QVector3D pos = modelPos;
            bulletPos.push_back(pos);
            bulletTime.restart();
        }

    }

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

    if (event->key() == Qt::Key_Up)
        playerPosYOffsetDown = 0;

    if (event->key() == Qt::Key_Down)
        playerPosYOffsetUp = 0;
}

void OpenGLWidget::resetGame() {
    // update max score
    if(score > maxScore){
        maxScore = score;
    }

    score = 0;
    speed = 5;
    numEnemies = 10;
    isPlayerDead = false;

    for(int i=0; i<NUM_MAX_ENEMIES; ++i)
    {
        QVector3D pos = enemyPos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = ((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f;
        float y = ((qrand() / (float)RAND_MAX) * 20.0f) - 10.0f;
        float z = -((qrand() / (float)RAND_MAX) * 50.0f);
        pos.setX(x);
        pos.setY(y);

        pos.setZ(-10);
        enemyPos[i] = pos;
    }
}
