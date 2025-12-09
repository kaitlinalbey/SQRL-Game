#include "ObjectFactory.h"
#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "ControllerComponent.h"
#include "BehaviorComponent.h"
#include <tinyxml2.h>
#include <iostream>

ObjectFactory& ObjectFactory::instance() {
    static ObjectFactory inst;
    return inst;
}

void ObjectFactory::registerType(const std::string& type, CreateFunc func) {
    creators_[type] = func;
}

std::unique_ptr<GameObject> ObjectFactory::create(const std::string& type, const ObjectParams& params) {
    auto it = creators_.find(type);
    if (it != creators_.end()) {
        return it->second(params);
    }
    std::cerr << "Unknown object type: " << type << "\n";
    return nullptr;
}

bool ObjectFactory::loadFromXML(const std::string& filepath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filepath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load ObjectFactory XML: " << filepath << "\n";
        return false;
    }
    
    auto* root = doc.RootElement();
    if (!root) return false;
    
    for (auto* objDef = root->FirstChildElement("Object"); objDef; objDef = objDef->NextSiblingElement("Object")) {
        const char* typeName = objDef->Attribute("type");
        if (!typeName) continue;
        
        std::cout << "Loaded object definition: " << typeName << "\n";
        
        // Parse components and their attributes
        for (auto* compDef = objDef->FirstChildElement("Component"); compDef; compDef = compDef->NextSiblingElement("Component")) {
            const char* compType = compDef->Attribute("type");
            if (compType) {
                std::cout << "  - Component: " << compType;
                
                // Log component parameters
                if (compDef->Attribute("x")) std::cout << " x=" << compDef->Attribute("x");
                if (compDef->Attribute("y")) std::cout << " y=" << compDef->Attribute("y");
                if (compDef->Attribute("width")) std::cout << " width=" << compDef->Attribute("width");
                if (compDef->Attribute("height")) std::cout << " height=" << compDef->Attribute("height");
                if (compDef->Attribute("speed")) std::cout << " speed=" << compDef->Attribute("speed");
                if (compDef->Attribute("texture")) std::cout << " texture=" << compDef->Attribute("texture");
                
                std::cout << "\n";
            }
        }
    }
    
    return true;
}
