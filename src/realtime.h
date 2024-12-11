#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "utils/sceneparser.h"
//#include "shapes/shape.h"
#include "camera/camera.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    RenderData renderData;

    glm::mat4 m_model = glm::mat4(1.f);
    glm::mat4 m_view  = glm::mat4(1.f);
    glm::mat4 m_proj  = glm::mat4(1.f);

    float offset;

    GLuint m_shader;

    glm::vec4 m_lightPos;

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    Camera m_camera;
    SceneCameraData m_cameraData;

    struct ShapeData {
        GLuint vao;
        GLuint vbo;
        glm::mat4 modelMatrix;
        int vertexCount;
        SceneMaterial material;
    };

    std::vector<ShapeData> m_shapeDataList;

    std::unordered_map<PrimitiveType, GLuint> shapeVAOs;
    std::unordered_map<PrimitiveType, GLuint> shapeVBOs;


    void setupShapes();
    // void setupVAOVBOForShape(Shape &shape, PrimitiveType shapeType, const glm::mat4& ctm, SceneMaterial material);
    void setupShaders();
    void setUpMesh(const glm::mat4& ctm, SceneMaterial mat);

    //bump mappin
    void loadNormalMap();
    void loadNormalMap2();
    void loadNormalMap3();
    void loadNormalMap4();
    GLuint m_roofMap;
    GLuint m_wallMap;
    GLuint m_shingleMap;
    GLuint m_chimneyMap;

    //skybox:
    GLuint m_skyboxTexture;
    void setupSkyBox();
    GLuint m_skybox_shader;
    GLuint m_skyboxVAO;
    GLuint m_skyboxVBO;
    void setupSkyBoxGeometry();
    void renderSkybox();

    //Proj6:
    GLuint m_texture_shader;
    GLuint m_defaultFBO;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    std::vector<float> mesh_data;
    GLuint mesh_vao;
    GLuint mesh_vbo;
    int mesh_vertex_count;

    void paintTexture(GLuint texture);
    void setupFullscreenQuad();
    void makeFBO();
    glm::mat4 getRotationMatrix(const glm::vec3& axis, float angle);

    //from lab
    /**
     * @brief verifyVAO - prints in the terminal how OpenGL would interpret `triangleData` using the inputted VAO arguments
     * @param triangleData - the vector containing the triangle data
     * @param index - same as glVertexAttribPointer()
     * @param size - same as glVertexAttribPointer()
     * @param stride - same as glVertexAttribPointer()
     * @param offset - same as glVertexAttribPointer()
     */
    void verifyVAO(std::vector<GLfloat> &triangleData, GLuint index, GLsizei size, GLsizei stride, const void* offset) {

        int newStride = int(stride / 4);
        int groupNum = 0;
        int newOffset = static_cast<int>(reinterpret_cast<intptr_t>(offset)) / 4;

        for (int i = newOffset; i < triangleData.size(); i = i + newStride) {
            std::cout << "Group " << groupNum << " of Values for VAO index " << index << std::endl;
            std::cout << "[";
            for (auto j = i; j < i + size; ++j) {
                if (j != i + size - 1) {
                    std::cout << triangleData[j]<< ", ";
                } else {
                    std::cout << triangleData[j]<< "]" << std::endl;
                }
            }
            groupNum = groupNum + 1;
        }
        std::cout << "" << std::endl;
    }
};
