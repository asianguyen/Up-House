#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include "camera/camera.h"
#include "utils/scenefilereader.h"
#include "utils/sceneparser.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils/objparser.h"

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

    glDeleteBuffers(1, &house_vbo);
    glDeleteVertexArrays(1, &house_vao);

    glDeleteBuffers(1, &balloon_vbo);
    glDeleteVertexArrays(1, &balloon_vao);

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

    loadNormalMap();
    loadNormalMap2();
    loadNormalMap3();
    loadNormalMap4();
    setupShaders();
    setupSkyBox();
    setupSkyBoxGeometry();

    setupFullscreenQuad();
    makeFBO();

}

void Realtime::setupShaders(){
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/effects.vert", ":/resources/shaders/effects.frag");

    m_skybox_shader = ShaderLoader::createShaderProgram(":/resources/shaders/skybox.vert", ":/resources/shaders/skybox.frag");


}

void Realtime::loadNormalMap() {
    glGenTextures(1, &m_roofMap);
    glBindTexture(GL_TEXTURE_2D, m_roofMap);

    int width, height, nrChannels;

    std::string normalFile= "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/roof2.jpg";
    //std::string normalFile= "C:/Users/dhlee/OneDrive/Desktop/cs1230/cs1230-final/resources/images/roof2.jpg";

    unsigned char *data = stbi_load(normalFile.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); //generates mipmaps for the texture for visual quality at different distances
    }
    else
    {
        std::cout << "Normal tex failed to load at path: " << normalFile << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Realtime::loadNormalMap2() {
    glGenTextures(1, &m_wallMap);
    glBindTexture(GL_TEXTURE_2D, m_wallMap);

    int width, height, nrChannels;


    std::string wallFile= "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/walls.jpg";
    unsigned char *data = stbi_load(wallFile.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); //generates mipmaps for the texture for visual quality at different distances
    }
    else
    {
        std::cout << "Normal tex failed to load at path: " << wallFile << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Realtime::loadNormalMap3() {
    glGenTextures(1, &m_chimneyMap);
    glBindTexture(GL_TEXTURE_2D, m_chimneyMap);

    int width, height, nrChannels;
    std::string chimneyFile= "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/brick.jpeg";
    unsigned char *data = stbi_load(chimneyFile.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); //generates mipmaps for the texture for visual quality at different distances
    }
    else
    {
        std::cout << "Normal tex failed to load at path: " << chimneyFile << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Realtime::loadNormalMap4() {
    glGenTextures(1, &m_shingleMap);
    glBindTexture(GL_TEXTURE_2D, m_shingleMap);

    int width, height, nrChannels;
    std::string shingleFile= "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/shingles.jpg";
    unsigned char *data = stbi_load(shingleFile.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); //generates mipmaps for the texture for visual quality at different distances
    }
    else
    {
        std::cout << "Normal tex failed to load at path: " << shingleFile << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void Realtime::setupSkyBox(){
    glGenTextures(1, &m_skyboxTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

    //load each texture face

    std::vector<std::string> faces = {
        "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/right.jpg", //Positive X
        "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/left.jpg",//Negative X
        "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/top.jpg", //Positive Y
        "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/bottom.jpg", //Negative Y
        "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/front.jpg",//Positive Z
        "/Users/sophialim/Desktop/CS1230/cs1230-final/resources/images/back.jpg" //Negative Z

    };

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                         );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            std::cout << "Reason: " << stbi_failure_reason() << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void Realtime::setupSkyBoxGeometry() {
    float skyboxVertices[] = {
        //back face
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        //front face
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        //left face
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,


        //right face
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,


        //top face
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

        //bottom face
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f
    };

    glGenVertexArrays(1, &m_skyboxVAO);
    glGenBuffers(1, &m_skyboxVBO);

    glBindVertexArray(m_skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

    //generate and bind a VBO and a VAO for a fullscreen quad
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

    renderSkybox();
    glEnable(GL_DEPTH_TEST);

    glUseProgram(m_shader);

    GLint kaLocation = glGetUniformLocation(m_shader, "k_a");
    glUniform1f(kaLocation, m_ka);

    GLint kdLocation = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(kdLocation, m_kd);

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

    GLint bumpMappingLoc = glGetUniformLocation(m_shader, "bumpmapping");
    glUniform1i(bumpMappingLoc, settings.bumpmapping);

    // loop through all shapes
    for (const auto& shapeData : m_shapeDataList) {

        glBindVertexArray(shapeData.vao);
        //glBindBuffer(GL_ARRAY_BUFFER, mesh_vbo);

        GLint modelMatrixLocation = glGetUniformLocation(m_shader, "modelMatrix");

        //glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(m_model * shapeData.modelMatrix));
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(shapeData.modelMatrix));


        GLint shininessLocation = glGetUniformLocation(m_shader, "shininess");
        glUniform1f(shininessLocation, shapeData.material.shininess);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_roofMap); //Bind the normal map to texture unit 1
        GLuint normalMapLocation = glGetUniformLocation(m_shader, "roofMap");
        glUniform1i(normalMapLocation, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_wallMap); //Bind the normal map to texture unit 2
        GLuint normalMapLocation2 = glGetUniformLocation(m_shader, "wallMap");
        glUniform1i(normalMapLocation2, 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_chimneyMap); //Bind the normal map to texture unit 3
        GLuint normalMapLocation3 = glGetUniformLocation(m_shader, "chimneyMap");
        glUniform1i(normalMapLocation3, 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_shingleMap); //Bind the normal map to texture unit 3
        GLuint normalMapLocation4 = glGetUniformLocation(m_shader, "shingleMap");
        glUniform1i(normalMapLocation4, 4);

        glDrawArrays(GL_TRIANGLES, 0, shapeData.vertexCount);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, width() * m_devicePixelRatio, height() * m_devicePixelRatio);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintTexture(m_fbo_texture);

    glUseProgram(0);
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    if (settings.circle){
        Realtime::moveCameraBezierCircle(deltaTime.count());
    }
    if (settings.curve){
        Realtime::moveCameraBezier(deltaTime.count());
    }

}

void Realtime::renderSkybox() {

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glUseProgram(m_skybox_shader);

    glm::mat4 view = glm::mat4(glm::mat3(m_view));

    glUniformMatrix4fv(glGetUniformLocation(m_skybox_shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_skybox_shader, "projection"), 1, GL_FALSE, glm::value_ptr(m_proj));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
    glUniform1i(glGetUniformLocation(m_skybox_shader, "skybox"), 0);

    glBindVertexArray(m_skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
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

        if (renderData.shapes[1].primitive.type == PrimitiveType::PRIMITIVE_MESH){
            setUpHouseMesh(renderData.shapes[1].ctm, renderData.shapes[1].primitive.material, "/Users/sophialim/Desktop/CS1230/cs1230-final/house/house.obj");
        }

        if (renderData.shapes[0].primitive.type == PrimitiveType::PRIMITIVE_MESH){

            std::vector<std::vector<float>> data;
            objparser::loadOBJBalloons("/Users/sophialim/Desktop/CS1230/cs1230-final/house/finalballoons.obj", data);

            for (const std::vector<float>& subvector : data) {
                setUpBalloonMesh(renderData.shapes[0].ctm, renderData.shapes[0].primitive.material, subvector);
            }
            break;
        }


    }
}

void Realtime::setUpHouseMesh(const glm::mat4& ctm, SceneMaterial mat, const char* path) {

    ShapeData shapedata;
    shapedata.modelMatrix = ctm;

    glGenBuffers(1, &house_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, house_vbo);

    shapedata.vbo = house_vbo;

    std::vector<float> data;

    objparser::loadOBJ(path, data);

    std::cout << "Loaded house data size: " << data.size() << std::endl;

    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    house_vertex_count = data.size() / 21;

    shapedata.vertexCount = house_vertex_count;

    shapedata.material = mat;

    glGenVertexArrays(1, &house_vao);
    glBindVertexArray(house_vao);

    shapedata.vao = house_vao;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,21 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    //normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // ka attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

    //kd attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(9 * sizeof(GLfloat)));

    //ks attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(12 * sizeof(GLfloat)));

    //specular attribute
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(15 * sizeof(GLfloat)));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(16 * sizeof(GLfloat)));

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(18 * sizeof(GLfloat)));

    m_shapeDataList.push_back(shapedata);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}




void Realtime::setUpBalloonMesh(const glm::mat4& ctm, SceneMaterial mat, const std::vector<float> data) {

    ShapeData shapedata;
    shapedata.modelMatrix = ctm;

    glGenBuffers(1, &shapedata.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, shapedata.vbo);

    std::cout << "Loaded balloon data size: " << data.size() << std::endl;


    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    shapedata.vertexCount = data.size() / 21;
    shapedata.material = mat;

    shapedata.offsetRand = rand();

    glGenVertexArrays(1, &shapedata.vao);
    glBindVertexArray(shapedata.vao);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,21 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    //normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // ka attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

    //kd attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(9 * sizeof(GLfloat)));

    //ks attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(12 * sizeof(GLfloat)));

    //specular attribute
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(15 * sizeof(GLfloat)));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(16 * sizeof(GLfloat)));

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 21 * sizeof(GLfloat), reinterpret_cast<void*>(18 * sizeof(GLfloat)));

    m_shapeDataList.push_back(shapedata);

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

glm::vec3 Realtime::bezierTangent(float t, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3){
    float u = 1 - t;
    return 3 * u * u * (p1 - p0) + 6 * u * t * (p2 - p1) + 3 * t * t * (p3 - p2);
}

glm::vec3 Realtime::bezierPosition(float t, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3){
    float u = 1 - t;
    return u * u * u * p0 + 3 * u * u * t * p1 + 3 * u * t * t * p2 + t * t * t * p3;
}

void Realtime::moveCameraBezier(float deltaTime){

    if (m_tIncreasing) {
        m_t += deltaTime * settings.cameraSpeed;
        if (m_t >= 1.0f) {
            m_t = 1.0f;
            m_tIncreasing = false; // reverse direction
        }
    } else {
        m_t -= deltaTime * settings.cameraSpeed;
        if (m_t <= 0.0f) {
            m_t = 0.0f;
            m_tIncreasing = true; // reverse direction
        }
    }
    glm::vec3 p0(-10.0f, -10.0f, 0.0f);
    glm::vec3 p1(2.0f, 30.0f, -20.0f);
    glm::vec3 p2(4.0f, 30.0f, -10.0f);
    glm::vec3 p3(10.0f,-40.0f,10.0f);

    glm::vec3 position = Realtime::bezierPosition(m_t, p0, p1, p2, p3);

    for (auto& shapeData : m_shapeDataList) {
        shapeData.modelMatrix = glm::translate(glm::mat4(1.0f), position);

    }

}


void Realtime::moveCameraBezierCircle(float deltaTime) {
    m_t += deltaTime * settings.cameraSpeed;
    if (m_t >= 1.0f) {
        m_t = 0.f;
    } else if (m_t < 0.0f) {
        m_t = 0.f;
    }

    // bezier control points
    const float radius = settings.cameraDistance;
    const float y = settings.cameraY;
    glm::vec3 p0( radius, y,  0.0f);
    glm::vec3 p1( radius, y,  radius * 0.55f);
    glm::vec3 p2( radius * 0.55f, y,  radius);
    glm::vec3 p3( 0.0f, y,  radius);

    glm::vec3 p4(-radius * 0.55f, y,  radius);
    glm::vec3 p5(-radius, y,  radius * 0.55f);
    glm::vec3 p6(-radius, y,  0.0f);

    glm::vec3 p7(-radius, y, -radius * 0.55f);
    glm::vec3 p8(-radius * 0.55f, y, -radius);
    glm::vec3 p9( 0.0f, y, -radius);

    glm::vec3 p10(radius * 0.55f, y, -radius);
    glm::vec3 p11(radius, y, -radius * 0.55f);

    float segmentT = m_t * 4.0f;
    int segment = static_cast<int>(segmentT);
    segmentT -= segment;

    glm::vec3 position;
    glm::vec3 forward;

    if (segment == 0) {
        position = Realtime::bezierPosition(segmentT, p0, p1, p2, p3);
        forward = Realtime::bezierTangent(segmentT, p0, p1, p2, p3);
    } else if (segment == 1) {
        position = Realtime::bezierPosition(segmentT, p3, p4, p5, p6);
        forward = Realtime::bezierTangent(segmentT, p3, p4, p5, p6);
    } else if (segment == 2) {
        position = Realtime::bezierPosition(segmentT, p6, p7, p8, p9);
        forward = Realtime::bezierTangent(segmentT, p6, p7, p8, p9);
    } else if (segment == 3) {
        position = Realtime::bezierPosition(segmentT, p9, p10, p11, p0);
        forward = Realtime::bezierTangent(segmentT, p9, p10, p11, p0);
    }

    forward = glm::normalize(forward);

    m_cameraData.pos = glm::vec4(position, 1.0f);
    m_cameraData.look = -m_cameraData.pos;
    m_cameraData.up = glm::vec4(0.f, 1.f, 0.f, 0.f);
    m_view = m_camera.getViewMatrix(
        glm::vec3(m_cameraData.pos),
        glm::vec3(m_cameraData.look),
        glm::vec3(m_cameraData.up)
        );
    update();
}


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
    glUseProgram(m_shader);
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    static float totalTime = 0.0f; // Track total time for smooth animation
    totalTime += deltaTime;


    float angle = sin(totalTime * 2.0f) * glm::radians(3.f);

    glm::vec3 pivotPoint(0.0f, 20.0f, 0.0f);
    glm::mat4 T_toPivot = glm::translate(glm::mat4(1.0f), -pivotPoint);

    glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 T_back = glm::translate(glm::mat4(1.0f), pivotPoint);

    m_house_model = T_back * R * T_toPivot;

    if (m_shapeDataList.size() > 0){
        m_shapeDataList[0].modelMatrix = m_house_model;
    }

    updateBalloons(deltaTime);

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
    glUseProgram(0);


    update(); // asks for a PaintGL() call to occur
}

void Realtime::updateBalloons(float deltaTime) {

    static float balloon_angle = 0.f;
    balloon_angle += 50*deltaTime;


    for (int i = 2; i < m_shapeDataList.size(); i++) {
        ShapeData &data = m_shapeDataList[i];

        float x = cos(balloon_angle + data.offsetRand) * 0.003f;
        float z = sin(balloon_angle + data.offsetRand) * 0.003f;

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0, z));

        data.modelMatrix = data.modelMatrix * translation;

    }
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
