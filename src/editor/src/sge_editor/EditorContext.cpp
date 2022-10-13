#include "EditorContext.h"

namespace sge {

EditorContext* EditorContext::s_instance = nullptr;

EditorContext* EditorContext::createContext()
{
	SGE_ASSERT(s_instance == nullptr);
	s_instance = new EditorContext();
	return s_instance;
}

void EditorContext::destroyContext()
{
	if (s_instance != nullptr) {
		s_instance->~EditorContext();
		s_instance = nullptr;
	}
}

} // namespace