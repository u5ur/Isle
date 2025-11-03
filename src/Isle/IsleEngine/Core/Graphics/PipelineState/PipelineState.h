// PipelineState.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>

namespace Isle
{
	class PipelineState
	{
	private:
		bool m_DepthTest = true;
		bool m_DepthWrite = true;
		bool m_BlendEnabled = false;
		bool m_CullEnabled = true;
		bool m_AlphaTest = false;
		bool m_UseSeparateAlphaBlend = false;
		float m_AlphaCutoff = 0.5f;
		bool m_ColorMaskR = true;
		bool m_ColorMaskG = true;
		bool m_ColorMaskB = true;
		bool m_ColorMaskA = true;

		GLenum m_CullFace = GL_BACK;
		GLenum m_BlendSrc = GL_SRC_ALPHA;
		GLenum m_BlendDst = GL_ONE_MINUS_SRC_ALPHA;
		GLenum m_BlendAlphaSrc = GL_ONE;
		GLenum m_BlendAlphaDst = GL_ONE_MINUS_SRC_ALPHA;

	public:
		void Bind();

		void SetAlphaTest(bool value, float cutoff = 0.5f);
		void SetDepthTest(bool value);
		void SetDepthWrite(bool value);
		void SetBlendEnabled(bool value);
		void SetCullEnabled(bool value);
		void SetCullFace(GLenum value);
		void SetBlendSrc(GLenum value);
		void SetBlendDst(GLenum value);
		void SetBlendAlphaSrc(GLenum value);
		void SetBlendAlphaDst(GLenum value);
		void SetColorMask(bool r, bool g, bool b, bool a);
	};
}