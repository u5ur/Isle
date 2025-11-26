#pragma once
#include <Core/Common/Common.h>
#include <functional>
#include <any>
#include <typeindex>

namespace Isle
{
    class ReflectionRegistry;
    class PropertyBase;
    class FunctionBase;
    class ClassDescriptor;

    enum class PropertyType
    {
        Int,
        Float,
        Bool,
        String,
        Vec2,
        Vec3,
        Vec4,
        Color,
        Object,
        Unknown
    };

    enum PropertyFlags : uint32_t
    {
        None = 0,
        ReadOnly = 1 << 0,
        EditAnywhere = 1 << 1,
        EditInstanceOnly = 1 << 2,
        BlueprintReadWrite = 1 << 3,
        Hidden = 1 << 4,
        Transient = 1 << 5,
        VisibleAnywhere = 1 << 6
    };

    class PropertyBase
    {
    protected:
        std::string m_Name;
        PropertyType m_Type;
        uint32_t m_Flags;
        std::string m_Category;
        std::string m_Tooltip;

    public:
        PropertyBase(const std::string& name, PropertyType type, uint32_t flags = PropertyFlags::EditAnywhere)
            : m_Name(name), m_Type(type), m_Flags(flags), m_Category("Default") {
        }

        virtual ~PropertyBase() = default;

        const std::string& GetName() const { return m_Name; }
        PropertyType GetType() const { return m_Type; }
        uint32_t GetFlags() const { return m_Flags; }
        const std::string& GetCategory() const { return m_Category; }
        const std::string& GetTooltip() const { return m_Tooltip; }

        void SetCategory(const std::string& category) { m_Category = category; }
        void SetTooltip(const std::string& tooltip) { m_Tooltip = tooltip; }

        virtual std::any GetValue(void* instance) const = 0;
        virtual void SetValue(void* instance, const std::any& value) = 0;
    };

    template<typename ClassType, typename PropType>
    class Property : public PropertyBase
    {
    private:
        PropType ClassType::* m_MemberPtr;

    public:
        Property(const std::string& name, PropType ClassType::* memberPtr, uint32_t flags = PropertyFlags::EditAnywhere)
            : PropertyBase(name, DeducePropertyType<PropType>(), flags)
            , m_MemberPtr(memberPtr)
        {
        }

        std::any GetValue(void* instance) const override
        {
            ClassType* obj = static_cast<ClassType*>(instance);
            return std::any(obj->*m_MemberPtr);
        }

        void SetValue(void* instance, const std::any& value) override
        {
            ClassType* obj = static_cast<ClassType*>(instance);
            obj->*m_MemberPtr = std::any_cast<PropType>(value);
        }

    private:
        template<typename T>
        static PropertyType DeducePropertyType()
        {
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>)
                return PropertyType::Int;
            else if constexpr (std::is_same_v<T, size_t>)
                return PropertyType::Int;
            else if constexpr (std::is_same_v<T, float>)
                return PropertyType::Float;
            else if constexpr (std::is_same_v<T, bool>)
                return PropertyType::Bool;
            else if constexpr (std::is_same_v<T, std::string>)
                return PropertyType::String;
            else if constexpr (std::is_same_v<T, glm::vec2>)
                return PropertyType::Vec2;
            else if constexpr (std::is_same_v<T, glm::vec3>)
                return PropertyType::Vec3;
            else if constexpr (std::is_same_v<T, glm::vec4>)
                return PropertyType::Vec4;
            else if constexpr (std::is_base_of_v<Object, T>)
                return PropertyType::Object;
            else
                return PropertyType::Unknown;
        }
    };

    class FunctionBase
    {
    protected:
        std::string m_Name;
        std::string m_Category;

    public:
        FunctionBase(const std::string& name) : m_Name(name), m_Category("Default") {}
        virtual ~FunctionBase() = default;

        const std::string& GetName() const { return m_Name; }
        const std::string& GetCategory() const { return m_Category; }
        void SetCategory(const std::string& category) { m_Category = category; }

        virtual std::any Invoke(void* instance, const std::vector<std::any>& args) = 0;
        virtual size_t GetParameterCount() const = 0;
    };

    template<typename ClassType, typename RetType, typename... Args>
    class Function : public FunctionBase
    {
    private:
        using FuncPtr = RetType(ClassType::*)(Args...);
        FuncPtr m_FuncPtr;

    public:
        Function(const std::string& name, FuncPtr funcPtr)
            : FunctionBase(name), m_FuncPtr(funcPtr)
        {
        }

        std::any Invoke(void* instance, const std::vector<std::any>& args) override
        {
            ClassType* obj = static_cast<ClassType*>(instance);

            if (args.size() != sizeof...(Args))
            {
                ISLE_ERROR("Function %s: Expected %zu arguments, got %zu\n",
                    m_Name.c_str(), sizeof...(Args), args.size());
                return std::any();
            }

            return InvokeImpl(obj, args, std::index_sequence_for<Args...>{});
        }

        size_t GetParameterCount() const override { return sizeof...(Args); }

    private:
        template<size_t... Is>
        std::any InvokeImpl(ClassType* obj, const std::vector<std::any>& args, std::index_sequence<Is...>)
        {
            if constexpr (std::is_void_v<RetType>)
            {
                (obj->*m_FuncPtr)(std::any_cast<Args>(args[Is])...);
                return std::any();
            }
            else
            {
                return std::any((obj->*m_FuncPtr)(std::any_cast<Args>(args[Is])...));
            }
        }
    };

    class ClassDescriptor
    {
    private:
        std::string m_Name;
        std::type_index m_TypeIndex;
        std::unordered_map<std::string, std::unique_ptr<PropertyBase>> m_Properties;
        std::unordered_map<std::string, std::unique_ptr<FunctionBase>> m_Functions;
        ClassDescriptor* m_Parent;

    public:
        ClassDescriptor(const std::string& name, std::type_index typeIndex)
            : m_Name(name), m_TypeIndex(typeIndex), m_Parent(nullptr)
        {
        }

        const std::string& GetName() const { return m_Name; }
        std::type_index GetTypeIndex() const { return m_TypeIndex; }

        void SetParent(ClassDescriptor* parent) { m_Parent = parent; }
        ClassDescriptor* GetParent() const { return m_Parent; }

        void AddProperty(std::unique_ptr<PropertyBase> prop)
        {
            m_Properties[prop->GetName()] = std::move(prop);
        }

        void AddFunction(std::unique_ptr<FunctionBase> func)
        {
            m_Functions[func->GetName()] = std::move(func);
        }

        PropertyBase* GetProperty(const std::string& name) const
        {
            auto it = m_Properties.find(name);
            if (it != m_Properties.end())
                return it->second.get();

            if (m_Parent)
                return m_Parent->GetProperty(name);

            return nullptr;
        }

        FunctionBase* GetFunction(const std::string& name) const
        {
            auto it = m_Functions.find(name);
            if (it != m_Functions.end())
                return it->second.get();

            if (m_Parent)
                return m_Parent->GetFunction(name);

            return nullptr;
        }

        const std::unordered_map<std::string, std::unique_ptr<PropertyBase>>& GetProperties() const
        {
            return m_Properties;
        }

        const std::unordered_map<std::string, std::unique_ptr<FunctionBase>>& GetFunctions() const
        {
            return m_Functions;
        }

        std::vector<PropertyBase*> GetAllProperties() const
        {
            std::vector<PropertyBase*> props;

            if (m_Parent)
            {
                auto parentProps = m_Parent->GetAllProperties();
                props.insert(props.end(), parentProps.begin(), parentProps.end());
            }

            for (auto& [name, prop] : m_Properties)
                props.push_back(prop.get());

            return props;
        }

        std::vector<FunctionBase*> GetAllFunctions() const
        {
            std::vector<FunctionBase*> funcs;

            if (m_Parent)
            {
                auto parentFuncs = m_Parent->GetAllFunctions();
                funcs.insert(funcs.end(), parentFuncs.begin(), parentFuncs.end());
            }

            for (auto& [name, func] : m_Functions)
                funcs.push_back(func.get());

            return funcs;
        }
    };

    class ReflectionRegistry : public Singleton<ReflectionRegistry>
    {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<ClassDescriptor>> m_Classes;
        std::unordered_map<std::string, ClassDescriptor*> m_ClassesByName;

    public:
        template<typename T>
        ClassDescriptor* RegisterClass(const std::string& name)
        {
            std::type_index typeIdx(typeid(T));

            if (m_Classes.find(typeIdx) != m_Classes.end())
            {
                return m_Classes[typeIdx].get();
            }

            auto desc = std::make_unique<ClassDescriptor>(name, typeIdx);
            ClassDescriptor* ptr = desc.get();

            m_Classes[typeIdx] = std::move(desc);
            m_ClassesByName[name] = ptr;

            ISLE_LOG("Registered class: %s\n", name.c_str());

            return ptr;
        }

        template<typename T>
        ClassDescriptor* GetClass()
        {
            std::type_index typeIdx(typeid(T));
            auto it = m_Classes.find(typeIdx);
            if (it != m_Classes.end())
                return it->second.get();
            return nullptr;
        }

        ClassDescriptor* GetClass(const std::string& name)
        {
            auto it = m_ClassesByName.find(name);
            if (it != m_ClassesByName.end())
                return it->second;
            return nullptr;
        }

        const std::unordered_map<std::string, ClassDescriptor*>& GetAllClasses() const
        {
            return m_ClassesByName;
        }
    };

    template<typename T>
    struct ClassRegistration
    {
        ClassDescriptor* descriptor;

        ClassRegistration(const char* className)
        {
            descriptor = ReflectionRegistry::Instance()->RegisterClass<T>(className);
        }

        template<typename PropType>
        ClassRegistration& Property(const char* name, PropType T::* member, uint32_t flags = PropertyFlags::EditAnywhere)
        {
            auto prop = std::make_unique<Isle::Property<T, PropType>>(name, member, flags);
            descriptor->AddProperty(std::move(prop));
            return *this;
        }

        template<typename RetType, typename... Args>
        ClassRegistration& Function(const char* name, RetType(T::* func)(Args...))
        {
            auto fn = std::make_unique<Isle::Function<T, RetType, Args...>>(name, func);
            descriptor->AddFunction(std::move(fn));
            return *this;
        }

        template<typename ParentType>
        ClassRegistration& Parent()
        {
            ClassDescriptor* parent = ReflectionRegistry::Instance()->GetClass<ParentType>();
            if (parent)
                descriptor->SetParent(parent);
            return *this;
        }
    };
}

#define GENERATED_BODY() \
    public: \
        static Isle::ClassDescriptor* StaticClass(); \
    private:

#define UPROPERTY(...) 
#define UFUNCTION()

#define BEGIN_REFLECT_CLASS(ClassName) \
    Isle::ClassDescriptor* ClassName::StaticClass() \
    { \
        static Isle::ClassRegistration<ClassName> registration(#ClassName); \
        return registration.descriptor; \
    } \
    static struct ClassName##_ReflectionRegistrar \
    { \
        using _ReflClass = ClassName; \
        ClassName##_ReflectionRegistrar() \
        { \
            ClassName::StaticClass(); \
            Isle::ClassRegistration<ClassName> reg(#ClassName); \
            reg

#define REFLECT_PROP(PropName, Flags) \
    .Property(#PropName, &_ReflClass::PropName, Flags)

#define REFLECT_FUNC(FuncName) \
    .Function(#FuncName, &_ReflClass::FuncName)

#define REFLECT_PARENT_CLASS(ParentClass) \
    .Parent<ParentClass>()

#define END_REFLECT_CLASS() \
                ; \
        } \
    } g_ReflectionRegistrar;