// Properties.h
#include <Core/Common/EditorCommon.h>
#include <Core/Editor/CommandHistory/CommandHistory.h>

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

        // Helper to track dragging state for different properties
        struct DragState
        {
            bool isDragging = false;
            glm::vec3 vec3Start;
            glm::vec4 vec4Start;
            float floatStart;
        };
        
        std::map<std::string, DragState> m_DragStates;
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
            // Don't record transform commands while CommandHistory is executing (prevents undo/redo loops)
            bool isExecuting = Editor::Instance()->m_Commands->IsExecutingCommand();
            
            glm::vec3 pos = component->m_Transform.m_Translation;
            glm::vec3 scl = component->m_Transform.m_Scale;
            glm::vec3 rotEuler = glm::degrees(glm::eulerAngles(component->m_Transform.m_Rotation));

            // Position
            auto& posDrag = m_DragStates["transform_pos"];
            if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.05f))
            {
                if (!posDrag.isDragging && !isExecuting)
                {
                    posDrag.isDragging = true;
                    posDrag.vec3Start = component->m_Transform.m_Translation;
                }
                component->m_Transform.m_Translation = pos;
            }
            if (posDrag.isDragging && !ImGui::IsItemActive())
            {
                posDrag.isDragging = false;
                Transform oldTransform = component->m_Transform;
                oldTransform.m_Translation = posDrag.vec3Start;
                
                auto cmd = std::make_unique<TransformCommand>(
                    component,
                    oldTransform,
                    component->m_Transform,
                    true
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Rotation
            auto& rotDrag = m_DragStates["transform_rot"];
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotEuler), 0.1f))
            {
                if (!rotDrag.isDragging && !isExecuting)
                {
                    rotDrag.isDragging = true;
                    rotDrag.vec3Start = glm::degrees(glm::eulerAngles(component->m_Transform.m_Rotation));
                }
                component->m_Transform.m_Rotation = glm::quat(glm::radians(rotEuler));
            }
            if (rotDrag.isDragging && !ImGui::IsItemActive())
            {
                rotDrag.isDragging = false;
                Transform oldTransform = component->m_Transform;
                oldTransform.m_Rotation = glm::quat(glm::radians(rotDrag.vec3Start));
                
                auto cmd = std::make_unique<TransformCommand>(
                    component,
                    oldTransform,
                    component->m_Transform,
                    true
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Scale
            auto& sclDrag = m_DragStates["transform_scl"];
            if (ImGui::DragFloat3("Scale", glm::value_ptr(scl), 0.05f))
            {
                if (!sclDrag.isDragging && !isExecuting)
                {
                    sclDrag.isDragging = true;
                    sclDrag.vec3Start = component->m_Transform.m_Scale;
                }
                component->m_Transform.m_Scale = scl;
            }
            if (sclDrag.isDragging && !ImGui::IsItemActive())
            {
                sclDrag.isDragging = false;
                Transform oldTransform = component->m_Transform;
                oldTransform.m_Scale = sclDrag.vec3Start;
                
                auto cmd = std::make_unique<TransformCommand>(
                    component,
                    oldTransform,
                    component->m_Transform,
                    true
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }
        }
    }

    void Editor::Properties::LightProperties()
    {
        Light* light = dynamic_cast<Light*>(Editor::Instance()->GetSelectedComponent());
        if (!light)
            return;

        bool isExecuting = Editor::Instance()->m_Commands->IsExecutingCommand();

        if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // Color
            glm::vec4 color = light->m_Color;
            if (ImGui::ColorEdit4("Color", glm::value_ptr(color)))
            {
                auto cmd = std::make_unique<LightPropertyCommand>(
                    light,
                    LightPropertyCommand::PropertyType::Color,
                    light->m_Color,
                    color
                );
                light->m_Color = color;
                if (!isExecuting)
                    Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Intensity
            auto& intensityDrag = m_DragStates["light_intensity"];
            float intensity = light->m_Intensity;
            if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f, "%.1f"))
            {
                if (!intensityDrag.isDragging && !isExecuting)
                {
                    intensityDrag.isDragging = true;
                    intensityDrag.floatStart = light->m_Intensity;
                }
                light->m_Intensity = intensity;
            }
            if (intensityDrag.isDragging && !ImGui::IsItemActive())
            {
                intensityDrag.isDragging = false;
                auto cmd = std::make_unique<LightPropertyCommand>(
                    light,
                    LightPropertyCommand::PropertyType::Intensity,
                    intensityDrag.floatStart,
                    light->m_Intensity
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Cast Shadows
            bool castShadows = light->m_CastShadows;
            if (ImGui::Checkbox("Cast Shadows", &castShadows))
            {
                auto cmd = std::make_unique<LightPropertyCommand>(
                    light,
                    LightPropertyCommand::PropertyType::CastShadows,
                    light->m_CastShadows,
                    castShadows
                );
                light->m_CastShadows = castShadows;
                if (!isExecuting)
                    Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            ImGui::Separator();

            // Directional Light
            if (auto dirLight = dynamic_cast<DirectionalLight*>(light))
            {
                if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto& dirDrag = m_DragStates["dir_direction"];
                    glm::vec3 direction = dirLight->m_Dir;
                    if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f))
                    {
                        if (!dirDrag.isDragging && !isExecuting)
                        {
                            dirDrag.isDragging = true;
                            dirDrag.vec3Start = dirLight->m_Dir;
                        }
                        dirLight->m_Dir = glm::normalize(direction);
                    }
                    if (dirDrag.isDragging && !ImGui::IsItemActive())
                    {
                        dirDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::Direction,
                            dirDrag.vec3Start,
                            dirLight->m_Dir
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }
                }
            }
            // Point Light
            else if (auto pointLight = dynamic_cast<PointLight*>(light))
            {
                if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto& posDrag = m_DragStates["point_position"];
                    glm::vec3 position = pointLight->m_Position;
                    if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.05f))
                    {
                        if (!posDrag.isDragging && !isExecuting)
                        {
                            posDrag.isDragging = true;
                            posDrag.vec3Start = pointLight->m_Position;
                        }
                        pointLight->m_Position = position;
                    }
                    if (posDrag.isDragging && !ImGui::IsItemActive())
                    {
                        posDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::Position,
                            posDrag.vec3Start,
                            pointLight->m_Position
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }

                    auto& radiusDrag = m_DragStates["point_radius"];
                    float radius = pointLight->m_Radius;
                    if (ImGui::SliderFloat("Radius", &radius, 0.0f, 200.0f, "%.1f"))
                    {
                        if (!radiusDrag.isDragging && !isExecuting)
                        {
                            radiusDrag.isDragging = true;
                            radiusDrag.floatStart = pointLight->m_Radius;
                        }
                        pointLight->m_Radius = radius;
                        pointLight->UpdateMatrices();
                    }
                    if (radiusDrag.isDragging && !ImGui::IsItemActive())
                    {
                        radiusDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::Radius,
                            radiusDrag.floatStart,
                            pointLight->m_Radius
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }
                }
            }
            // Spot Light
            else if (auto spotLight = dynamic_cast<SpotLight*>(light))
            {
                if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto& posDrag = m_DragStates["spot_position"];
                    glm::vec3 position = spotLight->m_Position;
                    if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.05f))
                    {
                        if (!posDrag.isDragging && !isExecuting)
                        {
                            posDrag.isDragging = true;
                            posDrag.vec3Start = spotLight->m_Position;
                        }
                        spotLight->m_Position = position;
                    }
                    if (posDrag.isDragging && !ImGui::IsItemActive())
                    {
                        posDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::Position,
                            posDrag.vec3Start,
                            spotLight->m_Position
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }

                    auto& dirDrag = m_DragStates["spot_direction"];
                    glm::vec3 direction = spotLight->m_Direction;
                    if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.01f))
                    {
                        if (!dirDrag.isDragging && !isExecuting)
                        {
                            dirDrag.isDragging = true;
                            dirDrag.vec3Start = spotLight->m_Direction;
                        }
                        spotLight->m_Direction = glm::normalize(direction);
                    }
                    if (dirDrag.isDragging && !ImGui::IsItemActive())
                    {
                        dirDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::Direction,
                            dirDrag.vec3Start,
                            spotLight->m_Direction
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }

                    auto& radiusDrag = m_DragStates["spot_radius"];
                    float radius = spotLight->m_Radius;
                    if (ImGui::SliderFloat("Radius", &radius, 0.0f, 200.0f, "%.1f"))
                    {
                        if (!radiusDrag.isDragging && !isExecuting)
                        {
                            radiusDrag.isDragging = true;
                            radiusDrag.floatStart = spotLight->m_Radius;
                        }
                        spotLight->m_Radius = radius;
                        spotLight->UpdateMatrices();
                    }
                    if (radiusDrag.isDragging && !ImGui::IsItemActive())
                    {
                        radiusDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::Radius,
                            radiusDrag.floatStart,
                            spotLight->m_Radius
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }

                    auto& innerDrag = m_DragStates["spot_inner"];
                    float inner = glm::degrees(spotLight->m_InnerCone);
                    if (ImGui::SliderFloat("Inner Cone", &inner, 1.0f, 89.0f, "%.1f°"))
                    {
                        if (!innerDrag.isDragging && !isExecuting)
                        {
                            innerDrag.isDragging = true;
                            innerDrag.floatStart = spotLight->m_InnerCone;
                        }
                        spotLight->m_InnerCone = glm::radians(inner);
                    }
                    if (innerDrag.isDragging && !ImGui::IsItemActive())
                    {
                        innerDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::InnerCone,
                            innerDrag.floatStart,
                            spotLight->m_InnerCone
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }

                    auto& outerDrag = m_DragStates["spot_outer"];
                    float outer = glm::degrees(spotLight->m_OuterCone);
                    if (ImGui::SliderFloat("Outer Cone", &outer, inner + 0.1f, 90.0f, "%.1f°"))
                    {
                        if (!outerDrag.isDragging && !isExecuting)
                        {
                            outerDrag.isDragging = true;
                            outerDrag.floatStart = spotLight->m_OuterCone;
                        }
                        spotLight->m_OuterCone = glm::radians(outer);
                    }
                    if (outerDrag.isDragging && !ImGui::IsItemActive())
                    {
                        outerDrag.isDragging = false;
                        auto cmd = std::make_unique<LightPropertyCommand>(
                            light,
                            LightPropertyCommand::PropertyType::OuterCone,
                            outerDrag.floatStart,
                            spotLight->m_OuterCone
                        );
                        Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
                    }

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

        bool isExecuting = Editor::Instance()->m_Commands->IsExecutingCommand();

        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            std::string name = material->GetName();
            char buffer[256];
            strncpy_s(buffer, name.c_str(), sizeof(buffer));
            if (ImGui::InputText("Name", buffer, sizeof(buffer)))
                material->SetName(buffer);

            ImGui::Separator();

            // Base Color
            glm::vec4 baseColor = material->GetBaseColorFactor();
            if (ImGui::ColorEdit4("Base Color", glm::value_ptr(baseColor)))
            {
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::BaseColor,
                    material->GetBaseColorFactor(),
                    baseColor
                );
                material->SetBaseColorFactor(baseColor);
                if (!isExecuting)
                    Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Metallic
            auto& metallicDrag = m_DragStates["mat_metallic"];
            float metallic = material->GetMetallicFactor();
            if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f))
            {
                if (!metallicDrag.isDragging && !isExecuting)
                {
                    metallicDrag.isDragging = true;
                    metallicDrag.floatStart = material->GetMetallicFactor();
                }
                material->SetMetallicFactor(metallic);
            }
            if (metallicDrag.isDragging && !ImGui::IsItemActive())
            {
                metallicDrag.isDragging = false;
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::Metallic,
                    metallicDrag.floatStart,
                    material->GetMetallicFactor()
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Roughness
            auto& roughnessDrag = m_DragStates["mat_roughness"];
            float roughness = material->GetRoughnessFactor();
            if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
            {
                if (!roughnessDrag.isDragging && !isExecuting)
                {
                    roughnessDrag.isDragging = true;
                    roughnessDrag.floatStart = material->GetRoughnessFactor();
                }
                material->SetRoughnessFactor(roughness);
            }
            if (roughnessDrag.isDragging && !ImGui::IsItemActive())
            {
                roughnessDrag.isDragging = false;
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::Roughness,
                    roughnessDrag.floatStart,
                    material->GetRoughnessFactor()
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Emissive Color
            glm::vec3 emissive = material->GetEmissiveFactor();
            if (ImGui::ColorEdit3("Emissive Color", glm::value_ptr(emissive)))
            {
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::Emissive,
                    material->GetEmissiveFactor(),
                    emissive
                );
                material->SetEmissiveFactor(emissive);
                if (!isExecuting)
                    Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Emissive Strength
            auto& emissiveStrengthDrag = m_DragStates["mat_emissive_strength"];
            float emissiveStrength = material->GetEmissiveStrength();
            if (ImGui::SliderFloat("Emissive Strength", &emissiveStrength, 0.0f, 10.0f))
            {
                if (!emissiveStrengthDrag.isDragging && !isExecuting)
                {
                    emissiveStrengthDrag.isDragging = true;
                    emissiveStrengthDrag.floatStart = material->GetEmissiveStrength();
                }
                material->SetEmissiveStrength(emissiveStrength);
            }
            if (emissiveStrengthDrag.isDragging && !ImGui::IsItemActive())
            {
                emissiveStrengthDrag.isDragging = false;
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::EmissiveStrength,
                    emissiveStrengthDrag.floatStart,
                    material->GetEmissiveStrength()
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Normal Scale
            auto& normalScaleDrag = m_DragStates["mat_normal_scale"];
            float normalScale = material->GetNormalScale();
            if (ImGui::SliderFloat("Normal Scale", &normalScale, 0.0f, 5.0f))
            {
                if (!normalScaleDrag.isDragging && !isExecuting)
                {
                    normalScaleDrag.isDragging = true;
                    normalScaleDrag.floatStart = material->GetNormalScale();
                }
                material->SetNormalScale(normalScale);
            }
            if (normalScaleDrag.isDragging && !ImGui::IsItemActive())
            {
                normalScaleDrag.isDragging = false;
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::NormalScale,
                    normalScaleDrag.floatStart,
                    material->GetNormalScale()
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Occlusion Strength
            auto& occlusionDrag = m_DragStates["mat_occlusion"];
            float occlusionStrength = material->GetOcclusionStrength();
            if (ImGui::SliderFloat("Occlusion Strength", &occlusionStrength, 0.0f, 1.0f))
            {
                if (!occlusionDrag.isDragging && !isExecuting)
                {
                    occlusionDrag.isDragging = true;
                    occlusionDrag.floatStart = material->GetOcclusionStrength();
                }
                material->SetOcclusionStrength(occlusionStrength);
            }
            if (occlusionDrag.isDragging && !ImGui::IsItemActive())
            {
                occlusionDrag.isDragging = false;
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::OcclusionStrength,
                    occlusionDrag.floatStart,
                    material->GetOcclusionStrength()
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // IOR
            auto& iorDrag = m_DragStates["mat_ior"];
            float ior = material->GetIOR();
            if (ImGui::SliderFloat("IOR", &ior, 1.0f, 2.5f))
            {
                if (!iorDrag.isDragging && !isExecuting)
                {
                    iorDrag.isDragging = true;
                    iorDrag.floatStart = material->GetIOR();
                }
                material->SetIOR(ior);
            }
            if (iorDrag.isDragging && !ImGui::IsItemActive())
            {
                iorDrag.isDragging = false;
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::IOR,
                    iorDrag.floatStart,
                    material->GetIOR()
                );
                Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }

            // Transparent
            bool transparent = material->GetTransparent();
            if (ImGui::Checkbox("Transparent", &transparent))
            {
                auto cmd = std::make_unique<MaterialPropertyCommand>(
                    material,
                    MaterialPropertyCommand::PropertyType::Transparent,
                    material->GetTransparent(),
                    transparent
                );
                material->SetTransparent(transparent);
                if (!isExecuting)
                    Editor::Instance()->m_Commands->ExecuteCommand(std::move(cmd));
            }
        }
    }
}