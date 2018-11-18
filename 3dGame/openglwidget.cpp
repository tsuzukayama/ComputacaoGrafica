#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget * parent) : QOpenGLWidget(parent)
{
    numEnemies = 10;
    score = 0;
    speed = 5;
    shooting = false;
    numBullets = 0;

    // create enemy vector
    for(int i = 0; i < NUM_MAX_ENEMIES; ++i) {
        QVector3D pos;
        enemyPos.insert(enemyPos.begin() + i, pos);
    }

    // create enemy vector

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
        pos.setZ(z);

        pos.setZ(-10);
        enemyPos[i] = pos;
    }

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
    model->readOFFFile(":/models/models/m1119.off");

    QImage playerTex;
    playerTex.load(":/textures/textures/wheat.png");
    playerTex = playerTex.convertToFormat(QImage::Format_RGBA8888);
    model->loadTextureLayer(playerTex);

    model->drawModel(modelPos.x(), modelPos.y(), modelPos.z(), 0.5);

    // load enemies
    if (!enemy)
        return;

    enemy->setLightAndCamera(light, camera);
    enemy->readOFFFile(":/models/models/cube.off");

    QImage enemyTex;
    enemyTex.load(":/textures/textures/wheat.png");
    enemyTex = enemyTex.convertToFormat(QImage::Format_RGBA8888);
    enemy->loadTextureLayer(enemyTex);

    for(int i = 0; i < floor(numEnemies); ++i) {
        enemy->drawModel(enemyPos[i].x(), enemyPos[i].y(), enemyPos[i].z(), 0.1);
    }

    // load bullet
    if (!bullet)
        return;

    bullet->setLightAndCamera(light, camera);
    bullet->readOFFFile(":/models/models/cube.off");
    QImage bulletTex;
    bulletTex.load(":/textures/textures/wheat.png");
    bulletTex = bulletTex.convertToFormat(QImage::Format_RGBA8888);
    bullet->loadTextureLayer(bulletTex);

    for ( auto &pos : bulletPos ) {
        bullet->drawModel(pos.x(), pos.y(), pos.z(), 0.1);
    }
}

void OpenGLWidget::resizeGL(int width, int height)
{
    camera.resizeViewport(width, height);

    if (model) model->trackBall.resizeViewport(width, height);

    update();
}

void OpenGLWidget::animate()
{
    float elapsedTime = time.restart() / 1000.0f;

    qDebug("elapsed: %i", bulletTime.elapsed());

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
        if(enemyPos[i].z() >= 1.0f){
            float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
            float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
            float x = rand()%(2-(-2) + 1) + (-2);
            float y = rand()%(2-(-2) + 1) + (-2);
            float z = -((qrand() / (float)RAND_MAX) * 100.0f);

            enemyPos[i].setX(x);
            enemyPos[i].setY(y);
            enemyPos[i].setZ(z);

        }
        else enemyPos[i].setZ(enemyPos[i].z() + speed * elapsedTime);
    }

    // Update projectile

    // Move projectile
    for ( auto &bullet : bulletPos ) {
        bullet.setZ(bullet.z() - 3.0f * elapsedTime);

        // Check whether the projectile missed the target
        if (bullet.z() < -10.0f)
        {
            qDebug("Missed");
        }
    }


    speed += 0.001;
    if (numEnemies < NUM_MAX_ENEMIES)
        numEnemies += 0.01;
    update();
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
