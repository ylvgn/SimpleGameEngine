#include "RendererSystem.h"
#include "components/CRenderer.h"

namespace sge {

RendererSystem* RendererSystem::s_instance = nullptr;

RendererSystem* RendererSystem::createSystem() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = new RendererSystem();
	return s_instance;
}

void RendererSystem::destroySystem() {
	if (s_instance) {
		s_instance->~RendererSystem();
		s_instance = nullptr;
	}
}

void RendererSystem::remove(CRenderer* c) {
	int index = -1;
	for (int i = 0; i < _renderers.size(); ++i) {
		if (index > 0) {
			_renderers[index++] = _renderers[i];
			continue;
		}
		if (_renderers[i] == c) {
			index = i;
		}
	}
	if (index > 0) {
		_renderers.pop_back();
	}
}

void RendererSystem::render(RenderRequest& req, const Math::Camera3f& camera) {
	// test
	static MyFrustum frustum(camera);
	frustum.render(req);

	// https://iquilezles.org/articles/frustumcorrect/
	struct CameraHelper {
		static bool isPointInFrontOfPlane(const Math::Plane3f& plane, const Vec3f& p) {
			return p.dot(plane.normal) >= plane.distance;
		}

		static bool isPointsInsidePlanes(const Vector<Math::Plane3f, 6>& planes, const Vector<Vec3f, 8>& points) {
			for (auto& plane : planes)
			{
				int out = 0;
				for (auto& point : points) {
					if (isPointInFrontOfPlane(plane, point)) out++;
				}
				if (out == 8) return false;
			}
			return true;
		}

		static bool boxInFrustum(const MyFrustum& frustum, const OBB& obb) {
			return isPointsInsidePlanes(frustum.planes, obb.points()) || isPointsInsidePlanes(obb.planes(), frustum.points);
		}
	};

	for (auto& r : renderers()) {
		if (!CameraHelper::boxInFrustum(frustum, r->getOBB())) continue;
		r->render(req);
	}
}

void RendererSystem::add(CRenderer* c) {
	_renderers.emplace_back(c);
}

}

