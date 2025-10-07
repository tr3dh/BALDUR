#pragma once

#include "../Objects/IObject.h"

struct KeywordRegister{

    KeywordRegister() = default;

    ~KeywordRegister(){

        for(auto& [kw, object] : keywordObjects){

            delete object;
        }

        keywordObjects.clear();
    }

    void registerKeyword(const std::string& keyword, IObject* object){
        
        RETURNING_ASSERT(!keywordObjects.contains(keyword), "Keyword bereits vorhanden",);
        keywordObjects.try_emplace(keyword, object);
    }

    bool contains(const std::string& keyword){
        return keywordObjects.contains(keyword);
    }

    IObject* constructFromKeyword(const std::string& keyword){

        RETURNING_ASSERT(keywordObjects.contains(keyword), "Keyword nicht vorhanden", nullptr);
        return keywordObjects[keyword];
    }

    std::map<std::string, IObject*> keywordObjects = {};

    friend std::ostream& operator<<(std::ostream& os, const KeywordRegister& reg){

        LOG << "Register mit " << reg.keywordObjects.size() << " registrierten Keywords" << endl;

        for(const auto& [kw, obj] : reg.keywordObjects){
            
            LOG << "Key : " << kw << " >> IObject Ptr with tID " << obj->getTypeIndex(); 
        }

        return os;
    }
};