// TransformWidget.h
#include <Core/Common/EditorCommon.h>
#include <Core/Editor/CommandHistory/CommandHistory.h>

namespace Isle
{
    class Editor::TransformWidget : public EditorComponent
    {
    private:
        Transform m_TransformBeforeEdit;
        bool m_IsEditing = false;

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "Transform Widget"; }

    public:
        ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_GizmoMode = ImGuizmo::WORLD;
    };

    void Editor::TransformWidget::Start()
    {
    }

    void Editor::TransformWidget::Update()
    {
        auto* editor = Editor::Instance();
        auto& selectedComponents = editor->GetSelectedComponents();

        if (selectedComponents.empty())
            return;

        SceneComponent* comp = selectedComponents[0];

        auto camera = MainCamera::Instance()->GetCamera();
        if (!camera) return;

        glm::mat4 view = camera->m_ViewMatrix;
        glm::mat4 proj = camera->m_ProjectionMatrix;

        auto viewport = editor->m_Viewport;
        if (!viewport) return;

        ImVec2 vpPos = viewport->GetPosition();
        ImVec2 vpSize = viewport->GetSize();
        if (vpSize.x <= 0.0f || vpSize.y <= 0.0f) return;

        auto tex = viewport->GetViewportTexture();
        if (!tex) return;

        float texAspect = (float)tex->m_Width / (float)tex->m_Height;
        float winAspect = vpSize.x / vpSize.y;
        ImVec2 displaySize = vpSize;
        if (texAspect > winAspect)
            displaySize.y = vpSize.x / texAspect;
        else
            displaySize.x = vpSize.y * texAspect;

        ImVec2 offset;
        offset.x = (vpSize.x - displaySize.x) * 0.5f;
        offset.y = (vpSize.y - displaySize.y) * 0.5f;

        ImVec2 gizmoPos = ImVec2(vpPos.x + offset.x, vpPos.y + offset.y);

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
        ImGuizmo::SetRect(gizmoPos.x, gizmoPos.y, displaySize.x, displaySize.y);

        glm::mat4 transform = glm::mat4(1.0f);
        if (m_GizmoMode == ImGuizmo::LOCAL)
            transform = comp->GetLocalMatrix();
        else
            transform = comp->GetWorldMatrix();

        if (ImGuizmo::IsUsing() && !m_IsEditing)
        {
            m_IsEditing = true;
            m_TransformBeforeEdit = comp->m_Transform;
        }

        if (ImGuizmo::Manipulate(glm::value_ptr(view),
            glm::value_ptr(proj),
            m_GizmoOperation,
            m_GizmoMode,
            glm::value_ptr(transform)))
        {
            glm::vec3 translation, scale, skew;
            glm::quat rotation;
            glm::vec4 perspective;
            glm::decompose(transform, scale, rotation, translation, skew, perspective);

            // Calculate delta transform
            glm::vec3 deltaPos = translation - comp->m_Transform.m_Translation;
            glm::quat deltaRot = rotation * glm::inverse(comp->m_Transform.m_Rotation);
            glm::vec3 deltaScale = scale / comp->m_Transform.m_Scale;

            // Apply to all selected components
            for (auto* selectedComp : selectedComponents)
            {
                if (m_GizmoMode == ImGuizmo::MODE::WORLD)
                {
                    selectedComp->SetWorldPosition(selectedComp->GetWorldPosition() + deltaPos);
                    selectedComp->SetWorldRotation(deltaRot * selectedComp->GetWorldRotation());
                    selectedComp->SetWorldScale(selectedComp->GetWorldScale() * deltaScale);
                }
                else if (m_GizmoMode == ImGuizmo::MODE::LOCAL)
                {
                    selectedComp->SetLocalPosition(selectedComp->m_Transform.m_Translation + deltaPos);
                    selectedComp->SetLocalRotation(deltaRot * selectedComp->m_Transform.m_Rotation);
                    selectedComp->SetLocalScale(selectedComp->m_Transform.m_Scale * deltaScale);
                }
            }

            // Update reference
            if (m_GizmoMode == ImGuizmo::MODE::WORLD)
            {
                comp->SetWorldPosition(translation);
                comp->SetWorldRotation(rotation);
                comp->SetWorldScale(scale);
            }
            else
            {
                comp->SetLocalPosition(translation);
                comp->SetLocalRotation(rotation);
                comp->SetLocalScale(scale);
            }
        }

        if (!ImGuizmo::IsUsing() && m_IsEditing)
        {
            m_IsEditing = false;

            Transform currentTransform = comp->m_Transform;
            if (m_TransformBeforeEdit.m_Translation != currentTransform.m_Translation ||
                m_TransformBeforeEdit.m_Rotation != currentTransform.m_Rotation ||
                m_TransformBeforeEdit.m_Scale != currentTransform.m_Scale)
            {
                auto cmd = std::make_unique<TransformCommand>(
                    comp,
                    m_TransformBeforeEdit,
                    currentTransform,
                    m_GizmoMode == ImGuizmo::LOCAL
                );

                editor->m_Commands->ExecuteCommand(std::move(cmd));
            }
        }
    }

    void Editor::TransformWidget::Destroy()
    {
    }
}