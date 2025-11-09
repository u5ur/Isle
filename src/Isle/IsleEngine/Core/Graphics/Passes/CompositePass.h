// CompositePass.h
#pragma once
#include <Core/Graphics/Passes/Pass.h>

namespace Isle
{
    class CompositePass : public Pass
    {
    public:
        virtual void Bind() override;
        virtual void Unbind() override;
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        
        Ref<Texture> GetOutputTexture();
        void BlitToScreen();
    };
}