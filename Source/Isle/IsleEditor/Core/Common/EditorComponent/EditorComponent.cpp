// EditorComponent.cpp
#include "EditorComponent.h"

namespace Isle
{
    void EditorComponent::SetDockConstraints(const DockConstraints& constraints)
    {
        m_DockConstraints = constraints;
    }

    DockConstraints& EditorComponent::GetDockConstraints()
    {
        return m_DockConstraints;
    }

    const DockConstraints& EditorComponent::GetDockConstraints() const
    {
        return m_DockConstraints;
    }

    ImVec2 EditorComponent::GetPosition()
    {
        return m_CalculatedPosition;
    }

    void EditorComponent::SetPosition(ImVec2 position)
    {
        m_Position = position;
    }

    ImVec2 EditorComponent::GetSize()
    {
        return m_CalculatedSize;
    }

    void EditorComponent::SetSize(ImVec2 size)
    {
        m_Size = size;
    }

    bool EditorComponent::IsOpen()
    {
        return m_IsOpen;
    }

    void EditorComponent::SetOpen(bool open)
    {
        m_IsOpen = open;
    }

    void EditorComponent::SetCalculatedBounds(ImVec2 pos, ImVec2 size)
    {
        m_CalculatedPosition = pos;
        m_CalculatedSize = size;
    }
}