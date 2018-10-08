#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    time.start();
    timer.start(0);
    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
}

OpenGLWidget::~OpenGLWidget()
{
    destroyVBOs();
    destroyShaders();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    qDebug(" OpenGL version : %s", glGetString(GL_VERSION));
    qDebug(" GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    createShaders();
    createVBOs();
}

void OpenGLWidget::resizeGL(int width , int height)
{
    m_width = width;
    m_height = height;
    glClearColor(0, 0, 0, 1);
}

void OpenGLWidget::paintGL()
{
    glViewport(0,
               0, m_width, m_height);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, nullptr);
}

void OpenGLWidget::toggleBackgroundColor(bool checked)
{
    makeCurrent();
    if (checked)
        glClearColor(1, 1, 1, 1);
    else
        glClearColor(0, 0, 0, 1);

    update();
}

void OpenGLWidget::createShaders()
{
    makeCurrent();

    destroyShaders();

    QString vertexShaderFile(":/shaders/vshader1.glsl");
    QString fragmentShaderFile(":/shaders/fshader1.glsl");

    QFile vs(vertexShaderFile);
    QFile fs(fragmentShaderFile);

    vs.open(QFile::ReadOnly | QFile::Text);
    fs.open(QFile::ReadOnly | QFile::Text);

    QTextStream streamVs(&vs), streamFs(&fs);

    QString qtStringVs = streamVs.readAll();
    QString qtStringFs = streamFs.readAll();

    std::string stdStringVs = qtStringVs.toStdString();
    std::string stdStringFs = qtStringFs.toStdString();

    // Create an empty vertex shader handle
    GLuint vertexShader = 0;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    const GLchar *source = stdStringVs.c_str();
    glShaderSource(vertexShader, 1, &source, nullptr);

    // Compile the vertex shader
    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);
        glDeleteShader(vertexShader);
        return;
    }

    // Create an empty fragment shader handle
    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    // Note that std :: string ’s . c_str is NULL character terminated .
    source = stdStringFs.c_str();
    glShaderSource(fragmentShader, 1, &source, nullptr);

    // Compile the fragment shader
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, & isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog (maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        return;
    }

    shaderProgram = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link our program
    glLinkProgram(shaderProgram);

    // Note the different functions here : glGetProgram * instead of glGetShader *.
    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
        qDebug ("%s", &infoLog[0]);
        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vs.close();
    fs.close();
}

void OpenGLWidget::destroyShaders()
{
    makeCurrent();
    glDeleteProgram(shaderProgram);
}

void OpenGLWidget::createVBOs()
{
    makeCurrent();
    destroyVBOs();

    vertices = std::make_unique<QVector4D []>(4);
    colors = std::make_unique<QVector4D []>(4);
    indices = std::make_unique<unsigned int []>(2 * 3);

    // Four vertices to define a square
    vertices[0] = QVector4D(-0.4, -0.5, 0, 1);
    vertices[1] = QVector4D( 0.4, -0.5, 0, 1);
    vertices[2] = QVector4D( 0.4,  0.5, 0, 1);
    vertices[3] = QVector4D(-0.4,  0.5, 0, 1);

    // Vertex colors
    colors[0] = QVector4D(1, 0, 0, 1); // Red
    colors[1] = QVector4D(0, 1, 0, 1); // Green
    colors[2] = QVector4D(0, 0, 1, 1); // Blue
    colors[3] = QVector4D(1, 1, 0, 1); // Yellow

    // Topology of the mesh ( two triangles that form a square )
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), vertices.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), colors.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);

    vertices.reset();
    colors.reset();
    indices.reset();
}

void OpenGLWidget::destroyVBOs()
{
    makeCurrent();

    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboColors);
    glDeleteBuffers(1, &vboIndices);
    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboIndices = 0;
    vboColors = 0;
    vao = 0;
}

void OpenGLWidget::changeFirstVertexColor(float r, float g, float b)
{
    makeCurrent();
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);

    QVector4D *colors = (QVector4D *) glMapBufferRange(
                GL_ARRAY_BUFFER,
                0, 1 * sizeof (QVector4D),
                GL_MAP_WRITE_BIT);

    colors[0] = QVector4D(r, g, b, 1);

    glUnmapBuffer(GL_ARRAY_BUFFER);
    update();
}

void OpenGLWidget::changeDiagonal(bool checked)
{
    makeCurrent();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    unsigned int *idx = (unsigned int *) glMapBufferRange(
                GL_ELEMENT_ARRAY_BUFFER,
                0, 2 * 3 * sizeof (unsigned int),
                GL_MAP_WRITE_BIT );

    if (checked)
    {
        idx[0] = 0;
        idx[1] = 1;
        idx[2] = 3;

        idx[3] = 1;
        idx[4] = 2;
        idx[5] = 3;      
    }
    else
    {
        idx[0] = 0;
        idx[1] = 1;
        idx[2] = 2;

        idx[3] = 2;
        idx[4] = 3;
        idx[5] = 0;
    }
    glUnmapBuffer (GL_ELEMENT_ARRAY_BUFFER);
    update();
}

void OpenGLWidget::redSliderChanged(int value)
{
    m_red = value / 255.0f;
    changeFirstVertexColor(m_red, m_green, m_blue);
}

void OpenGLWidget::greenSliderChanged(int value)
{
    m_green = value / 255.0f;
    changeFirstVertexColor(m_red, m_green, m_blue);
}

void OpenGLWidget::blueSliderChanged(int value)
{
    m_blue = value / 255.0f;
    changeFirstVertexColor(m_red, m_green, m_blue);
}

void OpenGLWidget::animate()
{
    makeCurrent();
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);

    QVector4D *position = (QVector4D *) glMapBufferRange(
                GL_ARRAY_BUFFER,
                0, 4 * sizeof (QVector4D),
                GL_MAP_WRITE_BIT);

    float elapsedTime = time.restart() / 1000.0f;
    float speed = 0.5f;
    float signal = 1;

    for (int i = 0; i < 4; ++i)
    {
        if (position[i].x() >= 1) {
            xAtEnd = false;
            changeDiagonal(false);
        }
        else if (position[i].x() <= -1) {
            xAtEnd = true;
            changeDiagonal(true);
        }

        if (position[i].y() >= 1) {
            yAtEnd = false;
            changeDiagonal(false);
        }
        else if (position[i].y() <= -1) {
            yAtEnd = true;
            changeDiagonal(true);
        }

        if(xAtEnd) {
            position[i].setX(position[i].x() + speed * elapsedTime * signal);
        } else {
            position[i].setX(position[i].x() - speed * elapsedTime * signal);
        }

        if(yAtEnd) {
            position[i].setY(position[i].y() + speed * elapsedTime * signal);
        } else {
            position[i].setY(position[i].y() - speed * elapsedTime * signal);
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    update();
}
