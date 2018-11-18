#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget * parent) : QOpenGLWidget(parent)
{
    numEnemies = 10;
    score = 0;
    speed = 5;

    for(int i = 0; i < NUM_MAX_ENEMIES; ++i) {
        QVector3D pos;

        enemyPos.insert(enemyPos.begin() + i, pos);
    }
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
    model = std::make_shared<Model>(this);
    enemy = std::make_shared<Model>(this);

    // set enemiees
    for(int i=0; i<NUM_MAX_ENEMIES; ++i)
    {
        QVector3D pos = enemyPos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = cos(ang) * radius;
        float y = sin(ang) * radius;
        float z = -50 + sin(ang) * radius;
        pos.setX(x);
        pos.setY(y);
        pos.setZ(z);
        qDebug("%f, %f", x, y);

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

}

void OpenGLWidget::resizeGL(int width, int height)
{
    camera.resizeViewport(width, height);
    update();
}

void OpenGLWidget::animate()
{
    float elapsedTime = time.restart() / 1000.0f;

    // move player
    if (modelPos.x() < -1) {
        modelPos.setX(-1);
    } else if (modelPos.x() > 1){
        modelPos.setX(1);
    }else
        modelPos.setX(modelPos.x() + (playerPosXOffsetLeft + playerPosXOffsetRight) * elapsedTime);

    if (modelPos.y() < -1) {
        modelPos.setY(-1);
    } else if (modelPos.y() > 1){
        modelPos.setY(1);
    }else
        modelPos.setY(modelPos.y() + (playerPosYOffsetUp + playerPosYOffsetDown) * elapsedTime);

    // update enemies
    for(int i = 0; i < floor(numEnemies); ++i) {
        if(enemyPos[i].z() >= 1.0f){
            float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
            float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
            float x = cos(ang) * radius;
            float y = sin(ang) * radius;
            float z = -50 + sin(ang) * radius;
            enemyPos[i].setX(x);
            enemyPos[i].setY(y);
            enemyPos[i].setZ(z);

        }
        else enemyPos[i].setZ(enemyPos[i].z() + speed * elapsedTime);
    }

    speed += 0.01;
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
