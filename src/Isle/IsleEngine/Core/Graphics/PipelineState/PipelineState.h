// PipelineState.h
#pragma once
#include <Core/Common/Common.h>
#include <Core/Graphics/GfxResource/GfxResource.h>

namespace Isle
{
	enum class CULL_MODE
	{
		NONE,
		FRONT,
		BACK,
		FRONT_AND_BACK
	};

	enum class BLEND_FACTOR
	{
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA
	};

	enum class COMPARE_FUNC
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_EQUAL,
		ALWAYS
	};

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

		CULL_MODE m_CullFace = CULL_MODE::BACK;
		BLEND_FACTOR m_BlendSrc = BLEND_FACTOR::SRC_ALPHA;
		BLEND_FACTOR m_BlendDst = BLEND_FACTOR::ONE_MINUS_SRC_ALPHA;
		BLEND_FACTOR m_BlendAlphaSrc = BLEND_FACTOR::ONE;
		BLEND_FACTOR m_BlendAlphaDst = BLEND_FACTOR::ONE_MINUS_SRC_ALPHA;
		COMPARE_FUNC m_DepthFunc = COMPARE_FUNC::LESS_EQUAL;

	public:
		void Bind();

		void SetAlphaTest(bool value, float cutoff = 0.5f);
		void SetDepthTest(bool value);
		void SetDepthWrite(bool value);
		void SetBlendEnabled(bool value);
		void SetCullEnabled(bool value);
		void SetCullFace(CULL_MODE value);
		void SetBlendSrc(BLEND_FACTOR value);
		void SetBlendDst(BLEND_FACTOR value);
		void SetBlendAlphaSrc(BLEND_FACTOR value);
		void SetBlendAlphaDst(BLEND_FACTOR value);
		void SetColorMask(bool r, bool g, bool b, bool a);
		void SetDepthFunc(COMPARE_FUNC func);
	};
}