#include "Parser.h"
#include "Scene.h"
#include "Sphere.h"
#include "Light.h"
#include "AABB.h"
#include "Plane.h"
#include "Mesh.h"
#include "HierachicalShape.h"
#include "Camera.h"
#include "MetaballPair.h"

#include <iostream>


void populateVectorHelper(glm::vec3& target, const std::string&& e0, json& data)
{
    try 
    {
        target.x = data[e0][0];
        target.y = data[e0][1];
        target.z = data[e0][2];
    }
    catch (...)
    {
        std::cerr << "Could not process critical JSON value. Exiting." << std::endl;
        throw std::exception();
    }
}

void populateVectorHelper(glm::vec3& target, const std::string&& e0, const std::string&& e1, json& data)
{
    try {
        target.x = data[e0][e1][0];
        target.y = data[e0][e1][1];
        target.z = data[e0][e1][2];
    }
    catch (...) 
    {
        std::cerr << "Could not process critical JSON value. Exiting." << std::endl;
        throw std::exception();
    }
}

Parser::Parser(const std::string&& _filename) :
    filename(_filename)
{
}

Parser::~Parser()
{
}

void pairMaterialwithObj(std::shared_ptr<Shape> s, std::vector<std::shared_ptr<Material>> materials, std::vector<int> materialIDs) 
{
    // Pair materials with object
    for (int i = 0; i < materialIDs.size(); i++)
    {
        for (int j = 0; j < materials.size(); j++)
        {
            if (materialIDs[i] == materials[j]->ID)
            {
                s->materials.push_back(materials[j]);
            }
        }
    }
}

void createModelMatrix(glm::mat4& M, const glm::vec3& t, const glm::vec3& r, const glm::vec3& s) 
{
    M = glm::translate(M, t);

    M = glm::rotate(M, glm::radians(r.x), glm::vec3(1, 0, 0));
    M = glm::rotate(M, glm::radians(r.y), glm::vec3(0, 1, 0));
    M = glm::rotate(M, glm::radians(r.z), glm::vec3(0, 0, 1));

    M = glm::scale(M, s);
}
 
// Perform a deep copy of one tree to another
void deepCopyTree(std::shared_ptr<HierachicalShape> node, std::shared_ptr<HierachicalShape> refnode)
{
    node->M = refnode->M;
    node->Minv = refnode->Minv;
    node->type = refnode->type;
    node->name = refnode->name;
    node->materials = refnode->materials;
    for (int i = 0; i < refnode->children.size(); i++) 
    {
        if (refnode->children[i]->type == "node")
        {
            std::shared_ptr<HierachicalShape> refChild = std::dynamic_pointer_cast<HierachicalShape>(refnode->children[i]);
            std::shared_ptr<HierachicalShape> child = std::make_shared<HierachicalShape>();
            child->M = refChild->M;
            child->Minv = refChild->Minv;
            child->name = refChild->name;
            child->type = refChild->type;
            node->children.push_back(child);
            deepCopyTree(child, refChild);
        }
        else if (refnode->children[i]->type == "sphere")
        {
            std::shared_ptr<Sphere> refChild = std::dynamic_pointer_cast<Sphere>(refnode->children[i]);
            std::shared_ptr<Sphere> child = std::make_shared<Sphere>(refChild->radius, refChild->center);
            child->name = refChild->name;
            child->type = refChild->type;
            child->radius = refChild->radius;
            child->center = refChild->center;
            child->materials.push_back(refChild->materials[0]);
            node->children.push_back(child);
        }
        else if (refnode->children[i]->type == "metaball")
        {
            std::shared_ptr<MetaballPair> refChild = std::dynamic_pointer_cast<MetaballPair>(refnode->children[i]);
            std::shared_ptr<MetaballPair> child = std::make_shared<MetaballPair>(refChild->radius, refChild->center1, refChild->center2);
            child->name = refChild->name;
            child->type = refChild->type;
            child->radius = refChild->radius;
            child->center1 = refChild->center1;
            child->center2 = refChild->center2;
            child->materials.push_back(refChild->materials[0]);
            node->children.push_back(child);
        }
        else if (refnode->children[i]->type == "plane")
        {
            std::shared_ptr<Plane> refChild = std::dynamic_pointer_cast<Plane>(refnode->children[i]);
            std::shared_ptr<Plane> child = std::make_shared<Plane>(refChild->normal, refChild->position);
            child->name = refChild->name;
            child->type = refChild->type;
            child->normal = refChild->normal;
            child->position = refChild->position;
            child->materials.push_back(refChild->materials[0]);
            node->children.push_back(child);
        }
        else if (refnode->children[i]->type == "mesh")
        {
            std::shared_ptr<Mesh> refChild = std::dynamic_pointer_cast<Mesh>(refnode->children[i]);
            std::shared_ptr<Mesh> child = std::make_shared<Mesh>(refChild->filepath);
            child->name = refChild->name;
            child->type = refChild->type;
            child->materials.push_back(refChild->materials[0]);
            node->children.push_back(child);
        }
        else if (refnode->children[i]->type == "box") 
        {
            std::shared_ptr<AABB> refChild = std::dynamic_pointer_cast<AABB>(refnode->children[i]);
            std::shared_ptr<AABB> child = std::make_shared<AABB>();
            child->name = refChild->name;
            child->type = refChild->type;
            child->minpt = refChild->minpt;
            child->maxpt = refChild->maxpt;
            child->materials.push_back(refChild->materials[0]);
            node->children.push_back(child);
        }
    }
}


void createNodeTree(std::shared_ptr<HierachicalShape> node, json& obj, std::vector<std::shared_ptr<Material>> materials) 
{
    std::string objType = obj["type"];
    std::string objName = obj["name"];

    glm::vec3 position = glm::vec3();
    populateVectorHelper(position, "position", obj);

    glm::vec3 rotation = glm::vec3();
    populateVectorHelper(rotation, "rotation", obj);

    glm::vec3 scale = glm::vec3();
    populateVectorHelper(scale, "scale", obj);

    createModelMatrix(node->M, position, rotation, scale);
    node->Minv = glm::inverse(node->M);

    json nodes = obj["children"];
    for (int i = 0; i < nodes.size(); i++) 
    {
        std::string childType = nodes[i]["type"];
        std::string childName = nodes[i]["name"];
        bool mirrorReflection = false;
        try {
            mirrorReflection = nodes[i]["mirrorReflection"];
        }
        catch (...)
        {

        }
        if (childType == "node")
        {
            std::shared_ptr<HierachicalShape> child = std::make_shared<HierachicalShape>();
            child->type = childType;
            child->name = childName;
            child->mirrorReflection = mirrorReflection;

            std::vector<int> materialIDs = nodes[i]["materials"];
            pairMaterialwithObj(child, materials, materialIDs);

            node->children.push_back(child);

            createNodeTree(child, nodes[i], materials);
        }
        else if (childType == "sphere")
        {
            float radius = obj["radius"];

            std::vector<int> materialIDs = nodes[i]["materials"];

            std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(radius);
            sphere->name = childName;
            sphere->type = childType;
            sphere->mirrorReflection = mirrorReflection;

            pairMaterialwithObj(sphere, materials, materialIDs);

            node->children.push_back(sphere);
        }
        else if (childType == "metaball")
        {
            float radius = obj["radius"];

            std::vector<int> materialIDs = nodes[i]["materials"];

            std::shared_ptr<MetaballPair> metaball = std::make_shared<MetaballPair>(radius);
            metaball->name = childName;
            metaball->type = childType;
            metaball->mirrorReflection = mirrorReflection;

            pairMaterialwithObj(metaball, materials, materialIDs);

            node->children.push_back(metaball);
        }
        else if (childType == "box")
        {
            glm::vec3 minVert = glm::vec3();
            populateVectorHelper(minVert, "min", nodes[i]);

            glm::vec3 maxVert = glm::vec3();
            populateVectorHelper(maxVert, "max", nodes[i]);

            std::vector<int> materialIDs = nodes[i]["materials"];

            std::shared_ptr<AABB> aabb = std::make_shared<AABB>();
            aabb->name = childName;
            aabb->type = childType;
            aabb->mirrorReflection = mirrorReflection;

            aabb->minpt = minVert;
            aabb->maxpt = maxVert;

            pairMaterialwithObj(aabb, materials, materialIDs);

            node->children.push_back(aabb);
        }
        else if (childType == "plane")
        {
            glm::vec3 normal = glm::vec3();
            populateVectorHelper(normal, "normal", obj);

            std::vector<int> materialIDs = nodes[i]["materials"];

            std::shared_ptr<Plane> plane = std::make_shared<Plane>(normal);
            plane->name = childName;
            plane->type = childType;
            plane->mirrorReflection = mirrorReflection;

            pairMaterialwithObj(plane, materials, materialIDs);

            node->children.push_back(plane);
        }
        else if (childType == "mesh")
        {
            std::string filepath = obj["filepath"];

            float s = obj["scale"];

            std::vector<int> materialIDs = nodes[i]["materials"];

            std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(filepath);
            mesh->name = childName;
            mesh->type = childType;
            mesh->mirrorReflection = mirrorReflection;

            pairMaterialwithObj(mesh, materials, materialIDs);

            node->children.push_back(mesh);
        }
    }
}

void Parser::createScene(std::shared_ptr<Scene> scene) 
{
    std::ifstream file(filename);
    if (file.is_open()) {
        json data;
        file >> data;

        // Retrieve the resolution from file, if not specified, then use default vals to init scene
        try {
            int width = data["resolution"][0];
            int height = data["resolution"][1];

            scene->width = width;
            scene->height = height;
            scene->aspect = (float)width / (float)height;
            scene->init();
        }
        catch (...)
        {
            std::cerr << "Could not read resolution, defaulting to 1080x720" << std::endl;

            scene->init();
        }

        // Add the ambient light to the scene
        glm::vec3 ambient = glm::vec3();
        populateVectorHelper(ambient, "ambient", data);

        scene->ambient = ambient;

        scene->jitter = data["AA"]["jitter"];
        scene->samples = data["AA"]["samples"];

        try {
            scene->dofBlur = data["DOFBlur"]["DOF"];
            scene->dofJitter = data["DOFBlur"]["DOFJitter"];
            scene->dofSamples = data["DOFBlur"]["DOFSamples"];
            scene->dofVariation = data["DOFBlur"]["DOFVariation"];
            scene->focalLength = data["DOFBlur"]["focalLength"];
        } 
        catch (...)
        {
            std::cerr << "Could not read depth of field blur, defaulting to disabled" << std::endl;
        }

        // Process the camera
        glm::vec3 camPosition = glm::vec3();
        populateVectorHelper(camPosition, "camera", "position", data);

        glm::vec3 lookAt = glm::vec3();
        populateVectorHelper(lookAt, "camera", "lookAt", data);

        glm::vec3 up = glm::vec3();
        populateVectorHelper(up, "camera", "up", data);

        float fov = data["camera"]["fov"];

        scene->cam->position = camPosition;
        scene->cam->lookAt = lookAt;
        scene->cam->up = up;
        scene->cam->fovy = fov;

        // Process the materials
        std::vector<std::shared_ptr<Material>> materials;
        json mats = data["materials"];
        for (json& mat : mats) 
        {
            std::string matName = mat["name"];
            int matID = mat["ID"];

            glm::vec3 diffuse = glm::vec3();
            populateVectorHelper(diffuse, "diffuse", mat);

            glm::vec3 specular = glm::vec3();
            populateVectorHelper(specular, "specular", mat);

            float hardness = mat["hardness"];

            std::shared_ptr<Material> material = std::make_shared<Material>();
            material->diffuse = diffuse;
            material->specular = specular;
            material->ID = matID;
            material->name = matName;
            material->hardness = hardness;

            materials.push_back(material);
        }

        std::vector<std::shared_ptr<HierachicalShape>> roots;
        std::vector<std::string> rootsNames;

        // Process the scene objects
        json objs = data["objects"];
        for (json& obj : objs) 
        {
            std::string objType = obj["type"];
            std::string objName = obj["name"];
            bool mirrorReflection = false;
            float mirrorReflectionIntensity = 1.0f;
            try {
                mirrorReflection = obj["mirrorReflection"];
                mirrorReflectionIntensity =obj["mirrorReflectionIntensity"];
            }
            catch (...)
            {

            }

            glm::vec3 objPosition = glm::vec3();
            populateVectorHelper(objPosition, "position", obj);

            std::vector<int> materialIDs = obj["materials"];

            // Create object and add it to the scene
            if (objType == "sphere")
            {
                float radius = obj["radius"];
                std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(radius, objPosition);
                sphere->name = objName;
                sphere->type = objType;
                sphere->mirrorReflection = mirrorReflection;
                sphere->mirrorReflectionIntensity = mirrorReflectionIntensity;

                pairMaterialwithObj(sphere, materials, materialIDs);

                scene->addObject(sphere);
            }
            else if (objType == "metaball")
            {
                float radius = obj["radius"];
                populateVectorHelper(objPosition, "position", obj);
                glm::vec3 objPosition2 = glm::vec3();
                populateVectorHelper(objPosition2, "position2", obj);

                std::shared_ptr<MetaballPair> metaball = std::make_shared<MetaballPair>(radius, objPosition, objPosition2);
                metaball->name = objName;
                metaball->type = objType;
                metaball->mirrorReflection = mirrorReflection;
                metaball->mirrorReflectionIntensity = mirrorReflectionIntensity;

                pairMaterialwithObj(metaball, materials, materialIDs);

                scene->addObject(metaball);
            }
            else if (objType == "box")
            {
                glm::vec3 size = glm::vec3();
                populateVectorHelper(size, "size", obj);

                glm::vec3 rotation = glm::vec3();
                populateVectorHelper(rotation, "rotation", obj);

                std::shared_ptr<AABB> aabb = std::make_shared<AABB>(size, objPosition);
                aabb->name = objName;
                aabb->type = objType;
                aabb->mirrorReflection = mirrorReflection;
                aabb->mirrorReflectionIntensity = mirrorReflectionIntensity;

                pairMaterialwithObj(aabb, materials, materialIDs);

                scene->addObject(aabb);
            }
            else if (objType == "plane")
            {
                glm::vec3 normal = glm::vec3();
                populateVectorHelper(normal, "normal", obj);

                std::shared_ptr<Plane> plane = std::make_shared<Plane>(normal, objPosition);
                plane->name = objName;
                plane->type = objType;
                plane->mirrorReflection = mirrorReflection;
                plane->mirrorReflectionIntensity = mirrorReflectionIntensity;

                pairMaterialwithObj(plane, materials, materialIDs);

                scene->addObject(plane);
            }
            else if (objType == "mesh")
            {
                std::string filepath = obj["filepath"];

                float s = obj["scale"];

                std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(filepath, objPosition);
                mesh->name = objName;
                mesh->type = objType;
                mesh->mirrorReflection = mirrorReflection;
                mesh->mirrorReflectionIntensity = mirrorReflectionIntensity;

                mesh->scale(s);

                pairMaterialwithObj(mesh, materials, materialIDs);

                scene->addObject(mesh);
            }
            else if (objType == "node")
            {
                std::string ref = obj["ref"];
                if (ref == "") 
                {
                    rootsNames.push_back(objName);

                    std::shared_ptr<HierachicalShape> node = std::make_shared<HierachicalShape>();
                    createNodeTree(node, obj, materials);
                                        
                    roots.push_back(node);

                    // Simply add the root node to the scene's objects
                    scene->addObject(node);
                }
                else // node obj has reference to an other one 
                {
                    int idx = -1;
                    // Assume the reference has already been created
                    for (int i = 0; i < roots.size(); i++) 
                    {
                        if (ref == rootsNames[i]) 
                        {
                            idx = i;
                            break;
                        }
                    }

                    if (idx > -1) 
                    {
                        std::shared_ptr<HierachicalShape> node = std::make_shared<HierachicalShape>();
                        deepCopyTree(node, roots[idx]);

                        glm::vec3 position = glm::vec3();
                        populateVectorHelper(position, "position", obj);

                        glm::vec3 rotation = glm::vec3();
                        populateVectorHelper(rotation, "rotation", obj);

                        glm::vec3 scale = glm::vec3();
                        populateVectorHelper(scale, "scale", obj);

                        node->M = glm::mat4(1.0f);
                        createModelMatrix(node->M, position, rotation, scale);
                        node->Minv = glm::inverse(node->M);

                        scene->addObject(node);
                    }
                    else 
                    {
                        std::cerr << "Could not find node reference, skipping its creation" << std::endl;
                        continue;
                    }
                }
            }
            else 
            {
                std::cerr << "Unknown object type, skipping its creation" << std::endl;
                continue;
            }
        }

        // Process the scene lights
        json lights = data["lights"];
        for (json& light : lights) 
        {
            std::string lightType = light["type"];
            std::string lightName = light["name"];

            glm::vec3 colour = glm::vec3();
            populateVectorHelper(colour, "colour", light);

            // Create light and add it to the scene
            if (lightType == "point")
            {
                glm::vec3 position = glm::vec3();
                populateVectorHelper(position, "position", light);

                float power = light["power"];

                std::shared_ptr<Light> light = std::make_shared<Light>();
                light->position = position;
                light->colour = colour;
                light->power = power;

                scene->addLight(light);
            }
            else if (lightType == "directional")
            {
                glm::vec3 direction = glm::vec3();
                populateVectorHelper(direction, "direction", light);

                std::shared_ptr<Light> light = std::make_shared<Light>();
                light->position = direction;
                light->colour = colour;
                light->power = 1.0; // Directional light has no distance fall off
                light->type = "directional";

                scene->addLight(light);
            }
            else 
            {
                std::cerr << "Unknown light type, skipping its creation" << std::endl;
                continue;
            }
        }
    }
}

void Parser::setJSONfilename(const std::string _filename) 
{
    filename = _filename;
}