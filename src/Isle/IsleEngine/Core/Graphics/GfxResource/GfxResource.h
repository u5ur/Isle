#include <Core/Common/Common.h>

namespace Isle
{
	class GfxResource : public Object
	{
	public:
		bool m_IsLoaded = false;
		bool m_IsResident = false;
		size_t m_SizeInBytes = 0;

	public:
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Bind(uint32_t slot = 0) {}
		virtual void Unbind(uint32_t slot = 0) {}

		bool IsLoaded() const;
		bool IsResident() const;
		size_t GetSizeInBytes() const;
	};
}