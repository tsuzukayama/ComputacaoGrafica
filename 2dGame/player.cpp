#include "Player.h"

Player::Player(QOpenGLWidget * _glWidget) {
    glWidget = _glWidget;
    glWidget -> makeCurrent();
    height = glWidget -> height();
    initializeOpenGLFunctions();
    createShaders();
    createVBOs();
}

Player::~Player() {
    destroyVBOs();
    destroyShaders();
}

void Player::destroyVBOs() {
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboIndices);
    glDeleteBuffers(1, &vboTexCoords);
    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboIndices = 0;
    vboTexCoords = 0;

    vao = 0;
}

void Player::destroyShaders() {
    glDeleteProgram(shaderProgram);
}

void Player::createVBOs() {
    glWidget -> makeCurrent();
    destroyVBOs();

    vertices = std::make_unique<QVector4D[]>(4);
    colors = std::make_unique<QVector4D[]>(4);
    indices = std::make_unique<unsigned int[]>(2 * 3);
    texCoords = std::make_unique<QVector2D []>(4);

    // create four vertices to define a square
    vertices[0] = QVector4D(-1, -1, 0, 1);
    vertices[1] = QVector4D(-1, 1, 0, 1);
    vertices[2] = QVector4D(1, 1, 0, 1);
    vertices[3] = QVector4D(1, -1, 0, 1);
    // create colors for the vertices
    colors[0] = QVector4D (1, 1, 1, 1) ; // red
    colors[1] = QVector4D (1, 1, 1, 1) ; // green
    colors[2] = QVector4D (1, 1, 1, 1) ; // blue
    colors[3] = QVector4D (1, 1, 1, 1) ; // yellow
    // topology of the mesh ( square )
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 2;
    indices[4] = 3;
    indices[5] = 0;

    texCoords [3] = QVector2D(0, 0);
    texCoords [2] = QVector2D(1, 0);
    texCoords [1] = QVector2D(1, 1);
    texCoords [0] = QVector2D(0, 1);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector4D), vertices.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, vboColors);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof( QVector4D ), colors.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);

    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(QVector2D),
    texCoords.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
}

void Player::createShaders() {
    // makeCurrent ();
    destroyShaders();

    QFile vs(":/shaders/vshader1.glsl");
    QFile fs(":/shaders/fshader1.glsl");

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
    const GLchar * source = stdStringVs.c_str();
    glShaderSource(vertexShader, 1, &source, 0);
    // Compile the vertex shader
    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv (vertexShader, GL_INFO_LOG_LENGTH ,&maxLength);
        // The maxLength includes the NULL character
        std::vector <GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s" ,&infoLog[0]);

        glDeleteShader(vertexShader);
        return;
    }
    // Create an empty fragment shader handle
    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Send the fragment shader source code to GL
    // Note that std :: string ’s . c_str is NULL character terminated.
    source = stdStringFs.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);
    // Compile the fragment shader
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector < GLchar > infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s" ,&infoLog[0]);
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
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        std::vector <GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);
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

void Player::drawModel(float size, float x, float y) {

    glClear(GL_COLOR_BUFFER_BIT);

    GLuint locScaling = glGetUniformLocation(shaderProgram, "scaling");
    GLuint locTranslation = glGetUniformLocation(shaderProgram, "translation");

    glUseProgram(shaderProgram);

    glBindVertexArray(vao);

    // Player
    GLuint locColorTexture = 0;
    locColorTexture = glGetUniformLocation (shaderProgram, "colorTexture");
    glUniform1i(locColorTexture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glUniform4f(locTranslation, x, y, 0, 0);
    glUniform1f(locScaling, size);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, 0);
}

void Player::loadTexture()
{
    glWidget->makeCurrent();

    QImage image;
    image.load(":/textures/lena.jpg");
    image = image.convertToFormat(QImage::Format_RGBA8888);

    qDebug() <<QString::number(image.width());

    if (textureID)
        glDeleteTextures(1, &textureID);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(),
        image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glWidget->update();
}
