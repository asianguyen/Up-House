#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    float nearPlane = 1;
    float farPlane = 1;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool bumpmapping = true;
    bool circle = false;
    bool curve = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;
    int cameraDistance = 30;
    int cameraY = 5;
    float cameraSpeed = 0.05;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
