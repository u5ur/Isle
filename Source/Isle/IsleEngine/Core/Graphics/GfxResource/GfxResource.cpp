// GfxResource.cpp
#include "GfxResource.h"

namespace Isle
{
    bool GfxResource::IsLoaded() const
    {
        return m_IsLoaded;
    }

    bool GfxResource::IsResident() const
    {
        return m_IsResident;
    }

    size_t GfxResource::GetSizeInBytes() const
    {
        return m_SizeInBytes;
    }
}