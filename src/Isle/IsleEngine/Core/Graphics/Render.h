#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/Pipeline/Pipeline.h>

namespace Isle
{
    class Render : public Singleton<Render>, public Component
    {
    private:
        Pipeline* m_Pipeline = nullptr;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;

        const Pipeline* GetPipeline();
    };
}
