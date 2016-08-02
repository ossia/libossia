#include <iostream>
#include <fstream>
#include <preset/preset.hpp>
#include "Editor/Value.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <string>
#include <sstream>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>

std::string valueToString(OSSIA::Value* val) {

    std::stringstream ss;

    switch (val->getType()) {
    case OSSIA::Value::Type::BOOL: {
        OSSIA::Bool* v = (OSSIA::Bool*)val;
        ss << "Bool " << v->value;
    }
        break;
    case OSSIA::Value::Type::INT: {
        OSSIA::Int* v = (OSSIA::Int*)val;
        ss << "Int " << v->value;
    }
        break;
    case OSSIA::Value::Type::STRING: {
        OSSIA::String* v = (OSSIA::String*)val;
        ss << "String " << v->value;
    }
        break;
    case OSSIA::Value::Type::FLOAT: {
        OSSIA::Float* v = (OSSIA::Float*)val;
        ss << "Float " << v->value;
    }
        break;
    case OSSIA::Value::Type::TUPLE: {
        OSSIA::Tuple* v = (OSSIA::Tuple*)val;
        ss << "Tuple [";
        for (auto ittuple = v->value.begin(); ittuple != v->value.end(); ++ittuple) {
            ss << valueToString(*ittuple);
            if (std::next(ittuple) != v->value.end())
                ss << ", ";
        }
        ss << "]";
    }
        break;
    }
    return ss.str();
}

void printPreset(ossia::presets::Preset preset) {
    for (auto pp = preset.begin(); pp != preset.end(); ++pp) {
        OSSIA::Value* val = pp->second;
        if (val) {
            std::cout << pp->first << ": ";
            std::cout << valueToString(val) << std::endl;
        }
    }
}

int main(int argc, char ** argv) {

    std::string filename = "../Tests/json/test1.json";
    if (argc >= 2) {
        filename = argv[1];
    }
    else {
        std::cout << "No file specified, running example on default test file " << filename << std::endl;
    }

    std::ifstream ifs (filename);

    std::string json;
    json.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    std::cout << "building preset" << std::endl;
    ossia::presets::Preset preset;
    try {
        preset = ossia::presets::read_json(std::string(json));
        printPreset(preset);

        std::cout << "building json from preset" << std::endl;
        std::string resultjson = ossia::presets::write_json(preset);
        std::cout << resultjson << std::endl;
    }
    catch (ossia::ossiaException prexc){
        std::cout << prexc.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
