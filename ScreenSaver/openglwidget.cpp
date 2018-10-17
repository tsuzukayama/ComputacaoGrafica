#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{    
}

OpenGLWidget::~OpenGLWidget()
{    
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    qDebug(" OpenGL version : %s", glGetString(GL_VERSION));
    qDebug(" GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    createShaders();
    createVBOs();    

    for(int i=0; i<NUM_SQUARE; ++i)
    {
        QVector3D pos = squarePos[i];

        float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
        float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
        float x = cos(ang) * radius;
        float y = sin(ang) * radius;
        pos.setX(x);
        pos.setY(y);        
        // pos.setZ(0);
        pos.setZ(((qrand() / (float)RAND_MAX) * 2.0f) - 1.0f);
        squarePos[i] = pos;
    }

    time.start();
    timer.start(0);
    connect(&timer, SIGNAL(timeout()), this, SLOT(animate()));
    glEnable(GL_DEPTH_TEST);    
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

    GLuint locTranslation = glGetUniformLocation(shaderProgram, "translation");
    GLuint locOuterRadius = glGetUniformLocation(shaderProgram, "outerRadius");
    GLuint locViewDimensions = glGetUniformLocation(shaderProgram, "viewDimensions");

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    for(int i=0; i<NUM_SQUARE; ++i)
    {
        // render OpenGL here
        // drawFilledCircle(squarePos[i].x(), squarePos[i].y(), 120);
        glUniform4f(locTranslation, squarePos[i].x(), squarePos[i].y(), squarePos[i].z(), 0);
        glUniform1f(locOuterRadius, 0.1f);
        glUniform2f(locViewDimensions, 0.1, 0.1);
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, nullptr);
    }
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
    indices = std::make_unique<unsigned int []>(2 * 3);

    // Four vertices to define a square
    vertices[0] = QVector4D(-0.4f * 0.1f, -0.5f * 0.1f, -1, 1);
    vertices[1] = QVector4D( 0.4f * 0.1f, -0.5f * 0.1f, -1, 1);
    vertices[2] = QVector4D( 0.4f * 0.1f,  0.5f * 0.1f, -1, 1);
    vertices[3] = QVector4D(-0.4f * 0.1f,  0.5f * 0.1f, -1, 1);

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

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);

    vertices.reset();
    indices.reset();
}

void OpenGLWidget::destroyVBOs()
{
    makeCurrent();

    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboIndices);
    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboIndices = 0;
    vao = 0;
}

void OpenGLWidget::animate()
{

    float elapsedTime = time.restart() / 1000.0f;
    float speed = 0.1f;
    float zspeed = 2.8f;
    float signal = 1;
    for(int s=0; s<NUM_SQUARE; ++s)
    {
        QVector3D pos = squarePos[s];        
        float limit = 1 - pos.z();
        if (pos.x() >= limit) {
            xAtEnd[s] = false;
        }
        else if (pos.x() <= -limit) {
            xAtEnd[s] = true;
        }

        if (pos.y() >= limit) {
            yAtEnd[s] = false;
        }
        else if (pos.y() <= -limit) {
            yAtEnd[s] = true;
        }

        if(xAtEnd[s]) {
            pos.setX(pos.x() + speed * elapsedTime * signal);
        } else {
            pos.setX(pos.x() - speed * elapsedTime * signal);
        }

        if(yAtEnd[s]) {
            pos.setY(pos.y() + speed * elapsedTime * signal);
        } else {
            pos.setY(pos.y() - speed * elapsedTime * signal);
        }        
        pos.setZ(pos.z() + zspeed * elapsedTime / 50);
        if (pos.z() >= 1.0f)
        {
            float ang = (qrand() / (float)RAND_MAX) * 2 * 3.14159265f;
            float radius = 1 + (qrand() / (float)RAND_MAX) * 2;
            float x = cos(ang) * radius;
            float y = sin(ang) * radius;
            pos.setX(x);
            pos.setY(y);
            pos.setZ(-1);

        }

        squarePos[s] = pos;
    }
    update();
}
