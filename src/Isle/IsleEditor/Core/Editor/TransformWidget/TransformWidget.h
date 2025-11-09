// TransformWidget.h
#include <Core/Common/EditorCommon.h>

namespace Isle
{
    class Editor::TransformWidget : public EditorComponent
    {
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
        SceneComponent* comp = dynamic_cast<SceneComponent*>(editor->m_SelectedComponent);
        if (!comp)
            return;

        auto camera = MainCamera::Instance()->GetCamera();
        glm::mat4 view = camera->m_ViewMatrix;
        glm::mat4 proj = camera->m_ProjectionMatrix;

        auto viewport = editor->m_Viewport;
        if (!viewport)
            return;

        ImVec2 vpPos = viewport->GetPosition();
        ImVec2 vpSize = viewport->GetSize();
        if (vpSize.x <= 0.0f || vpSize.y <= 0.0f)
            return;

        auto tex = viewport->GetViewportTexture();
        if (!tex)
            return;

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

            if (m_GizmoMode == ImGuizmo::MODE::WORLD)
            {
                comp->SetWorldPosition(translation);
                comp->SetWorldRotation(rotation);
                comp->SetWorldScale(scale);
            }
            else if (m_GizmoMode == ImGuizmo::MODE::LOCAL)
            {
                comp->SetLocalPosition(translation);
                comp->SetLocalRotation(rotation);
                comp->SetLocalScale(scale);
            }
        }
    }

    void Editor::TransformWidget::Destroy()
    {
    }
}