#pragma once
#include <IsleEngine.h>
#include <Core/Common/EditorCommon.h>

namespace Isle
{
    enum class DOCK_SIDE
    {
        NONE,
        TOP,
        BOTTOM,
        LEFT,
        RIGHT,
        FILL
    };

    class EditorComponent;

    struct DockConstraints
    {
        DOCK_SIDE m_Side = DOCK_SIDE::NONE;
        float m_MinSize = 50.0f;
        float m_MaxSize = -1.0f;
        float m_PreferredSize = 300.0f;
        bool m_CanResize = true;
        bool m_CanMove = true;
        bool m_Showtitle = true;
        int m_Priority = 0;
        EditorComponent* m_StackedAbove = nullptr;
        float m_StackRatio = 0.5f;
    };

    class EditorComponent : public Object
    {
    protected:
        ImVec2 m_Position;
        ImVec2 m_Size;
        ImVec2 m_CalculatedPosition;
        ImVec2 m_CalculatedSize;
        bool m_IsOpen = true;
        ImGuiWindowFlags m_WindowFlags = 0;
        DockConstraints m_DockConstraints;

    public:
        virtual void Start() {};
        virtual void Update() {};
        virtual void Destroy() {};
        virtual const char* GetWindowName() const = 0;

        bool IsOpen();
        void SetOpen(bool open);
        ImVec2 GetPosition();
        void SetPosition(ImVec2 position);
        ImVec2 GetSize();
        void SetSize(ImVec2 size);
        void SetDockConstraints(const DockConstraints& constraints);
        DockConstraints& GetDockConstraints();
        const DockConstraints& GetDockConstraints() const;
        void SetCalculatedBounds(ImVec2 pos, ImVec2 size);
    };
}