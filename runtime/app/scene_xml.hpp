#pragma once

#include <string>
#include <unordered_map>

namespace PWEngine::App
{
    class XMLNode
    {
        std::string node_name;
        std::unordered_map<std::string, XMLNode*> sub_node;
        
    };
}