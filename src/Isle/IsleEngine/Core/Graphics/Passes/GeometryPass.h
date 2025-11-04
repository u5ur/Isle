// GeometryPass.h
#pragma once
#include <Core/Graphics/Passes/Pass.h>

namespace Isle
{
    class GeometryPass : public Pass
    {
    public:
        virtual void Bind() override;
        virtual void Unbind() override;
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
    };
}

