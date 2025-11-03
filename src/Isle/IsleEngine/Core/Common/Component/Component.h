// Component.h

#pragma once

namespace Isle
{
	class Component : public Object
	{
	public:
		virtual void Start() {};
		virtual void Update() {};
		virtual void Destroy() {};
	};
}