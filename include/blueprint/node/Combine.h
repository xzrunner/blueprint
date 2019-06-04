#pragma once

#include "blueprint/Node.h"
#include "blueprint/Pin.h"

namespace bp
{
namespace node
{

class Combine : public Node
{
public:
    Combine()
        : Node("Combine")
    {
        AddPin(std::make_shared<Pin>(true, ID_R, PIN_FLOAT1, "R", *this));
        AddPin(std::make_shared<Pin>(true, ID_G, PIN_FLOAT1, "G", *this));
        AddPin(std::make_shared<Pin>(true, ID_B, PIN_FLOAT1, "B", *this));
        AddPin(std::make_shared<Pin>(true, ID_A, PIN_FLOAT1, "A", *this));

        AddPin(std::make_shared<Pin>(false, ID_RGBA, PIN_FLOAT4, "RGBA", *this));
        AddPin(std::make_shared<Pin>(false, ID_RGB,  PIN_FLOAT3, "RGB",  *this));
        AddPin(std::make_shared<Pin>(false, ID_RG,   PIN_FLOAT2, "RG",   *this));

        Layout();
    }

    enum InputID
    {
        ID_R = 0,
        ID_G,
        ID_B,
        ID_A,
    };

    enum OutputID
    {
        ID_RGBA = 0,
        ID_RGB,
        ID_RG,
    };

    RTTR_ENABLE(Node)

}; // Combine

}
}