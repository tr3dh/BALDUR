#pragma once

#include "../Objects/IObject.h"

struct KeywordRegister{

    KeywordRegister() = default;

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
};