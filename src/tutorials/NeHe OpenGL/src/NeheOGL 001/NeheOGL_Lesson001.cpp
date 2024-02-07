#include "NeheOGL_Lesson001.h"

namespace sge {

void NeheOGL_Lesson001::onDraw() {
	// SGE_LOG("onDraw\n");

	// glViewport's coordinate is same as GDI: origin left-top(0,0), right-bottom(width,height)
	glViewport(0, 0, static_cast<int>(_clientRect.w), static_cast<int>(_clientRect.h));

	glClearColor(0.0f, 0.2f, 0.2f, 0);					// Set Clear Color (Background Color)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen Color And Depth Buffer

	swapBuffers();										// Swap Buffers (Double Buffering)
}

}