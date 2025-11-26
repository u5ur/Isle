#include "PipelineState.h"

namespace Isle
{
	static GLenum ToGL(CULL_MODE mode)
	{
		switch (mode)
		{
		case CULL_MODE::FRONT: return GL_FRONT;
		case CULL_MODE::BACK: return GL_BACK;
		case CULL_MODE::FRONT_AND_BACK: return GL_FRONT_AND_BACK;
		default: return 0;
		}
	}

	static GLenum ToGL(BLEND_FACTOR factor)
	{
		switch (factor)
		{
		case BLEND_FACTOR::ZERO: return GL_ZERO;
		case BLEND_FACTOR::ONE: return GL_ONE;
		case BLEND_FACTOR::SRC_COLOR: return GL_SRC_COLOR;
		case BLEND_FACTOR::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
		case BLEND_FACTOR::DST_COLOR: return GL_DST_COLOR;
		case BLEND_FACTOR::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
		case BLEND_FACTOR::SRC_ALPHA: return GL_SRC_ALPHA;
		case BLEND_FACTOR::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
		case BLEND_FACTOR::DST_ALPHA: return GL_DST_ALPHA;
		case BLEND_FACTOR::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
		default: return GL_ONE;
		}
	}

	static GLenum ToGL(COMPARE_FUNC func)
	{
		switch (func)
		{
		case COMPARE_FUNC::NEVER: return GL_NEVER;
		case COMPARE_FUNC::LESS: return GL_LESS;
		case COMPARE_FUNC::EQUAL: return GL_EQUAL;
		case COMPARE_FUNC::LESS_EQUAL: return GL_LEQUAL;
		case COMPARE_FUNC::GREATER: return GL_GREATER;
		case COMPARE_FUNC::NOT_EQUAL: return GL_NOTEQUAL;
		case COMPARE_FUNC::GREATER_EQUAL: return GL_GEQUAL;
		case COMPARE_FUNC::ALWAYS: return GL_ALWAYS;
		default: return GL_LEQUAL;
		}
	}

	void PipelineState::Bind()
	{
		if (m_DepthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		glDepthMask(m_DepthWrite ? GL_TRUE : GL_FALSE);
		glDepthFunc(ToGL(m_DepthFunc));

		if (m_BlendEnabled)
		{
			glEnable(GL_BLEND);

			if (m_UseSeparateAlphaBlend)
			{
				glBlendFuncSeparate(
					ToGL(m_BlendSrc),
					ToGL(m_BlendDst),
					ToGL(m_BlendAlphaSrc),
					ToGL(m_BlendAlphaDst)
				);
			}
			else
			{
				glBlendFunc(ToGL(m_BlendSrc), ToGL(m_BlendDst));
			}
		}
		else
		{
			glDisable(GL_BLEND);
		}

		if (m_CullEnabled && m_CullFace != CULL_MODE::NONE)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(ToGL(m_CullFace));
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		glColorMask(
			m_ColorMaskR ? GL_TRUE : GL_FALSE,
			m_ColorMaskG ? GL_TRUE : GL_FALSE,
			m_ColorMaskB ? GL_TRUE : GL_FALSE,
			m_ColorMaskA ? GL_TRUE : GL_FALSE
		);
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

	void PipelineState::SetCullFace(CULL_MODE value)
	{
		m_CullFace = value;
	}

	void PipelineState::SetBlendSrc(BLEND_FACTOR value)
	{
		m_BlendSrc = value;
	}

	void PipelineState::SetBlendDst(BLEND_FACTOR value)
	{
		m_BlendDst = value;
	}

	void PipelineState::SetBlendAlphaSrc(BLEND_FACTOR value)
	{
		m_BlendAlphaSrc = value;
		m_UseSeparateAlphaBlend = true;
	}

	void PipelineState::SetBlendAlphaDst(BLEND_FACTOR value)
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

	void PipelineState::SetDepthFunc(COMPARE_FUNC func)
	{
		m_DepthFunc = func;
	}
}
