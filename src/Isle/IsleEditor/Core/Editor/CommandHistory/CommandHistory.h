// CommandHistory.h
#pragma once
#include <Core/Common/EditorCommon.h>
#include <memory>
#include <vector>
#include <stack>

namespace Isle
{
    // Base command interface
    class ICommand
    {
    public:
        virtual ~ICommand() = default;
        virtual void Execute() = 0;
        virtual void Undo() = 0;
        virtual const char* GetName() const = 0;
    };

    // Transform command for moving/rotating/scaling objects
    class TransformCommand : public ICommand
    {
    private:
        SceneComponent* m_Component;
        Transform m_OldTransform;
        Transform m_NewTransform;
        bool m_IsLocal;

    public:
        TransformCommand(SceneComponent* component, const Transform& oldTransform, const Transform& newTransform, bool isLocal = true)
            : m_Component(component)
            , m_OldTransform(oldTransform)
            , m_NewTransform(newTransform)
            , m_IsLocal(isLocal)
        {
        }

        virtual void Execute() override
        {
            if (!m_Component) return;
            m_Component->m_Transform = m_NewTransform;
        }

        virtual void Undo() override
        {
            if (!m_Component) return;
            m_Component->m_Transform = m_OldTransform;
        }

        virtual const char* GetName() const override
        {
            return "Transform";
        }
    };

    // Property change command (generic for any property)
    template<typename T>
    class PropertyCommand : public ICommand
    {
    private:
        std::function<void(const T&)> m_Setter;
        T m_OldValue;
        T m_NewValue;
        std::string m_Name;

    public:
        PropertyCommand(const std::string& name, std::function<void(const T&)> setter, const T& oldValue, const T& newValue)
            : m_Name(name)
            , m_Setter(setter)
            , m_OldValue(oldValue)
            , m_NewValue(newValue)
        {
        }

        virtual void Execute() override
        {
            m_Setter(m_NewValue);
        }

        virtual void Undo() override
        {
            m_Setter(m_OldValue);
        }

        virtual const char* GetName() const override
        {
            return m_Name.c_str();
        }
    };

    // Material property command
    class MaterialPropertyCommand : public ICommand
    {
    public:
        enum class PropertyType
        {
            BaseColor,
            Metallic,
            Roughness,
            Emissive,
            EmissiveStrength,
            NormalScale,
            OcclusionStrength,
            IOR,
            Transparent
        };

    private:
        Material* m_Material;
        PropertyType m_Type;

        // Union to store different types of values
        union PropertyValue
        {
            glm::vec4 vec4Value;
            glm::vec3 vec3Value;
            float floatValue;
            bool boolValue;

            PropertyValue() : vec4Value(0.0f) {}
        };

        PropertyValue m_OldValue;
        PropertyValue m_NewValue;

    public:
        MaterialPropertyCommand(Material* material, PropertyType type, const glm::vec4& oldVal, const glm::vec4& newVal)
            : m_Material(material), m_Type(type)
        {
            m_OldValue.vec4Value = oldVal;
            m_NewValue.vec4Value = newVal;
        }

        MaterialPropertyCommand(Material* material, PropertyType type, const glm::vec3& oldVal, const glm::vec3& newVal)
            : m_Material(material), m_Type(type)
        {
            m_OldValue.vec3Value = oldVal;
            m_NewValue.vec3Value = newVal;
        }

        MaterialPropertyCommand(Material* material, PropertyType type, float oldVal, float newVal)
            : m_Material(material), m_Type(type)
        {
            m_OldValue.floatValue = oldVal;
            m_NewValue.floatValue = newVal;
        }

        MaterialPropertyCommand(Material* material, PropertyType type, bool oldVal, bool newVal)
            : m_Material(material), m_Type(type)
        {
            m_OldValue.boolValue = oldVal;
            m_NewValue.boolValue = newVal;
        }

        virtual void Execute() override
        {
            if (!m_Material) return;
            ApplyValue(m_NewValue);
        }

        virtual void Undo() override
        {
            if (!m_Material) return;
            ApplyValue(m_OldValue);
        }

        virtual const char* GetName() const override
        {
            switch (m_Type)
            {
            case PropertyType::BaseColor: return "Base Color";
            case PropertyType::Metallic: return "Metallic";
            case PropertyType::Roughness: return "Roughness";
            case PropertyType::Emissive: return "Emissive";
            case PropertyType::EmissiveStrength: return "Emissive Strength";
            case PropertyType::NormalScale: return "Normal Scale";
            case PropertyType::OcclusionStrength: return "Occlusion Strength";
            case PropertyType::IOR: return "IOR";
            case PropertyType::Transparent: return "Transparent";
            default: return "Material Property";
            }
        }

    private:
        void ApplyValue(const PropertyValue& value)
        {
            switch (m_Type)
            {
            case PropertyType::BaseColor:
                m_Material->SetBaseColorFactor(value.vec4Value);
                break;
            case PropertyType::Metallic:
                m_Material->SetMetallicFactor(value.floatValue);
                break;
            case PropertyType::Roughness:
                m_Material->SetRoughnessFactor(value.floatValue);
                break;
            case PropertyType::Emissive:
                m_Material->SetEmissiveFactor(value.vec3Value);
                break;
            case PropertyType::EmissiveStrength:
                m_Material->SetEmissiveStrength(value.floatValue);
                break;
            case PropertyType::NormalScale:
                m_Material->SetNormalScale(value.floatValue);
                break;
            case PropertyType::OcclusionStrength:
                m_Material->SetOcclusionStrength(value.floatValue);
                break;
            case PropertyType::IOR:
                m_Material->SetIOR(value.floatValue);
                break;
            case PropertyType::Transparent:
                m_Material->SetTransparent(value.boolValue);
                break;
            }
        }
    };

    // Light property command
    class LightPropertyCommand : public ICommand
    {
    public:
        enum class PropertyType
        {
            Color,
            Intensity,
            CastShadows,
            Direction,
            Position,
            Radius,
            InnerCone,
            OuterCone
        };

    private:
        Light* m_Light;
        PropertyType m_Type;

        union PropertyValue
        {
            glm::vec4 vec4Value;
            glm::vec3 vec3Value;
            float floatValue;
            bool boolValue;

            PropertyValue() : vec4Value(0.0f) {}
        };

        PropertyValue m_OldValue;
        PropertyValue m_NewValue;

    public:
        LightPropertyCommand(Light* light, PropertyType type, const glm::vec4& oldVal, const glm::vec4& newVal)
            : m_Light(light), m_Type(type)
        {
            m_OldValue.vec4Value = oldVal;
            m_NewValue.vec4Value = newVal;
        }

        LightPropertyCommand(Light* light, PropertyType type, const glm::vec3& oldVal, const glm::vec3& newVal)
            : m_Light(light), m_Type(type)
        {
            m_OldValue.vec3Value = oldVal;
            m_NewValue.vec3Value = newVal;
        }

        LightPropertyCommand(Light* light, PropertyType type, float oldVal, float newVal)
            : m_Light(light), m_Type(type)
        {
            m_OldValue.floatValue = oldVal;
            m_NewValue.floatValue = newVal;
        }

        LightPropertyCommand(Light* light, PropertyType type, bool oldVal, bool newVal)
            : m_Light(light), m_Type(type)
        {
            m_OldValue.boolValue = oldVal;
            m_NewValue.boolValue = newVal;
        }

        virtual void Execute() override
        {
            if (!m_Light) return;
            ApplyValue(m_NewValue);
        }

        virtual void Undo() override
        {
            if (!m_Light) return;
            ApplyValue(m_OldValue);
        }

        virtual const char* GetName() const override
        {
            switch (m_Type)
            {
            case PropertyType::Color: return "Light Color";
            case PropertyType::Intensity: return "Light Intensity";
            case PropertyType::CastShadows: return "Cast Shadows";
            case PropertyType::Direction: return "Light Direction";
            case PropertyType::Position: return "Light Position";
            case PropertyType::Radius: return "Light Radius";
            case PropertyType::InnerCone: return "Inner Cone";
            case PropertyType::OuterCone: return "Outer Cone";
            default: return "Light Property";
            }
        }

    private:
        void ApplyValue(const PropertyValue& value)
        {
            switch (m_Type)
            {
            case PropertyType::Color:
                m_Light->m_Color = value.vec4Value;
                break;
            case PropertyType::Intensity:
                m_Light->m_Intensity = value.floatValue;
                break;
            case PropertyType::CastShadows:
                m_Light->m_CastShadows = value.boolValue;
                break;
            case PropertyType::Direction:
                if (auto dirLight = dynamic_cast<DirectionalLight*>(m_Light))
                    dirLight->m_Dir = glm::normalize(value.vec3Value);
                else if (auto spotLight = dynamic_cast<SpotLight*>(m_Light))
                    spotLight->m_Direction = glm::normalize(value.vec3Value);
                break;
            case PropertyType::Position:
                if (auto pointLight = dynamic_cast<PointLight*>(m_Light))
                    pointLight->m_Position = value.vec3Value;
                else if (auto spotLight = dynamic_cast<SpotLight*>(m_Light))
                    spotLight->m_Position = value.vec3Value;
                break;
            case PropertyType::Radius:
                if (auto pointLight = dynamic_cast<PointLight*>(m_Light))
                {
                    pointLight->m_Radius = value.floatValue;
                    pointLight->UpdateMatrices();
                }
                else if (auto spotLight = dynamic_cast<SpotLight*>(m_Light))
                {
                    spotLight->m_Radius = value.floatValue;
                    spotLight->UpdateMatrices();
                }
                break;
            case PropertyType::InnerCone:
                if (auto spotLight = dynamic_cast<SpotLight*>(m_Light))
                    spotLight->m_InnerCone = value.floatValue;
                break;
            case PropertyType::OuterCone:
                if (auto spotLight = dynamic_cast<SpotLight*>(m_Light))
                    spotLight->m_OuterCone = value.floatValue;
                break;
            }
        }
    };

    // Command History Manager
    class Editor::CommandHistory : public EditorComponent
    {
    private:
        std::vector<std::unique_ptr<ICommand>> m_Commands;
        int m_CurrentIndex = -1;
        const int m_MaxCommands = 100;
        bool m_IsExecuting = false;

    public:
        virtual void Start() override {}
        virtual void Update() override {}
        virtual void Destroy() override { Clear(); }
        virtual const char* GetWindowName() const override { return "Command History"; }

        void ExecuteCommand(std::unique_ptr<ICommand> command)
        {
            if (!command) return;

            m_IsExecuting = true;
            command->Execute();
            m_IsExecuting = false;

            if (m_CurrentIndex >= 0 && m_CurrentIndex < (int)m_Commands.size() - 1)
            {
                auto eraseBegin = m_Commands.begin() + m_CurrentIndex + 1;
                if (eraseBegin < m_Commands.end())
                    m_Commands.erase(eraseBegin, m_Commands.end());
            }


            m_Commands.push_back(std::move(command));
            m_CurrentIndex++;

            if (m_Commands.size() > m_MaxCommands)
            {
                m_Commands.erase(m_Commands.begin());
                m_CurrentIndex--;
            }
        }

        void Undo()
        {
            if (!CanUndo()) return;

            m_IsExecuting = true;
            m_Commands[m_CurrentIndex]->Undo();
            m_CurrentIndex--;
            m_IsExecuting = false;
        }

        void Redo()
        {
            if (!CanRedo()) return;

            m_CurrentIndex++;
            m_IsExecuting = true;
            m_Commands[m_CurrentIndex]->Execute();
            m_IsExecuting = false;
        }

        bool CanUndo() const
        {
            return m_CurrentIndex >= 0;
        }

        bool CanRedo() const
        {
            return m_CurrentIndex < (int)m_Commands.size() - 1;
        }

        void Clear()
        {
            m_Commands.clear();
            m_CurrentIndex = -1;
        }

        bool IsExecutingCommand() const
        {
            return m_IsExecuting;
        }

        const std::vector<std::unique_ptr<ICommand>>& GetCommands() const
        {
            return m_Commands;
        }

        int GetCurrentIndex() const
        {
            return m_CurrentIndex;
        }
    };
}