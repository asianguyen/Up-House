#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "shapes/cube.h"
#include "shapes/cone.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "utils/shaderloader.h"
#include <glm/gtc/type_ptr.hpp>
#include "camera/camera.h"
#include "utils/scenefilereader.h"
#include "utils/sceneparser.h"
#include "shapes/shape.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_camera(SceneCameraData())
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    for (const auto& shapeData : m_shapeDataList) {
        glDeleteBuffers(1, &shapeData.vbo);
        glDeleteVertexArrays(1, &shapeData.vao);
    }
    m_shapeDataList.clear();

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // Shader setup (DO NOT EDIT)

    m_defaultFBO = 2;

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/effects.vert", ":/resources/shaders/effects.frag");

    setupFullscreenQuad();
    makeFBO();

}

void Realtime::setupFullscreenQuad() {
    std::vector<GLfloat> fullscreen_quad_data = {
        //     POSITIONS         //     UV COORDINATES     //
        -1.f,  1.f, 0.0f,          0.0f, 1.0f,  //Top left
        -1.f, -1.f, 0.0f,          0.0f, 0.0f,  //Bottom left
        1.f, -1.f, 0.0f,          1.0f, 0.0f,  //Bottom right

        1.f,  1.f, 0.0f,          1.0f, 1.0f,  //Top right
        -1.f,  1.f, 0.0f,          0.0f, 1.0f, //Top left
        1.f, -1.f, 0.0f,         1.0f, 0.0f // Bottom RIght
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    GLint position_loc = glGetAttribLocation(m_texture_shader, "position");
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(
        position_loc,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(GLfloat),
        reinterpret_cast<void*>(0)
        );

    GLint uv_loc = glGetAttribLocation(m_texture_shader, "uv");
    glEnableVertexAttribArray(uv_loc);
    glVertexAttribPointer(
        uv_loc,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(GLfloat),
        reinterpret_cast<void*>(3 * sizeof(GLfloat))
        );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Realtime::makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width() * m_devicePixelRatio, height() * m_devicePixelRatio, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width() * m_devicePixelRatio, height() * m_devicePixelRatio);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);


    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

}

void Realtime::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, width() * m_devicePixelRatio, height() * m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);

    //set view and projection matrices
    GLint viewMatrixLocation = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(m_view));
    GLint projMatrixLocation = glGetUniformLocation(m_shader, "projectionMatrix");
    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, glm::value_ptr(m_proj));

    GLint kaLocation = glGetUniformLocation(m_shader, "k_a");
    glUniform1f(kaLocation, m_ka);

    GLint kdLocation = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(kdLocation, m_kd);


    // GLint lightPosLocation = glGetUniformLocation(m_shader, "lightPos");
    // glUniform4fv(lightPosLocation, 1, glm::value_ptr(m_lightPos));

    glm::vec4 cameraPos = glm::inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    GLint ksLocation = glGetUniformLocation(m_shader, "k_s");
    glUniform1f(ksLocation, m_ks);

    GLint cameraPosLocation = glGetUniformLocation(m_shader, "cameraPos");
    glUniform4fv(cameraPosLocation, 1, glm::value_ptr(cameraPos));

    for (int i = 0; i < renderData.lights.size(); i++) {
        const auto& light = renderData.lights[i];

        std::string typeName = "lightTypes[" + std::to_string(i) + "]";
        std::string posName = "lightPositions[" + std::to_string(i) + "]";
        std::string dirName = "lightDirections[" + std::to_string(i) + "]";
        std::string colorName = "lightColors[" + std::to_string(i) + "]";
        std::string attenName = "lightAttenuations[" + std::to_string(i) + "]";
        std::string angleName = "lightAngles[" + std::to_string(i) + "]";

        glUniform1i(glGetUniformLocation(m_shader, typeName.c_str()), static_cast<int>(light.type));

        GLint lightColorLocation = glGetUniformLocation(m_shader, colorName.c_str());
        glUniform4fv(lightColorLocation, 1, glm::value_ptr(light.color));

        GLint lightAttenLocation = glGetUniformLocation(m_shader, attenName.c_str());
        glUniform3fv(lightAttenLocation, 1, glm::value_ptr(light.function));

        switch(light.type){
        case LightType::LIGHT_DIRECTIONAL:{
            GLint lightDirLocation = glGetUniformLocation(m_shader, dirName.c_str());
            glUniform4fv(lightDirLocation, 1, glm::value_ptr(light.dir));

            break;
        }
        case LightType::LIGHT_POINT:{

            GLint lightPosLocation = glGetUniformLocation(m_shader, posName.c_str());
            glUniform4fv(lightPosLocation, 1, glm::value_ptr(light.pos));
            break;
        }
        case LightType::LIGHT_SPOT:{
            GLint lightDirLocation = glGetUniformLocation(m_shader, dirName.c_str());
            glUniform4fv(lightDirLocation, 1, glm::value_ptr(light.dir));

            GLint lightPosLocation = glGetUniformLocation(m_shader, posName.c_str());
            glUniform4fv(lightPosLocation, 1, glm::value_ptr(light.pos));

            GLint lightAngleLocation = glGetUniformLocation(m_shader, angleName.c_str());
            glUniform2f(lightAngleLocation,light.angle, light.penumbra);

            break;
        }

        }

    }

    GLint lightCountLoc = glGetUniformLocation(m_shader, "lightCount");
    glUniform1i(lightCountLoc, renderData.lights.size());

    //loop through all shapes
    for (const auto& shapeData : m_shapeDataList) {

        //UPDATE pick it out for shape once
        glBindVertexArray(shapeData.vao);
        glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo);

        GLint modelMatrixLocation = glGetUniformLocation(m_shader, "modelMatrix");
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(shapeData.modelMatrix));

        GLint ambientColorLoc = glGetUniformLocation(m_shader, "materialAmbient");
        glUniform4fv(ambientColorLoc, 1, glm::value_ptr(shapeData.material.cAmbient));

        GLint diffuseColorLoc = glGetUniformLocation(m_shader, "materialDiffuse");
        glUniform4fv(diffuseColorLoc, 1, glm::value_ptr(shapeData.material.cDiffuse));

        GLint specularColorLoc = glGetUniformLocation(m_shader, "materialSpecular");
        glUniform4fv(specularColorLoc, 1, glm::value_ptr(shapeData.material.cSpecular));

        GLint shininessLocation = glGetUniformLocation(m_shader, "shininess");
        glUniform1f(shininessLocation, shapeData.material.shininess);

        glDrawArrays(GL_TRIANGLES, 0, shapeData.vertexCount);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, width() * m_devicePixelRatio, height() * m_devicePixelRatio);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintTexture(m_fbo_texture);

    //glUseProgram(0);
}

void Realtime::paintTexture(GLuint texture){
    //directly from lab 11

    glUseProgram(m_texture_shader);

    glUniform1i(glGetUniformLocation(m_texture_shader, "useInvert"), settings.perPixelFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "useBlur"), settings.kernelBasedFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "tex"), 0);

    int texWidth, texHeight;
    glBindTexture(GL_TEXTURE_2D, texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
    glUniform2f(glGetUniformLocation(m_texture_shader, "texSize"), (float)texWidth, (float)texHeight);

    glBindVertexArray(m_fullscreen_vao);
    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0); // Set active texture slot to 0
    glBindTexture(GL_TEXTURE_2D, texture); // Bind the input texture to slot 0

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::setupShapes() {
    //dont need to loop through all the shapes, use same VAO and VBO
    for (auto &primitive : renderData.shapes) {
        Shape *shape;

        switch (primitive.primitive.type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            shape = new Cube();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            shape = new Cone();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            shape = new Cylinder();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            shape = new Sphere();
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
        }

        shape->updateParams(settings.shapeParameter1, settings.shapeParameter2);
        setupVAOVBOForShape(*shape, primitive.primitive.type, primitive.ctm, primitive.primitive.material);

    }
}

void Realtime::setupVAOVBOForShape(Shape &shape, PrimitiveType shapeType, const glm::mat4& ctm, SceneMaterial material) {

    ShapeData shapeData;
    shapeData.modelMatrix = ctm;

    //NOTE: I tried my best to incorporate your feedback from project 5 about only rendering the VAO and VBO once per shape type but it proceeded to crash my computer 5 times and took ridiculously long to run. If you have any feedback about my trial implementation I would love to know, thanks!
    // //check if VBO for this shape type already exists
    // if(shapeVBOs.find(shapeType)== shapeVBOs.end()){
    //     glGenBuffers(1, &shapeData.vbo);
    //     glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo);

    //     std::vector<float> data = shape.generateShape();
    //     glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    //     shapeData.vertexCount = data.size() / 6;
    //     shapeVBOs[shapeType] = shapeData.vbo;
    // }else{
    //     shapeData.vbo = shapeVBOs[shapeType]; //reuse VBO
    // }

    // //check if VAO for this shape type already exists
    // if(shapeVAOs.find(shapeType)== shapeVAOs.end()){
    //     glGenVertexArrays(1, &shapeData.vao);
    //     glBindVertexArray(shapeData.vao);

    //     //position attribute
    //     glEnableVertexAttribArray(0);
    //     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    //     //normal attribute
    //     glEnableVertexAttribArray(1);
    //     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    //     shapeVAOs[shapeType] = shapeData.vao;

    // }else{
    //     shapeData.vao = shapeVAOs[shapeType]; //reuse VAO
    // }

    glGenBuffers(1, &shapeData.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, shapeData.vbo);

    std::vector<float> data = shape.generateShape();
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    shapeData.vertexCount = data.size() / 6;


    shapeData.material = material;

    glGenVertexArrays(1, &shapeData.vao);
    glBindVertexArray(shapeData.vao);

    //position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    //normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    m_shapeDataList.push_back(shapeData);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::sceneChanged() {
    std::string filepath = settings.sceneFilePath;
    SceneParser parser;

    //parse the file and store data in renderData
    bool success = parser.parse(filepath, renderData);

    if (success) {
        std::cout << "Scene successfully parsed!" << std::endl;

        m_cameraData = renderData.cameraData;

        m_camera = Camera(m_cameraData);

        m_view = m_camera.getViewMatrix(
            glm::vec3(m_cameraData.pos),
            glm::vec3(m_cameraData.look),
            glm::vec3(m_cameraData.up)
            );

        float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());

        m_proj = m_camera.getProjectionMatrix(
            aspectRatio,
            m_cameraData.heightAngle,
            settings.nearPlane,
            settings.farPlane
            );

        m_ka = renderData.globalData.ka;
        m_kd = renderData.globalData.kd;
        m_ks = renderData.globalData.ks;

        for (const auto& shapeData : m_shapeDataList) {
            glDeleteBuffers(1, &shapeData.vbo);
            glDeleteVertexArrays(1, &shapeData.vao);
        }
        m_shapeDataList.clear();

        setupShapes();

        update(); // asks for a PaintGL() call to occur
    } else {
        std::cerr << "Failed to parse scene." << std::endl;
    }

}

void Realtime::settingsChanged() {

    //keep track of parameters and update if it changes

    float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());

    m_proj = m_camera.getProjectionMatrix(
        aspectRatio,
        m_cameraData.heightAngle,
        settings.nearPlane,
        settings.farPlane
        );

    for (const auto& shapeData : m_shapeDataList) {
        glDeleteBuffers(1, &shapeData.vbo);
        glDeleteVertexArrays(1, &shapeData.vao);
    }
    m_shapeDataList.clear();

    //unnecessarily realocating data for shapes
    setupShapes();

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!



void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate


        glm::vec3 look = glm::normalize(glm::vec3(m_cameraData.look));
        glm::vec3 up = glm::normalize(glm::vec3(m_cameraData.up));

        //perpendicular to the look and up vectors
        glm::vec3 right = glm::normalize(glm::cross(look, up));

        float sensitivity = 0.005f;

        //Mouse X Distance Traveled: Rotates the camera about the axis defined by the world space vector (0,1,0)
        glm::mat4 horizontalRotation = getRotationMatrix(glm::vec3(0.0f, 1.0f, 0.0f), deltaX * sensitivity);

        //Mouse Y Distance Traveled: Rotates the camera about the axis defined by a vector perpendicular to the look and up vectors of the camera.
        glm::mat4 verticalRotation = getRotationMatrix(right, deltaY * sensitivity);

        glm::mat4 combinedRotation = verticalRotation * horizontalRotation;

        glm::vec4 rotatedLook = combinedRotation * glm::vec4(look, 0.0f);
        glm::vec4 rotatedUp = combinedRotation * glm::vec4(up, 0.0f);

        m_cameraData.look = rotatedLook;
        m_cameraData.up = rotatedUp;

        //update view matrix
        m_view = m_camera.getViewMatrix(
            glm::vec3(m_cameraData.pos),
            glm::vec3(m_cameraData.look),
            glm::vec3(m_cameraData.up)
            );

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    float speed = 5.0f * deltaTime;
    bool moved = false;

    glm::vec3 look = glm::normalize(glm::vec3(m_cameraData.look));
    glm::vec3 up = glm::normalize(glm::vec3(m_cameraData.up));
    glm::vec3 right = glm::normalize(glm::cross(look, up));
    glm::vec3 movement(0.0f);

    //W: Translates the camera in the direction of the look vector (forward)
    if (m_keyMap[Qt::Key_W]) movement += look * speed;
    //S: Translates the camera in the opposite direction of the look vector(backward)
    if (m_keyMap[Qt::Key_S]) movement -= look * speed;

    //A: Translates the camera in the left direction, perpendicular to the look and up vectors (left)
    if (m_keyMap[Qt::Key_A]) movement -= right * speed;
    //D: Translates the camera in the right direction, also perpendicular to the look and up vectors. (right)
    if (m_keyMap[Qt::Key_D]) movement += right * speed;

    //Space: Translates the camera along the world space vector (0,1,0)(up)
    if (m_keyMap[Qt::Key_Space]) movement += glm::vec3(0, 1, 0) * speed;
    //Ctrl: Translates the camera along the world space vector (0,-1,0)(down)
    if (m_keyMap[Qt::Key_Control]) movement -= glm::vec3(0, 1, 0) * speed; //command on mac?

    if (glm::length(movement) > 0) {
        m_cameraData.pos += glm::vec4(movement, 0.0f);
        moved = true;
    }

    if (moved) {
        //update view matrix
        m_view = m_camera.getViewMatrix(
            glm::vec3(m_cameraData.pos),
            glm::vec3(m_cameraData.look),
            glm::vec3(m_cameraData.up)
            );
    }

    update(); // asks for a PaintGL() call to occur
}

glm::mat4 Realtime::getRotationMatrix(const glm::vec3& axis, float angle) {
    float cosTheta = cos(angle);
    float sinTheta = sin(angle);
    float oneMinusCos = 1.0f - cosTheta;

    //Rodrigues' formula -algo
    return glm::mat4(
        cosTheta + axis.x * axis.x * oneMinusCos,
        axis.x * axis.y * oneMinusCos - axis.z * sinTheta,
        axis.x * axis.z * oneMinusCos + axis.y * sinTheta,
        0.0f,

        axis.x * axis.y * oneMinusCos + axis.z * sinTheta,
        cosTheta + axis.y * axis.y * oneMinusCos,
        axis.y * axis.z * oneMinusCos - axis.x * sinTheta,
        0.0f,

        axis.x * axis.z * oneMinusCos - axis.y * sinTheta,
        axis.y * axis.z * oneMinusCos + axis.x * sinTheta,
        cosTheta + axis.z * axis.z * oneMinusCos,
        0.0f,

        0.0f, 0.0f, 0.0f, 1.0f
        );
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}