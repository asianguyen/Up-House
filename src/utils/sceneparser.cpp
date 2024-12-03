#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>


void SceneParser::traverseSceneGraph(SceneNode* node, glm::mat4 parentCTM, RenderData &renderData){
    glm::mat4 currentCTM = parentCTM;

    if(node == nullptr){
        return;
    }

    //While you are traversing the tree, build your cumulative transformation
    //apply node transformations
    for(SceneTransformation* transformation: node->transformations){
        switch(transformation->type){
        case TransformationType::TRANSFORMATION_ROTATE:
            currentCTM = glm::rotate(currentCTM, transformation->angle, transformation->rotate);
            break;
        case TransformationType::TRANSFORMATION_TRANSLATE:
            currentCTM = glm::translate(currentCTM, transformation->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            currentCTM = glm::scale(currentCTM, transformation->scale);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            currentCTM *= transformation->matrix;
            break;
        }
    }

    //for each primitive you encounter, you should:
    //(1) construct a RenderShapeData object using the primitive and its corresponding CTM, and
    //(2) append the RenderShapeData onto renderData.shapes.

    for(ScenePrimitive* primitive: node->primitives){
        RenderShapeData shapeData;
        shapeData.primitive = *primitive;
        shapeData.ctm = currentCTM;
        renderData.shapes.push_back(shapeData);
    }

    //Similarly, for each light you encounter (remember that each node in the scene graph contains primitives, lights, and transformations),
    //you will also need to construct a SceneLightData object and append it onto renderData.lights.
    for(SceneLight* light: node->lights){
        SceneLightData lightData;
        lightData.id = light->id;
        lightData.angle = light->angle;
        lightData.color = light->color;
        lightData.type = light->type;
        lightData.function = light->function;
        lightData.penumbra = light->penumbra;

        if (light->type != LightType::LIGHT_DIRECTIONAL) {
            lightData.pos = currentCTM * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); //transform the light's position
        }
        lightData.dir = currentCTM * light->dir; //transform light's direction


        renderData.lights.push_back(lightData); //append
    }

    for (SceneNode* child : node->children) {
        traverseSceneGraph(child, currentCTM, renderData);
    }


}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Task 5: populate renderData with global data, and camera data;
    renderData.cameraData = fileReader.getCameraData();
    renderData.globalData = fileReader.getGlobalData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!

    //First, get the root node of the scene graph from the fileReader.
    SceneNode* root = fileReader.getRootNode();

    //Also, clear() renderData.shapes. You will be appending to this shortly.
    renderData.shapes.clear();
    renderData.lights.clear();

    //Then, traverse the tree in a depth-first manner
    glm::mat4 identityMatrix(1.0f);
    traverseSceneGraph(root, identityMatrix, renderData);

    return true;
}
