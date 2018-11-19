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
    playerSpeed = 2;
    modelXState = XIdle;
    modelYState = YIdle;
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
    worldBox = std::make_shared<WorldBox>(this);

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
    bullet->material.ambient = QVector4D(0.02f, 1.0f, 0.02f, 1.0f);

    model->readOFFFile(":/models/models/player_ship_grey.off", "phong");
    enemy->readOFFFile(":/models/models/enemy_ship.off", "phong");
    bullet->readOFFFile(":/models/models/cube.off", "phong");
    worldBox->readOFFFile(":/models/models/cube.off");

    worldBox->loadCubeMapTexture();
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

    model->drawModel(modelPos.x(), modelPos.y(), modelPos.z(), QVector3D(0.5, 0.5, 0.5), modelRotation);

    // load enemies
    if (!enemy)
        return;

    enemy->setLightAndCamera(light, camera);
    // make enemies red

    for(int i = 0; i < floor(numEnemies); ++i) {
        enemy->drawModel(enemyPos[i].x(), enemyPos[i].y(), enemyPos[i].z(), QVector3D(0.3f, 0.3f, 0.3f));
    }

    // load bullet
    if (!bullet)
        return;
    bullet->setLightAndCamera(light, camera);


    for ( auto &pos : bulletPos ) {
        bullet->drawModel(pos.x(), pos.y(), pos.z(), QVector3D(0.02f, 0.02f, 0.5f));
    }

    if(!worldBox)
        return;
    worldBox->setLightAndCamera(light, camera);

    // load worldBox
    worldBox->drawModel(worldBoxPos.x(), worldBoxPos.y(), worldBoxPos.z(), 1000);
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

            camera.eye.setX(-xBorder);
            camera.center.setX(-xBorder);
            camera.computeViewMatrix();
        } else if (modelPos.x() > xBorder){

            modelPos.setX(xBorder);

            camera.eye.setX(xBorder);
            camera.center.setX(xBorder);
            camera.computeViewMatrix();
        } else {

            modelPos.setX(modelPos.x() + (playerPosXOffsetLeft + playerPosXOffsetRight) * elapsedTime);

            camera.eye.setX(camera.eye.x() + (playerPosXOffsetLeft + playerPosXOffsetRight) * elapsedTime);
            camera.center.setX(camera.center.x() + (playerPosXOffsetLeft + playerPosXOffsetRight) * elapsedTime);
            camera.computeViewMatrix();
        }

        if (modelPos.y() < -yBorder) {
            modelPos.setY(-yBorder);

            camera.eye.setY(-yBorder + 0.2f);
            camera.center.setY(-yBorder);
            camera.computeViewMatrix();
        } else if (modelPos.y() > yBorder){
            modelPos.setY(yBorder);

            camera.eye.setY(yBorder + 0.2f);
            camera.center.setY(yBorder);
            camera.computeViewMatrix();
        } else {
            modelPos.setY(modelPos.y() + (playerPosYOffsetUp + playerPosYOffsetDown) * elapsedTime);

            camera.eye.setY(camera.eye.y() + (playerPosYOffsetUp + playerPosYOffsetDown) * elapsedTime);
            camera.center.setY(camera.center.y() + (playerPosYOffsetUp + playerPosYOffsetDown) * elapsedTime);
            camera.computeViewMatrix();
        }

        // rotate player X

        if (modelXState == TurningLeft && modelRotation.y() < 10)
        {
            modelRotation.setY(modelRotation.y() + elapsedTime * 100);
        }
        if (modelXState == StabilizingLeft && modelRotation.y() > 0)
        {
            modelRotation.setY(modelRotation.y() - elapsedTime * 100);
        }

        if (modelXState == TurningRight && modelRotation.y() > -10)
        {
            modelRotation.setY(modelRotation.y() - elapsedTime * 100);
        }
        if (modelXState == StabilizingRight && modelRotation.y() < 0)
        {
            modelRotation.setY(modelRotation.y() + elapsedTime * 100);
        }
        if (modelRotation.y() == 0.0f && (modelXState == StabilizingLeft || modelXState == StabilizingRight))
        {
            modelXState = XIdle;
        }

        // rotate player Y
        if (modelYState == TurningUp && modelRotation.x() < 100)
        {
            modelRotation.setX(modelRotation.x() + elapsedTime * 100);
        }
        if (modelYState == StabilizingUp && modelRotation.x() > 90)
        {
            modelRotation.setX(modelRotation.x() - elapsedTime * 100);
        }

        if (modelYState == TurningDown && modelRotation.x() > 80)
        {
            modelRotation.setX(modelRotation.x() - elapsedTime * 100);
        }
        if (modelYState == StabilizingDown && modelRotation.x() < 90)
        {
            modelRotation.setX(modelRotation.x() + elapsedTime * 100);
        }
        if (modelRotation.x() == 90.0f && (modelYState == StabilizingUp || modelYState == StabilizingDown))
        {
            modelYState = YIdle;
        }

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
                //float x = rand()%(2-(-2) + 1) + (-2);
                //float y = rand()%(2-(-2) + 1) + (-2);
                float max = 1;
                float min = -1;

                float x = RandomFloat(min, max);
                float y = RandomFloat(min, max);

                //float x = 0;
                //float y = 0;
                float z = -((qrand() / (float)RAND_MAX) * 200.0f);

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
    if (event->key() == Qt::Key_Right)
    {
        playerPosXOffsetRight = 1.0f * playerSpeed;
        modelXState = TurningRight;
    }
    if (event->key() == Qt::Key_Left)
    {
        playerPosXOffsetLeft = -1.0f * playerSpeed;
        modelXState = TurningLeft;
    }

    if (event->key() == Qt::Key_Up)
    {
        playerPosYOffsetDown = 1.0f * playerSpeed;
        modelYState = TurningUp;
    }
    if (event->key() == Qt::Key_Down)
    {
        playerPosYOffsetUp = -1.0f * playerSpeed;
        modelYState = TurningDown;
    }

    if (event->key() == Qt::Key_Space)
    {
        if (bulletTime.elapsed() > 500)
        {
            QVector3D pos = modelPos;
            pos.setZ(-0.5f);
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
    {
        playerPosXOffsetRight = 0;
        modelXState = StabilizingRight;
    }

    if (event->key() == Qt::Key_Left)
    {
        playerPosXOffsetLeft = 0;
        modelXState = StabilizingLeft;
    }

    if (event->key() == Qt::Key_Up)
    {
        playerPosYOffsetDown = 0;
        modelYState = StabilizingUp;
    }

    if (event->key() == Qt::Key_Down)
    {
        playerPosYOffsetUp = 0;
        modelYState = StabilizingDown;
    }
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

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!model)
        return;

    model->trackBall.mouseMove(event->localPos());
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!model)
        return;

    if (event->button() & Qt::LeftButton)
        model->trackBall.mousePress(event->localPos());
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!model)
        return;

    if (event->button() & Qt::LeftButton)
        model->trackBall.mouseRelease(event->localPos());
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
   if(!model)
       return;

   worldBoxPos.setY(worldBoxPos.y() + 0.001 * event->delta());
   qDebug("y: %f", worldBoxPos.y());
}

float OpenGLWidget::RandomFloat(float min, float max)
{
    // this  function assumes max > min, you may want
    // more robust error checking for a non-debug build
    assert(max > min);
    float random = ((float) rand()) / (float) RAND_MAX;

    // generate (in your case) a float between 0 and (4.5-.78)
    // then add .78, giving you a float between .78 and 4.5
    float range = max - min;
    return (random*range) + min;
}
