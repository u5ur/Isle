// IsleEditor.cpp
#include <Core/EditorApplication/EditorApplication.h>

int main()
{
    Isle::EditorApplication::Instance()->Start();

    while (!Isle::EditorApplication::Instance()->GetWindow()->ShouldClose())
    {
        Isle::EditorApplication::Instance()->Update();
    }

    Isle::EditorApplication::Instance()->Destroy();
    return 0;
}