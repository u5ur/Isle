// PipelineState.cpp
#include "PipelineState.h"

namespace Isle
{
	void PipelineState::Bind()
	{
		if (m_DepthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		glDepthMask(m_DepthWrite ? GL_TRUE : GL_FALSE);

		if (m_BlendEnabled)
		{
			glEnable(GL_BLEND);
			if (m_UseSeparateAlphaBlend)
				glBlendFuncSeparate(m_BlendSrc, m_BlendDst, m_BlendAlphaSrc, m_BlendAlphaDst);
			else
				glBlendFunc(m_BlendSrc, m_BlendDst);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		if (m_CullEnabled)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(m_CullFace);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		glColorMask(m_ColorMaskR, m_ColorMaskG, m_ColorMaskB, m_ColorMaskA);
	}

	void PipelineState::SetAlphaTest(bool value, float cutoff)
	{
		m_AlphaTest = value;
		m_AlphaCutoff = cutoff;
	}

	void PipelineState::SetDepthTest(bool value)
	{
		m_DepthTest = value;
	}

	void PipelineState::SetDepthWrite(bool value)
	{
		m_DepthWrite = value;
	}

	void PipelineState::SetBlendEnabled(bool value)
	{
		m_BlendEnabled = value;
	}

	void PipelineState::SetCullEnabled(bool value)
	{
		m_CullEnabled = value;
	}

	void PipelineState::SetCullFace(GLenum value)
	{
		m_CullFace = value;
	}

	void PipelineState::SetBlendSrc(GLenum value)
	{
		m_BlendSrc = value;
	}

	void PipelineState::SetBlendDst(GLenum value)
	{
		m_BlendDst = value;
	}

	void PipelineState::SetBlendAlphaSrc(GLenum value)
	{
		m_BlendAlphaSrc = value;
		m_UseSeparateAlphaBlend = true;
	}

	void PipelineState::SetBlendAlphaDst(GLenum value)
	{
		m_BlendAlphaDst = value;
		m_UseSeparateAlphaBlend = true;
	}

	void PipelineState::SetColorMask(bool r, bool g, bool b, bool a)
	{
		m_ColorMaskR = r;
		m_ColorMaskG = g;
		m_ColorMaskB = b;
		m_ColorMaskA = a;
	}
}
