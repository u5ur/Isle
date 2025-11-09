// Properties.h
#include <Core/Common/EditorCommon.h>

namespace Isle
{
    class Editor::Properties : public EditorComponent
    {
    public:
        virtual void Start() override;
        virtual void Update() override;
        virtual void Destroy() override;
        virtual const char* GetWindowName() const override { return "Properties"; }

    private:
        void DefaultProperties();
        void MeshProperties();
        void LightProperties();

    };

    void Editor::Properties::Start()
    {

    }

    void Editor::Properties::Update()
    {
        auto component = Editor::Instance()->GetSelectedComponent();
        if (!component)
        {
            ImGui::Text("Select a SceneComponent\n");
            return;
        }

        DefaultProperties();
        MeshProperties();
        LightProperties();
    }

    void Editor::Properties::Destroy()
    {
    }

    void Editor::Properties::DefaultProperties()
    {
        auto component = Editor::Instance()->GetSelectedComponent();
        if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
        {
            std::string name = component->GetName();
            char buffer[256];
            strncpy_s(buffer, name.c_str(), sizeof(buffer));
            if (ImGui::InputText("Name", buffer, sizeof(buffer)))
                component->SetName(buffer);

            ImGui::Text("ID: %d", component->GetId());
        }

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            glm::vec3 pos = component->m_Transform.m_Translation;
            glm::vec3 scl = component->m_Transform.m_Scale;
            glm::vec3 rotEuler = glm::degrees(glm::eulerAngles(component->m_Transform.m_Rotation));

            if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.05f))
                component->m_Transform.m_Translation = pos;

            if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotEuler), 0.1f))
                component->m_Transform.m_Rotation = glm::quat(glm::radians(rotEuler));

            if (ImGui::DragFloat3("Scale", glm::value_ptr(scl), 0.05f))
                component->m_Transform.m_Scale = scl;
        }
    }

    void Editor::Properties::LightProperties()
    {
        Light* light = dynamic_cast<Light*>(Editor::Instance()->GetSelectedComponent());
        if (!light)
            return;

        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(light->m_Color));
            ImGui::SliderFloat("Intensity", &light->m_Intensity, 0.0f, 100.0f, "%.2f");
            ImGui::Checkbox("Cast Shadows", &light->m_CastShadows);

            ImGui::Separator();

            if (auto dirLight = dynamic_cast<DirectionalLight*>(light))
            {
                if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    glm::vec3 direction = dirLight->m_Dir;
                    if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f))
                        dirLight->m_Dir = glm::normalize(direction);

                    if (ImGui::Button("Recalculate Light Space Matrix"))
                        dirLight->GetLightSpaceMatrix();
                }
            }
            else if (auto pointLight = dynamic_cast<PointLight*>(light))
            {
                if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat3("Position", glm::value_ptr(pointLight->m_Position), 0.05f);

                    if (ImGui::SliderFloat("Radius", &pointLight->m_Radius, 0.0f, 200.0f, "%.1f"))
                    {
                        pointLight->UpdateMatrices();
                    }

                    if (ImGui::Button("Rebuild Shadow Matrices"))
                        pointLight->UpdateMatrices();
                }
            }

            // Spot Light
            else if (auto spotLight = dynamic_cast<SpotLight*>(light))
            {
                if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::DragFloat3("Position", glm::value_ptr(spotLight->m_Position), 0.05f);

                    glm::vec3 direction = spotLight->m_Direction;
                    if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f))
                        spotLight->m_Direction = glm::normalize(direction);

                    if (ImGui::SliderFloat("Radius", &spotLight->m_Radius, 0.0f, 200.0f, "%.1f"))
                    {
                        spotLight->UpdateMatrices();
                    }

                    float inner = glm::degrees(spotLight->m_InnerCone);
                    float outer = glm::degrees(spotLight->m_OuterCone);
                    if (ImGui::SliderFloat("Inner Cone", &inner, 1.0f, 89.0f, "%.1f°"))
                        spotLight->m_InnerCone = glm::radians(inner);
                    if (ImGui::SliderFloat("Outer Cone", &outer, inner + 0.1f, 90.0f, "%.1f°"))
                        spotLight->m_OuterCone = glm::radians(outer);

                    if (ImGui::Button("Rebuild Shadow Matrix"))
                        spotLight->UpdateMatrices();
                }
            }
        }
    }


    void Editor::Properties::MeshProperties()
    {
        Mesh* mesh = dynamic_cast<Mesh*>(Editor::Instance()->GetSelectedComponent());
        if (!mesh)
            return;

        Material* material = mesh->GetMaterial();
        if (!material)
            return;

        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            std::string name = material->GetName();
            char buffer[256];
            strncpy_s(buffer, name.c_str(), sizeof(buffer));
            if (ImGui::InputText("Name", buffer, sizeof(buffer)))
                material->SetName(buffer);

            ImGui::Separator();

            glm::vec4 baseColor = material->GetBaseColorFactor();
            if (ImGui::ColorEdit4("Base Color", glm::value_ptr(baseColor)))
                material->SetBaseColorFactor(baseColor);

            float metallic = material->GetMetallicFactor();
            if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f))
                material->SetMetallicFactor(metallic);

            float roughness = material->GetRoughnessFactor();
            if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
                material->SetRoughnessFactor(roughness);

            glm::vec3 emissive = material->GetEmissiveFactor();
            if (ImGui::ColorEdit3("Emissive Color", glm::value_ptr(emissive)))
                material->SetEmissiveFactor(emissive);

            float emissiveStrength = material->GetEmissiveStrength();
            if (ImGui::SliderFloat("Emissive Strength", &emissiveStrength, 0.0f, 10.0f))
                material->SetEmissiveStrength(emissiveStrength);

            float normalScale = material->GetNormalScale();
            if (ImGui::SliderFloat("Normal Scale", &normalScale, 0.0f, 5.0f))
                material->SetNormalScale(normalScale);

            float occlusionStrength = material->GetOcclusionStrength();
            if (ImGui::SliderFloat("Occlusion Strength", &occlusionStrength, 0.0f, 1.0f))
                material->SetOcclusionStrength(occlusionStrength);

            float ior = material->GetIOR();
            if (ImGui::SliderFloat("IOR", &ior, 1.0f, 2.5f))
                material->SetIOR(ior);

            bool transparent = material->GetTransparent();
            if (ImGui::Checkbox("Transparent", &transparent))
                material->SetTransparent(transparent);
        }
    }

}