#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget * parent) : QOpenGLWidget(parent)
{
}

void OpenGLWidget::changeShader(int shaderIndex)
{
    if (!model)
        return;

    model->shaderIndex = shaderIndex;
    update();
}

void OpenGLWidget::wheelEvent(QWheelEvent *event)
{
   if(!model)
       return;

   model->zoom += 0.001 * event->delta();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    qDebug("OpenGL version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glEnable(GL_DEPTH_TEST);

    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer.start(0);
}

void OpenGLWidget::paintGL()
{
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!model)
        return;

    GLuint shaderProgramID = model->shaderProgram[model->shaderIndex];

    QVector4D ambientProduct = light.ambient * model->material.ambient;
    QVector4D diffuseProduct = light.diffuse * model->material.diffuse;
    QVector4D specularProduct = light.specular * model->material.specular;

    GLint locProjection = glGetUniformLocation(shaderProgramID, "projection");
    GLint locView = glGetUniformLocation(shaderProgramID, "view");
    GLint locLightPosition = glGetUniformLocation(shaderProgramID, "lightPosition");
    GLint locAmbientProduct = glGetUniformLocation(shaderProgramID, "ambientProduct");
    GLint locDiffuseProduct = glGetUniformLocation(shaderProgramID, "diffuseProduct");
    GLint locSpecularProduct = glGetUniformLocation(shaderProgramID, "specularProduct");
    GLint locShininess = glGetUniformLocation(shaderProgramID, "shininess");

    glUseProgram(shaderProgramID);

    glUniformMatrix4fv(locProjection, 1, GL_FALSE, camera.projectionMatrix.data());
    glUniformMatrix4fv(locView, 1, GL_FALSE, camera.viewMatrix.data());
    glUniform4fv(locLightPosition, 1, &(light.position[0]));
    glUniform4fv(locAmbientProduct, 1, &(ambientProduct[0]));
    glUniform4fv(locDiffuseProduct, 1, &(diffuseProduct[0]));
    glUniform4fv(locSpecularProduct, 1, &(specularProduct[0]));
    glUniform1f(locShininess, model->material.shininess);

    model->drawModel();
}

void OpenGLWidget::resizeGL(int width, int height)
{
    camera.resizeViewport(width, height);

    if (model)
        model->trackBall.resizeViewport(width, height);

    update();
}

void OpenGLWidget::showFileOpenDialog()
{
    QByteArray fileFormat = "off";
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open File", QDir::homePath(),
        QString("\%1 Files (*.\%2)").arg(QString(fileFormat.toUpper())).arg(QString(fileFormat)), nullptr
#ifdef Q_OS_LINUX
        , QFileDialog::DontUseNativeDialog
#endif
    );
    int shaderIndex = 0;
    if (!fileName.isEmpty())
    {
        if (model)
            shaderIndex = model->shaderIndex;

        model = std::make_shared<Model>(this);
        model->shaderIndex = shaderIndex;
        model->readOFFFile(fileName);

        model->trackBall.resizeViewport(width(), height());

        emit statusBarMessage(QString("Vertices: \%1, Faces: \%2").arg(model->numVertices).arg(model->numFaces));
        emit enableComboShaders(true);
    }
    update();
}

void OpenGLWidget::animate()
{
    update();
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

void OpenGLWidget::loadTexture()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        QString("Open Image File"), "",
        QString("JPEG (*.jpg *.jpeg);;PNG (*.png)"), nullptr
#ifdef Q_OS_LINUX
        , QFileDialog::DontUseNativeDialog
#endif
                                                   );
    if (!fileName.isEmpty() && model)
    {
        QImage image;
        image.load(fileName);
        image = image.convertToFormat(QImage::Format_RGBA8888);

        model->loadTexture(image);
    }
    update();
}

void OpenGLWidget::loadTextureLayer()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        QString("Open Image File"), "",
        QString("JPEG (*.jpg *.jpeg);;PNG (*.png)"), nullptr
#ifdef Q_OS_LINUX
        , QFileDialog::DontUseNativeDialog
#endif
                                                    );
    if (!fileName.isEmpty() && model)
    {
        QImage image;
        image.load(fileName);
        image = image.convertToFormat(QImage::Format_RGBA8888);

        model->loadTextureLayer(image);
    }
    update();
}

// Strong focus is required
void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        QApplication::quit();
    }
}

void OpenGLWidget::toggleBackgroundColor(bool changeBColor)
{
    makeCurrent();

    if (changeBColor)
         glClearColor(0, 0, 0, 1);
    else
         glClearColor(1, 1, 1, 1);

    update();
}
