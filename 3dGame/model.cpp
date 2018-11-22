#include "model.h"

Model::Model(QOpenGLWidget *_glWidget)
{   
    glWidget = _glWidget;
    glWidget->makeCurrent();

    initializeOpenGLFunctions();
//    QImage image;
//    image.load(":/textures/textures/sun.jpg");
//    image = image.convertToFormat(QImage::Format_RGBA8888);
//    loadTextureLayer(image);

    shaderIndex = 3;
    numShaders = 0;
}

Model::~Model()
{
    destroyVBOs();
    destroyShaders();
}

void Model::destroyVBOs()
{
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboIndices);
    glDeleteBuffers(1, &vboNormals);
    glDeleteBuffers(1, &vboTexCoords);
    glDeleteBuffers(1, &vboTangents);

    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboIndices = 0;
    vboNormals = 0;
    vboTexCoords = 0;
    vboTangents = 0;
    vao = 0;
}

void Model::createVBOs()
{
    glWidget->makeCurrent();

    destroyVBOs();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector4D), vertices.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    vertices.reset();

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector3D), normals.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    normals.reset();

    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector2D), texCoords.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
    texCoords.reset();

    glGenBuffers(1, &vboTangents);
    glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector4D), tangents.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(3);
    tangents.reset();

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * 3 * sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);
    indices.reset();
}


void Model::destroyShaders ()
{
    glDeleteProgram(shaderProgram);
}


void Model::createShaders(QString shaderName)
{
    destroyShaders();

    QString vShaderPath = QString(":/shaders/shaders/v%1.glsl").arg(shaderName);
    QString fShaderPath = QString(":/shaders/shaders/f%1.glsl").arg(shaderName);
    QString gShaderPath = QString(":/shaders/shaders/g%1.glsl").arg(shaderName);

    QFile vs(vShaderPath);
    QFile fs(fShaderPath);
    QFile gs(gShaderPath);

    vs.open(QFile::ReadOnly | QFile::Text);
    fs.open(QFile::ReadOnly | QFile::Text);
    gs.open(QFile::ReadOnly | QFile::Text);

    QTextStream streamVs(&vs), streamFs(&fs), streamGs(&gs);

    QString qtStringVs = streamVs.readAll();
    QString qtStringFs = streamFs.readAll();
    QString qtStringGs = streamGs.readAll();

    std::string stdStringVs = qtStringVs.toStdString();
    std::string stdStringFs = qtStringFs.toStdString();
    std::string stdStringGs = qtStringGs.toStdString();

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

    // fragment shader

    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    source = stdStringFs.c_str();
    glShaderSource(fragmentShader, 1, &source, nullptr);

    // Compile the fragment shader
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        return;
    }

    // geometry shader

    GLuint geometryShader = 0;
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    // Send the fragment shader source code to GL
    source = stdStringGs.c_str();
    glShaderSource(geometryShader, 1, &source, nullptr);

    // Compile the fragment shader
    glCompileShader(geometryShader);

    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(geometryShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        return;
    }

    shaderProgram = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geometryShader);

    // Link our program
    glLinkProgram(shaderProgram);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(geometryShader);
        return;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDetachShader(shaderProgram, geometryShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);

    vs.close();
    fs.close();
    gs.close();
}

void Model::drawModel(float x, float y, float z, QVector3D scale, QVector3D rotation, boolean isToon)
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(x, y, z);
    modelMatrix.rotate(rotation.x(), QVector3D(1, 0, 0));
    modelMatrix.rotate(rotation.y(), QVector3D(0, 1, 0));
    modelMatrix.rotate(rotation.z(), QVector3D(0, 0, 1));

    modelMatrix.scale(scale);
    modelMatrix.translate(-midPoint);

    GLuint locModel = 0;
    GLuint locNormalMatrix = 0;
    GLuint locShininess = 0;
    locModel = glGetUniformLocation(shaderProgram, "model");
    locNormalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
    locShininess = glGetUniformLocation(shaderProgram, "shininess");

    glBindVertexArray(vao);

    glUniformMatrix4fv(locModel, 1, GL_FALSE, modelMatrix.data());
    glUniformMatrix3fv(locNormalMatrix, 1, GL_FALSE, modelMatrix.normalMatrix().data());
    glUniform1f(locShininess, static_cast<GLfloat>(material.shininess));

    if (textureID)
    {
        GLuint locColorTexture = 0;
        locColorTexture = glGetUniformLocation(shaderProgram, "colorTexture");
        glUniform1i(locColorTexture, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    if (textureLayerID)
    {
        GLuint locColorTextureLayer = 1;
        locColorTextureLayer = glGetUniformLocation(shaderProgram, "colorTextureLayer");
        glUniform1i(locColorTextureLayer, 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureLayerID);
    }

    if (textureCubeMapID)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubeMapID);
    }

    if(isToon)
        glDrawElements(GL_TRIANGLES_ADJACENCY, numFaces * 3, GL_UNSIGNED_INT, nullptr);
    else
        glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, nullptr);

}

void Model::readOFFFile(QString const &fileName, QString shaderName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly | QFile::Text);
    if (!file.isOpen())
    {
        qWarning("Cannot open file.");
        return;
    }

    QTextStream stream(&file);

    QString line;
    stream >> line;
    stream >> numVertices >> numFaces >> line;

    vertices = std::make_unique<QVector4D[]>(numVertices);
    indices = std::make_unique<unsigned int[]>(numFaces * 3);

    if (numVertices > 0)
    {
        float minLim = std::numeric_limits<float>::lowest();
        float maxLim = std::numeric_limits<float>::max();
        QVector4D max(minLim, minLim, minLim, 1.0);
        QVector4D min(maxLim, maxLim, maxLim, 1.0);
        for (unsigned int i = 0; i < numVertices; ++i)
        {
            float x, y, z;
            stream >> x >> y >> z;
            max.setX(std::max(max.x(), x));
            max.setY(std::max(max.y(), y));
            max.setZ(std::max(max.z(), z));
            min.setX(std::min(min.x(), x));
            min.setY(std::min(min.y(), y));
            min.setZ(std::min(min.z(), z));
            vertices[i] = QVector4D(x, y, z, 1.0);
        }

        midPoint = QVector3D((min + max) * 0.5);
        invDiag = std::sqrt(3.0) / (max - min).length();
    }

    for (unsigned int i = 0; i < numFaces; ++i)
    {
        unsigned int a, b, c;
        stream >> line >> a >> b >> c;
        indices[i * 3 + 0] = a;
        indices[i * 3 + 1] = b;
        indices[i * 3 + 2] = c;
    }
    file.close();

    createNormals();
    createTexCoords();
    createTangents();
    createShaders(shaderName);
    createVBOs();
}

void Model::createNormals()
{
    normals = std::make_unique<QVector3D[]>(numVertices);

    for (unsigned int i = 0; i < numFaces; ++i)
    {
        QVector3D a = QVector3D(vertices[indices[i * 3 + 0]]);
        QVector3D b = QVector3D(vertices[indices[i * 3 + 1]]);
        QVector3D c = QVector3D(vertices[indices[i * 3 + 2]]);
        QVector3D faceNormal = QVector3D::crossProduct((b - a), (c - b));

        // Accumulates face normals on the vertices
        normals[indices[i * 3 + 0]] += faceNormal;
        normals[indices[i * 3 + 1]] += faceNormal;
        normals[indices[i * 3 + 2]] += faceNormal;
    }

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        normals[i].normalize();
    }
}

void Model::createTexCoords()
{
    texCoords = std::make_unique<QVector2D[]>(numVertices);

    // Compute minimum and maximum values
    auto minz = std::numeric_limits<float>::max();
    auto maxz = std::numeric_limits<float>::lowest();

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        minz = std::min(vertices[i].z(), minz);
        maxz = std::max(vertices[i].z(), maxz);
    }

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        float s = (std::atan2(vertices[i].y(), vertices[i].x()) + M_PI) / (2 * M_PI);
        float t = 1.0f - (vertices[i].z() - minz) / (maxz - minz);
        texCoords[i] = QVector2D(s, t);
    }
}

void Model::createTangents()
{
    tangents = std::make_unique<QVector4D[]>(numVertices);

    std::unique_ptr<QVector3D[]> bitangents;
    bitangents = std::make_unique<QVector3D[]>(numVertices);

    for (unsigned int i = 0; i < numFaces ; ++i)
    {
        unsigned int i1 = indices[i * 3 + 0];
        unsigned int i2 = indices[i * 3 + 1];
        unsigned int i3 = indices[i * 3 + 2];

        QVector3D E = vertices[i1].toVector3D();
        QVector3D F = vertices[i2].toVector3D();
        QVector3D G = vertices[i3].toVector3D();

        QVector2D stE = texCoords[i1];
        QVector2D stF = texCoords[i2];
        QVector2D stG = texCoords[i3];

        QVector3D P = F - E;
        QVector3D Q = G - E;

        QVector2D st1 = stF - stE;
        QVector2D st2 = stG - stE;

        QMatrix2x2 M;
        M(0, 0) =  st2.y();
        M(0, 1) = -st1.y();
        M(1, 0) = -st2.x();
        M(1, 1) =  st1.x();
        M *= (1.0f / (st1.x() * st2.y() - st2.x() * st1.y()));

        QVector4D T = QVector4D (M(0, 0) * P.x() + M(0, 1) * Q.x(),
                                 M(0, 0) * P.y() + M(0, 1) * Q.y(),
                                 M(0, 0) * P.z() + M(0, 1) * Q.z(), 0.0);

        QVector3D B = QVector3D (M(1, 0) * P.x() + M(1, 1) * Q.x(),
                                 M(1, 0) * P.y() + M(1, 1) * Q.y(),
                                 M(1, 0) * P.z() + M(1, 1) * Q.z());
        tangents[i1] += T;
        tangents[i2] += T;
        tangents[i3] += T;

        bitangents[i1] += B;
        bitangents[i2] += B;
        bitangents[i3] += B;
    }

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        const QVector3D &n = normals[i];
        const QVector4D &t = tangents[i];
        tangents[i] = (t - n * QVector3D::dotProduct(n, t.toVector3D())).normalized();
        QVector3D b = QVector3D::crossProduct(n, t.toVector3D());
        float hand = QVector3D::dotProduct(b, bitangents[i]);
        tangents[i].setW((hand < 0.0f) ? -1.0f : 1.0f);
    }
}

void Model::loadTexture(const QImage &image)
{
    if (textureID)
    {
        glDeleteTextures(1, &textureID);
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Model::loadTextureLayer(const QImage &image)
{   
    if (textureLayerID)
    {
        glDeleteTextures(1, &textureLayerID);
    }

    glGenTextures(1, &textureLayerID);
    glBindTexture(GL_TEXTURE_2D, textureLayerID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Model::loadCubeMapTexture()
{
    /*
    QString folderName = "Some folder containing the cube maps";
    QImage negx = QImage(QString(folderName).append("/negx.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage negy = QImage(QString(folderName).append("/negy.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage negz = QImage(QString(folderName).append("/negz.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage posx = QImage(QString(folderName).append("/posx.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage posy = QImage(QString(folderName).append("/posy.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage posz = QImage(QString(folderName).append("/posz.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    */

    QImage negx = QImage(QString(":/textures/cube/negx.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage negy = QImage(QString(":/textures/cube/negy.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage negz = QImage(QString(":/textures/cube/negz.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage posx = QImage(QString(":/textures/cube/posx.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage posy = QImage(QString(":/textures/cube/posy.jpg")).convertToFormat(QImage::Format_RGBA8888 );
    QImage posz = QImage(QString(":/textures/cube/posz.jpg")).convertToFormat(QImage::Format_RGBA8888 );


    negx = negx.convertToFormat(QImage::Format_RGBA8888);
    negy = negy.convertToFormat(QImage::Format_RGBA8888);
    negz = negz.convertToFormat(QImage::Format_RGBA8888);
    posx = posx.convertToFormat(QImage::Format_RGBA8888);
    posy = posy.convertToFormat(QImage::Format_RGBA8888);
    posz = posz.convertToFormat(QImage::Format_RGBA8888);

    if (textureCubeMapID)
    {
        glDeleteTextures(1, &textureCubeMapID);
    }

    glGenTextures(1, &textureCubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubeMapID);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, negx.width(), negx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, negx.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, negy.width(), negy.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, negy.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, negz.width(), negz.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, negz.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, posx.width(), posx.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, posx.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, posy.width(), posy.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, posy.bits());
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, posz.width(), posz.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, posz.bits());

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}


void Model::setLightAndCamera(Light light, Camera camera) {
    GLuint shaderProgramID = shaderProgram;

    QVector4D ambientProduct = light.ambient * material.ambient;
    QVector4D diffuseProduct = light.diffuse * material.diffuse;
    QVector4D specularProduct = light.specular * material.specular;

    GLint locProjection = glGetUniformLocation(shaderProgramID, "projection");
    GLint locView = glGetUniformLocation(shaderProgramID, "view");
    GLint locLightPosition = glGetUniformLocation(shaderProgramID, "lightPosition");
    GLint locAmbientProduct = glGetUniformLocation(shaderProgramID, "ambientProduct");
    GLint locDiffuseProduct = glGetUniformLocation(shaderProgramID, "diffuseProduct");
    GLint locSpecularProduct = glGetUniformLocation(shaderProgramID, "specularProduct");

    GLint locShininess = glGetUniformLocation(shaderProgramID, "shininess");
    GLint locMaterialKd = glGetUniformLocation(shaderProgramID, "materialKd");
    GLint locMaterialKa = glGetUniformLocation(shaderProgramID, "materialKa");
    GLint locLightIntensity = glGetUniformLocation(shaderProgramID, "lightIntensity");

    glUseProgram(shaderProgramID);

    glUniformMatrix4fv(locProjection, 1, GL_FALSE, camera.projectionMatrix.data());
    glUniformMatrix4fv(locView, 1, GL_FALSE, camera.viewMatrix.data());
    glUniform4fv(locLightPosition, 1, &(light.position[0]));
    glUniform4fv(locAmbientProduct, 1, &(ambientProduct[0]));
    glUniform4fv(locDiffuseProduct, 1, &(diffuseProduct[0]));
    glUniform4fv(locSpecularProduct, 1, &(specularProduct[0]));

    glUniform3fv(locMaterialKd, 1, &(material.diffuse[0]));
    glUniform3fv(locMaterialKa, 1, &(material.ambient[0]));
    glUniform3fv(locLightIntensity, 1, &(QVector3D(1, 1, 1)[0]));

    glUniform1f(locShininess, material.shininess);
}
