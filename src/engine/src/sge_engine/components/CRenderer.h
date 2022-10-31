#pragma once

#include "Component.h"
#include <sge_render.h>

namespace sge {

struct MyFrustum : public NonCopyable
{
	MyFrustum(const Math::Camera3f& c) {
		const Vec3f& pos = c.pos();
		const Vec3f& up = c.up().normalize();
		const Vec3f& forward = (c.aim() - pos).normalize();
		const Vec3f& right = forward.cross(up);

		float fov = c.fov();
		float nearClip = c.nearClip();
		float farClip = c.farClip();
		float farHalfHeight = farClip * Math::tan(fov / 2);
		float nearHalfHeight = nearClip * Math::tan(fov / 2);

		planes.resize(6);
		Vec3f nearCenter = pos + forward * nearClip;
		Vec3f farCenter = pos + forward * farClip;

		planes.emplace_back(Math::Plane3f(forward, nearCenter.length()));	// back plane
		planes.emplace_back(Math::Plane3f(forward, farCenter.length()));	// front plane

		{ // top plane
			Vec3f p1 = nearCenter + up * nearHalfHeight;
			Vec3f p2 = farCenter + up * farHalfHeight;
			Vec3f center = (p2 + p1) / 2;
			Vec3f normal = right.cross(p2 - p1).normalize();
			planes.emplace_back(Math::Plane3f(normal, center.length()));
		}

		{ // bottom plane
			Vec3f p1 = nearCenter - up * nearHalfHeight;
			Vec3f p2 = farCenter - up * farHalfHeight;
			Vec3f center = (p2 + p1) / 2;
			Vec3f normal = right.cross(p2 - p1).normalize();
			planes.emplace_back(Math::Plane3f(normal, center.length()));
		}

		{ // right plane
			Vec3f p1 = nearCenter + right * nearHalfHeight;
			Vec3f p2 = farCenter + right * farHalfHeight;
			Vec3f center = (p2 + p1) / 2;
			Vec3f normal = right.cross(p2 - p1).normalize();
			planes.emplace_back(Math::Plane3f(normal, center.length()));
		}

		{ // left plane
			Vec3f p1 = nearCenter - right * nearHalfHeight;
			Vec3f p2 = farCenter - right * farHalfHeight;
			Vec3f center = (p2 + p1) / 2;
			Vec3f normal = right.cross(p2 - p1).normalize();
			planes.emplace_back(Math::Plane3f(normal, center.length()));
		}

		//-----------------------------
		static const float dx[4] = { -1, -1, 1, 1 };
		static const float dy[4] = { -1, 1, -1, 1 };
		for (int i = 0; i < 4; i++) {
			Vec3f dir = right * dx[i] + up * dy[i];
			points.emplace_back(nearCenter + dir * nearHalfHeight);
			points.emplace_back(farCenter + dir * farHalfHeight);
		}
#if 1 //-------------------------------------- debug
		auto* renderer = Renderer::instance();
		
		{ // material
			auto shader = Renderer::instance()->createShader("Assets/Shaders/test.shader");
			_material = renderer->createMaterial();
			_material->setShader(shader);
			_material->setParam("test_float", 1.0f);
			_material->setParam("test_color", Color4f(1, 1, 1, 1));
		}

		{ // vertex buffer
			using Vertex = Vertex_PosColorNormalUv<8>;
			_vertexLayout = Vertex::s_layout();
			Vector<Vertex, 8> vertexData;
			vertexData.resize(points.size());
			size_t i = 0;
			for (auto& p : points) {
				vertexData[i].pos = p;
				vertexData[i].color[0].set(255, 255, 255, 255);
				i++;
			}
			_vertexCount = vertexData.size();
			RenderGpuBuffer::CreateDesc desc;
			desc.type = RenderGpuBufferType::Vertex;
			desc.bufferSize = points.size() * _vertexLayout->stride;
			_vertexBuffer = renderer->createGpuBuffer(desc);
			_vertexBuffer->uploadToGpu(spanCast<u8>(vertexData.span()));
		}

		{ // index buffer
			using VertexIndex = u16;
			Vector<VertexIndex, 36>	indexData {
				0,2,6, 0,6,4, // Near
				3,1,7, 5,7,1, // Far
				0,3,2, 0,1,3, // Right
				4,6,7, 4,7,5, // Left
				2,7,3, 2,7,6, // Down
				0,5,1, 0,4,5, // Top
			};
			_indexCount = indexData.size();
			auto byteSpan = spanCast<const u8>(indexData.span());
			RenderGpuBuffer::CreateDesc desc;
			desc.type = RenderGpuBufferType::Index;
			desc.bufferSize = byteSpan.size();
			_indexCount = indexData.size();
			_indexBuffer = renderer->createGpuBuffer(desc);
			_indexBuffer->uploadToGpu(byteSpan);
		}
#endif
	}

	// debug
	void render(RenderRequest& req) {
		if (!_material) { SGE_ASSERT(false); return; }
		req.setMaterialCommonParams(_material);

		auto m = Mat4f::s_TS(req.camera_pos, { 1,1,1 }); // temp
		auto mvp = req.matrix_vp * m;
		_material->setParam("sge_matrix_mvp", mvp);

		auto passes = _material->passes();
		for (size_t i = 0; i < passes.size(); i++) {
			auto* cmd = req.addDrawCall();
#if _DEBUG
			cmd->debugLoc = SGE_LOC;
#endif
			cmd->material			= _material;
			cmd->materialPassIndex	= i;
			cmd->primitive			= RenderPrimitiveType::Triangles;
			cmd->vertexLayout		= _vertexLayout;
			cmd->vertexBuffer		= _vertexBuffer;
			cmd->vertexCount		= _vertexCount;
			cmd->indexBuffer		= _indexBuffer;
			cmd->indexType			= RenderDataType::UInt16;
			cmd->indexCount			= _indexCount;
		}
	}

	Vector<Vec3f, 8> points;
	Vector<Math::Plane3f, 6> planes;

private:
#if 1 // debug
	const VertexLayout*		_vertexLayout = nullptr;
	SPtr<RenderGpuBuffer>	_vertexBuffer;
	SPtr<RenderGpuBuffer>	_indexBuffer;
	size_t _indexCount		= 0;
	size_t _vertexCount		= 0;
	SPtr<Material>			_material;
#endif
};

struct AABB {
	AABB() = default;
	AABB(const AABB& aabb) {
		min = aabb.min;
		max = aabb.max;
		_points.clear();
		_planes.clear();
	}

	void update(const Vec3f& p) {
		min.x = Math::min(min.x, p.x);
		min.y = Math::min(min.y, p.y);
		min.z = Math::min(min.z, p.z);

		max.x = Math::max(max.x, p.x);
		max.y = Math::max(max.y, p.y);
		max.z = Math::max(max.z, p.z);
	}

	AABB(const EditMesh& editMesh) {
		min = max = editMesh.pos[0];
		for (auto& p : editMesh.pos) {
			update(p);
		}
	}

	const Vector<Vec3f, 8>& points() const {
		if (_points.size() == 0) {
			_calcPoints();
		}
		return _points;
	}

	const Vector<Math::Plane3f, 6>& planes() const {
		if (_planes.size() == 0) {
			_calcPlanes();
		}
		return _planes;
	}

	Vec3f min{ 0,0,0 };
	Vec3f max{ 0,0,0 };

protected:

	void _calcPoints() const {
		auto& ps = constCast<Vector<Vec3f, 8>>(_points);
		ps.reserve(8);
		ps.emplace_back(Vec3f{ min.x, min.y, min.z }); // [0]
		ps.emplace_back(Vec3f{ min.x, max.y, min.z }); // [1]
		ps.emplace_back(Vec3f{ min.x, min.y, max.z }); // [2]
		ps.emplace_back(Vec3f{ min.x, max.y, max.z }); // [3]
		ps.emplace_back(Vec3f{ max.x, max.y, max.z }); // [4]
		ps.emplace_back(Vec3f{ max.x, min.y, max.z }); // [5]
		ps.emplace_back(Vec3f{ max.x, max.y, min.z }); // [6]
		ps.emplace_back(Vec3f{ max.x, min.y, min.z }); // [7]
	}

	void _calcPlanes() const {
		const auto& ps = points();
		auto& pls = constCast<Vector<Math::Plane3f, 6>>(_planes);
		pls.reserve(6);
		pls.emplace_back(Math::Triangle3f(ps[0], ps[1], ps[2])); // L
		pls.emplace_back(Math::Triangle3f(ps[2], ps[3], ps[4])); // F
		pls.emplace_back(Math::Triangle3f(ps[3], ps[1], ps[4])); // T
		pls.emplace_back(Math::Triangle3f(ps[5], ps[6], ps[7])); // R
		pls.emplace_back(Math::Triangle3f(ps[7], ps[6], ps[0])); // B
		pls.emplace_back(Math::Triangle3f(ps[2], ps[5], ps[7])); // D
	}

	Vector<Vec3f, 8> _points;
	Vector<Math::Plane3f, 6> _planes;
};

struct OBB : public AABB {
	OBB() = default;
	OBB(const AABB& aabb_) : AABB(aabb_) {};
	OBB(const AABB& aabb_, Mat4f m) : OBB(aabb_) {
		min = m.mulPoint4x3(min);
		max = m.mulPoint4x3(max);
	}
};

class CRenderer : public Component {
	SGE_OBJECT_TYPE(CRenderer, Component)
public:
	CRenderer();
	~CRenderer();

	void render(RenderRequest& req) { onRender(req); }
	const OBB getOBB() const { return onGetOBB(); }

protected:
	virtual void onRender(RenderRequest& req) {}
	virtual const OBB onGetOBB() const { return OBB(); }
};

}