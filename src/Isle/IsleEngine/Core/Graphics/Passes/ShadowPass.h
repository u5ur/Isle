// ShadowPass.h
#pragma once
#include <Core/Graphics/Passes/Pass.h>

namespace Isle
{
    class ShadowPass : public Pass
    {
    private:
        int m_Size = 2048;

    public:
        virtual void Bind() override;
        virtual void Unbind() override;
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        int GetSize();
        void SetSize(int size);
    };
}

