#include <Core/Common/Common.h>

namespace Isle
{
	class Triangle
	{
	public:
		int m_Id = -1;
		glm::vec3 a, b, c;
	};

	class ConvexHull : public SceneComponent
	{
	public:
		std::vector<Triangle> m_Triangles;


	};
}