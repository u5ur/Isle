// Viewport.h
#pragma once
#include <IsleEngine.h>
#include <Core/Common/EditorCommon.h>
#include <Core/ModuleManager/ModuleManager.h>
#include <Core/Camera/MainCamera.h>
#include <Core/Scene/Scene.h>

namespace Isle
{
    class Editor::Viewport : public EditorComponent
    {
    private:
        Ref<Texture> m_ViewportTexture;
        glm::vec2 m_LastViewportSize = { 0, 0 };

    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "Viewport"; }

        Ref<Texture> GetViewportTexture();
        void SetViewportTexture(Ref<Texture> texture);

        void HandleSelection();
        ImVec2 WorldToScreen(const glm::vec3& worldPos);
    };

    void Editor::Viewport::Start()
    {

    }

    void Editor::Viewport::Update()
    {
        m_ViewportTexture = Render::Instance()->GetOutputTexture();

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImVec2 startPos = ImGui::GetCursorScreenPos();

        if (m_ViewportTexture && m_ViewportTexture->m_Id != 0)
        {
            viewportSize = ImGui::GetContentRegionAvail();

            if (m_LastViewportSize.x != viewportSize.x || m_LastViewportSize.y != viewportSize.y)
            {
                m_LastViewportSize = glm::vec2(viewportSize.x, viewportSize.y);
            }

            float textureAspect = (float)m_ViewportTexture->m_Width / (float)m_ViewportTexture->m_Height;
            float windowAspect = viewportSize.x / viewportSize.y;

            ImVec2 displaySize = viewportSize;

            if (textureAspect > windowAspect)
                displaySize.y = viewportSize.x / textureAspect;
            else
                displaySize.x = viewportSize.y * textureAspect;

            ImVec2 pos = ImGui::GetCursorScreenPos();
            pos.x += (viewportSize.x - displaySize.x) * 0.5f;
            pos.y += (viewportSize.y - displaySize.y) * 0.5f;

            ImGui::SetCursorScreenPos(pos);
            ImGui::Image(
                (void*)(intptr_t)m_ViewportTexture->m_Id,
                displaySize,
                ImVec2(0, 1),
                ImVec2(1, 0)
            );

            if (displaySize.x < viewportSize.x || displaySize.y < viewportSize.y)
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRect(
                    ImGui::GetItemRectMin(),
                    ImGui::GetItemRectMax(),
                    IM_COL32(255, 255, 255, 64)
                );
            }
        }
        else
        {
            viewportSize = ImGui::GetContentRegionAvail();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();

            const float gridSize = 20.0f;
            for (int y = 0; y <= viewportSize.y / gridSize; y++)
            {
                for (int x = 0; x <= viewportSize.x / gridSize; x++)
                {
                    bool isDark = (x + y) % 2 == 0;
                    ImVec2 rectMin(pos.x + x * gridSize, pos.y + y * gridSize);
                    ImVec2 rectMax(pos.x + (x + 1) * gridSize, pos.y + (y + 1) * gridSize);

                    drawList->AddRectFilled(
                        rectMin,
                        rectMax,
                        isDark ? IM_COL32(60, 60, 60, 255) : IM_COL32(40, 40, 40, 255)
                    );
                }
            }

            ImVec2 textSize = ImGui::CalcTextSize("No Viewport Available");
            ImVec2 textPos = ImVec2(
                pos.x + (viewportSize.x - textSize.x) * 0.5f,
                pos.y + (viewportSize.y - textSize.y) * 0.5f
            );

            drawList->AddText(textPos, IM_COL32(255, 255, 255, 128), "No Viewport Available");
        }

        ImGui::SetCursorScreenPos(ImVec2(
            ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x - 180.0f,
            ImGui::GetWindowPos().y + 10.0f
        ));

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 40, 40, 200));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(35, 35, 35, 255));

        MainCamera* mainCam = MainCamera::Instance();
        const char* cameraTypeNames[] = { "Orthographic", "Default Perspective", "Editor Perspective" };
        const char* currentTypeName = cameraTypeNames[(int)mainCam->m_Type];

        ImGui::SetNextItemWidth(170.0f);
        if (ImGui::BeginCombo("##CameraType", currentTypeName))
        {
            for (int i = 0; i < IM_ARRAYSIZE(cameraTypeNames); i++)
            {
                bool isSelected = (mainCam->m_Type == (CAMERA_TYPE)i);
                if (ImGui::Selectable(cameraTypeNames[i], isSelected))
                {
                    mainCam->m_Type = (CAMERA_TYPE)i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();

        HandleSelection();
    }


    void Editor::Viewport::Destroy()
    {
        m_ViewportTexture = nullptr;
    }

    Ref<Texture> Editor::Viewport::GetViewportTexture()
    {
        return m_ViewportTexture;
    }

    void Editor::Viewport::SetViewportTexture(Ref<Texture> texture)
    {
        m_ViewportTexture = texture;
    }

    ImVec2 Editor::Viewport::WorldToScreen(const glm::vec3& worldPos)
    {
        MainCamera* cam = MainCamera::Instance();
        glm::mat4 viewProj = cam->GetCamera()->GetViewProjection();

        glm::vec4 clipSpace = viewProj * glm::vec4(worldPos, 1.0f);

        if (clipSpace.w == 0.0f)
            return ImVec2(0, 0);

        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

        if (clipSpace.w < 0.0f)
            return ImVec2(0, 0);

        ImVec2 viewportSize = m_CalculatedSize;
        ImVec2 viewportPos = m_CalculatedPosition;

        if (m_ViewportTexture && m_ViewportTexture->m_Id != 0)
        {
            float textureAspect = (float)m_ViewportTexture->m_Width / (float)m_ViewportTexture->m_Height;
            float windowAspect = viewportSize.x / viewportSize.y;

            ImVec2 displaySize = viewportSize;
            if (textureAspect > windowAspect)
                displaySize.y = viewportSize.x / textureAspect;
            else
                displaySize.x = viewportSize.y * textureAspect;

            ImVec2 offset;
            offset.x = (viewportSize.x - displaySize.x) * 0.5f;
            offset.y = (viewportSize.y - displaySize.y) * 0.5f;

            float screenX = ((ndc.x + 1.0f) * 0.5f) * displaySize.x + offset.x;
            float screenY = ((1.0f - ndc.y) * 0.5f) * displaySize.y + offset.y;

            return ImVec2(viewportPos.x + screenX, viewportPos.y + screenY);
        }

        float screenX = ((ndc.x + 1.0f) * 0.5f) * viewportSize.x;
        float screenY = ((1.0f - ndc.y) * 0.5f) * viewportSize.y;

        return ImVec2(viewportPos.x + screenX, viewportPos.y + screenY);
    }

    bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
        const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& t)
    {
        glm::vec3 invDir = 1.0f / rayDir;
        glm::vec3 t0 = (aabbMin - rayOrigin) * invDir;
        glm::vec3 t1 = (aabbMax - rayOrigin) * invDir;

        glm::vec3 tmin = glm::min(t0, t1);
        glm::vec3 tmax = glm::max(t0, t1);

        float tNear = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
        float tFar = glm::min(glm::min(tmax.x, tmax.y), tmax.z);

        if (tNear > tFar || tFar < 0.0f) return false;

        t = tNear > 0.0f ? tNear : tFar;
        return true;
    }

    bool RayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t)
    {
        const float EPS = 1e-6f;
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 h = glm::cross(rayDir, edge2);
        float a = glm::dot(edge1, h);
        if (fabs(a) < EPS) return false;

        float f = 1.0f / a;
        glm::vec3 s = rayOrigin - v0;
        float u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) return false;

        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(rayDir, q);
        if (v < 0.0f || u + v > 1.0f) return false;

        t = f * glm::dot(edge2, q);
        return t > EPS;
    }

    void Editor::Viewport::HandleSelection()
    {
        if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            return;

        if (ImGuizmo::IsOver() || ImGuizmo::IsUsing()) 
            return;

        ImVec2 viewportPos = m_CalculatedPosition;
        ImVec2 viewportSize = m_CalculatedSize;

        glm::vec2 mousePos = Input::Instance()->GetMousePosition();

        if (mousePos.x < viewportPos.x || mousePos.x > viewportPos.x + viewportSize.x ||
            mousePos.y < viewportPos.y || mousePos.y > viewportPos.y + viewportSize.y)
        {
            return;
        }

        glm::vec2 localMousePos = mousePos - glm::vec2(viewportPos.x, viewportPos.y);

        glm::vec2 ndc;
        ndc.x = (2.0f * localMousePos.x) / viewportSize.x - 1.0f;
        ndc.y = 1.0f - (2.0f * localMousePos.y) / viewportSize.y;

        if (m_ViewportTexture && m_ViewportTexture->m_Id != 0)
        {
            float textureAspect = (float)m_ViewportTexture->m_Width / (float)m_ViewportTexture->m_Height;
            float windowAspect = viewportSize.x / viewportSize.y;

            ImVec2 displaySize = viewportSize;
            if (textureAspect > windowAspect)
                displaySize.y = viewportSize.x / textureAspect;
            else
                displaySize.x = viewportSize.y * textureAspect;

            ImVec2 offset;
            offset.x = (viewportSize.x - displaySize.x) * 0.5f;
            offset.y = (viewportSize.y - displaySize.y) * 0.5f;

            if (localMousePos.x < offset.x || localMousePos.x > offset.x + displaySize.x ||
                localMousePos.y < offset.y || localMousePos.y > offset.y + displaySize.y)
            {
                return;
            }

            localMousePos.x -= offset.x;
            localMousePos.y -= offset.y;

            ndc.x = (2.0f * localMousePos.x) / displaySize.x - 1.0f;
            ndc.y = 1.0f - (2.0f * localMousePos.y) / displaySize.y;
        }

        MainCamera* cam = MainCamera::Instance();
        glm::mat4 invView = glm::inverse(cam->GetCamera()->m_ViewMatrix);
        glm::mat4 invProj = glm::inverse(cam->GetCamera()->m_ProjectionMatrix);

        glm::vec4 rayClip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
        glm::vec4 rayEye = invProj * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

        glm::vec3 rayDir = glm::normalize(glm::vec3(invView * rayEye));
        glm::vec3 rayOrigin = glm::vec3(invView[3]);

        StaticMesh* bestMesh = nullptr;
        float bestDist = FLT_MAX;

        std::function<void(SceneComponent*)> testComponent;
        testComponent = [&](SceneComponent* comp)
            {
                if (!comp)
                    return;

                if (StaticMesh* mesh = dynamic_cast<StaticMesh*>(comp))
                {
                    glm::mat4 world = mesh->GetWorldMatrix();
                    glm::vec3 aabbMin = glm::vec3(world * glm::vec4(mesh->m_Bounds.m_Min, 1.0f));
                    glm::vec3 aabbMax = glm::vec3(world * glm::vec4(mesh->m_Bounds.m_Max, 1.0f));

                    float tAABB;
                    if (!RayIntersectsAABB(rayOrigin, rayDir, aabbMin, aabbMax, tAABB))
                        return;

                    const auto& vertices = mesh->GetVertices();
                    const auto& indices = mesh->GetIndices();

                    for (size_t i = 0; i < indices.size(); i += 3)
                    {
                        glm::vec3 p0 = glm::vec3(world * glm::vec4(vertices[indices[i]].m_Position, 1.0f));
                        glm::vec3 p1 = glm::vec3(world * glm::vec4(vertices[indices[i + 1]].m_Position, 1.0f));
                        glm::vec3 p2 = glm::vec3(world * glm::vec4(vertices[indices[i + 2]].m_Position, 1.0f));

                        float tTri;
                        if (RayIntersectsTriangle(rayOrigin, rayDir, p0, p1, p2, tTri))
                        {
                            if (tTri < bestDist && tTri > 0.0f)
                            {
                                bestDist = tTri;
                                bestMesh = mesh;
                            }
                        }
                    }
                }

                for (auto* child : comp->m_Children)
                    testComponent(child);
            };

        for (auto& component : Scene::Instance()->GetChildren())
        {
            if (!component)
                continue;

            testComponent(component);
        }

        if (bestMesh)
        {
            Editor::Instance()->m_SelectedComponent = bestMesh;
        }
        else
        {
            Editor::Instance()->m_SelectedComponent = nullptr;
        }

    }
}