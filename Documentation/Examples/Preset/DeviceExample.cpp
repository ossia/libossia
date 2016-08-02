#include <iostream>
#include <string>
#include <memory>
#include <fstream>

#include <preset/preset.hpp>
#include <preset/exception.hpp>
#include "Network/Protocol/Local.h"
#include "Network/Device.h"
#include "Network/Address.h"
#include "Network/Node.h"

void printValue(const OSSIA::Value *v);

void printValueCallback(const OSSIA::Value * v) {
    printValue(v);
    std::cout << std::endl;
}

void printValue(const OSSIA::Value * v)
{
    switch (v->getType())
    {
        case OSSIA::Value::Type::IMPULSE :
        {
            std::cout << "Impulse -";
            break;
        }
        case OSSIA::Value::Type::BOOL :
        {
            OSSIA::Bool * b = (OSSIA::Bool*)v;
            std::cout << "Bool " << b->value;
            break;
        }
        case OSSIA::Value::Type::INT :
        {
            OSSIA::Int * i = (OSSIA::Int*)v;
            std::cout << "Int " << i->value;
            break;
        }
        case OSSIA::Value::Type::FLOAT :
        {
            OSSIA::Float * f = (OSSIA::Float*)v;
            std::cout << "Float " << f->value;
            break;
        }
        case OSSIA::Value::Type::CHAR :
        {
            OSSIA::Char * c = (OSSIA::Char*)v;
            std::cout << "Char " << c->value;
            break;
        }
        case OSSIA::Value::Type::STRING :
        {
            OSSIA::String * s = (OSSIA::String*)v;
            std::cout << "String " << s->value;
            break;
        }
        case OSSIA::Value::Type::DESTINATION :
        {
            OSSIA::Destination * d = (OSSIA::Destination*)v;
            std::cout << "Destination " << d->value;
            break;
        }
        case OSSIA::Value::Type::TUPLE :
        {
            OSSIA::Tuple * t = (OSSIA::Tuple*)v;
            std::cout << "Tuple [";
            bool first = true;
            for (const auto & e : t->value)
            {
                if (!first) std::cout << ", ";
                printValue(e);
                first = false;
            }
            std::cout << "]";
            break;
        }
        case OSSIA::Value::Type::GENERIC :
        {
            // todo
            break;
        }
        default:
            break;
    }
}

void printNode(std::shared_ptr<OSSIA::Node> node, int padding) {
    for (int i = 0; i < padding; ++i) {
        std::cout << "   |";
    }
    std::cout << node->getName();
    auto children = node->children();
    if (children.size() == 0) {
        std::cout << ": ";
        printValue(node->getAddress()->getValue());
    }
    std::cout << std::endl;
    for (auto child = children.begin(); child != children.end(); ++child) {
        printNode((*child), padding + 1);
    }
}

void printDevice(std::shared_ptr<OSSIA::Device> dev) {
    std::cout << "---------------------------------" << std::endl;
    auto children = dev->children();
    if (children.size() == 0) {
        std::cout << "Device is empty" << std::endl;
    }
    else {
        std::cout << dev->getName() << std::endl;
        for (auto child = children.begin(); child != children.end(); ++child) {
            printNode((*child), 1);
        }
    }
    std::cout << "---------------------------------" << std::endl;
}

void printPreset(ossia::presets::Preset preset) {
    for (auto pp = preset.begin(); pp != preset.end(); ++pp) {
        const OSSIA::Value* val = pp->second;
        if (val) {
            std::cout << pp->first << ": ";
            printValue(val);
            std::cout << std::endl;
        }
    }
}

int main(int argc, char** argv) {

    auto localProtocol = OSSIA::Local::create();
    auto localDevice = OSSIA::Device::create(localProtocol, "device");

    if (argc == 1) {

        std::cout << "No JSON specified, running default example" << std::endl << std::endl;

        std::string json = R"_({"scene":{"cube":{"position":{"x":0,"y":12,"z":5}, "rotation":{"y":0.3}}, "scenetitle":"La scène a un titre !"}})_";
        try {
            ossia::presets::Preset p = ossia::presets::read_json(json);

            auto scenenode = *(localDevice->emplace(localDevice->children().cend(), "scene"));
            auto cubenode = *(scenenode->emplace(scenenode->children().cend(), "cube"));
            auto cubeposnode = *(cubenode->emplace(cubenode->children().cend(), "position"));

            auto cubexpnode = *(cubeposnode->emplace(cubeposnode->children().cend(), "x"));
            auto cubexpaddr = cubexpnode->createAddress(OSSIA::Value::Type::INT);

            auto cubeypnode = *(cubeposnode->emplace(cubeposnode->children().cend(), "y"));
            auto cubeypaddr = cubeypnode->createAddress(OSSIA::Value::Type::INT);

            auto cubezpnode = *(cubeposnode->emplace(cubeposnode->children().cend(), "z"));
            auto cubezpaddr = cubezpnode->createAddress(OSSIA::Value::Type::INT);

            auto cuberotnode = *(cubenode->emplace(cubenode->children().cend(), "rotation"));

            auto cubexrnode = *(cuberotnode->emplace(cuberotnode->children().cend(), "x"));
            auto cubexraddr = cubexrnode->createAddress(OSSIA::Value::Type::FLOAT);

            auto cubeyrnode = *(cuberotnode->emplace(cuberotnode->children().cend(), "y"));
            auto cubeyraddr = cubeyrnode->createAddress(OSSIA::Value::Type::FLOAT);

            auto cubezrnode = *(cuberotnode->emplace(cuberotnode->children().cend(), "z"));
            auto cubezraddr = cubezrnode->createAddress(OSSIA::Value::Type::FLOAT);

            OSSIA::Int i (-1);
            cubexpaddr->pushValue(&i);
            cubeypaddr->pushValue(&i);
            cubezpaddr->pushValue(&i);

            OSSIA::Float f (-1.0f);
            cubexraddr->pushValue(&f);
            cubeyraddr->pushValue(&f);
            cubezraddr->pushValue(&f);

            printDevice(localDevice);
            std::cout << "Applying preset" << std::endl;
            ossia::devices::apply_preset(localDevice, p);
            printDevice(localDevice);

            ossia::presets::Preset preset = ossia::devices::make_preset(*localDevice);
            printPreset(preset);
            std::string outjson = ossia::presets::write_json(preset);
            std::cout << outjson << std::endl;
        }
        catch (ossia::ossiaException prexc) {
            std::cout << std::string (prexc.what()) << std::endl;
        }
        catch (std::exception e) {
            std::cout << std::string (e.what()) << std::endl;
        }
        catch (...) {
            std::cout << "oups :/" << std::endl;
        }
    }

    else {
        std::string filename = argv[1];
        std::ifstream ifs (filename);
        std::string json;
        json.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

        ossia::presets::Preset preset;

        try {
            std::cout << "Creating preset from JSON...";
            preset = ossia::presets::read_json(json);
            std::cout << "done." << std::endl;
            printPreset(preset);
            std::cout << "Building device from preset...";
            ossia::devices::apply_preset(localDevice, preset, ossia::devices::keep_arch_off);
            std::cout << "done." <<std::endl;
        }
        catch (ossia::ossiaException_InvalidJSON prexc) {
            std::cout << "Specified file is invalid JSON" << std::endl;
        }
        catch (std::exception e) {
            std::cout << e.what() << std::endl;
        }

        std::cout << "Resulting device: " << std::endl;
        printDevice(localDevice);

        std::cout << "Converting device to preset...";
        ossia::presets::Preset buildpreset;

        try {
            buildpreset = ossia::devices::make_preset(*localDevice);
            std::cout << "done." << std::endl;
        }
        catch (std::exception e) {
            std::cout << e.what() << std::endl;
        }

        std::cout << "Resulting preset: " << std::endl;
        printPreset(buildpreset);

        std::cout << ossia::presets::write_json(buildpreset) << std::endl;
    }

    return EXIT_SUCCESS;
}
