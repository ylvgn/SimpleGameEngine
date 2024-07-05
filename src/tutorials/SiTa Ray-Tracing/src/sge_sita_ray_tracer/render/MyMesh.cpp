#include "MyMesh.h"

namespace sge {

void MyMesh::createRect(float w, float h) {
	vertices.resize(4);

	float x = w / 2;
	float y = h / 2;

	auto* v = vertices.data();

	v->pos.set(-x,-y, 0); v->color.set(255, 255, 255, 255); v->uv.set(0,1); v->normal.set(0,0,1); v++;
	v->pos.set( x,-y, 0); v->color.set(255, 255, 255, 255); v->uv.set(1,1); v->normal.set(0,0,1); v++;
	v->pos.set( x, y, 0); v->color.set(255, 255, 255, 255); v->uv.set(1,0); v->normal.set(0,0,1); v++;
	v->pos.set(-x, y, 0); v->color.set(255, 255, 255, 255); v->uv.set(0,0); v->normal.set(0,0,1); v++;

	indices.assign({
		0,1,2, 
		0,2,3,
	});
}

void MyMesh::createCube(float w, float h, float d) {
	vertices.resize(8);

	float x = w / 2;
	float y = h / 2;
	float z = d / 2;

	auto* v = vertices.data();

	v->pos.set(-x,-y, z); v->color.set(255, 255, 255, 255); v->uv.set(0,1); v++;
	v->pos.set( x,-y, z); v->color.set(255, 255, 255, 255); v->uv.set(1,1); v++;
	v->pos.set( x, y, z); v->color.set(255, 255, 255, 255); v->uv.set(1,0); v++;
	v->pos.set(-x, y, z); v->color.set(255, 255, 255, 255); v->uv.set(0,0); v++;
	v->pos.set(-x,-y,-z); v->color.set(255, 255, 255, 255); v->uv.set(0,1); v++;
	v->pos.set( x,-y,-z); v->color.set(255, 255, 255, 255); v->uv.set(1,1); v++;
	v->pos.set( x, y,-z); v->color.set(255, 255, 255, 255); v->uv.set(1,0); v++;
	v->pos.set(-x, y,-z); v->color.set(255, 255, 255, 255); v->uv.set(0,0); v++;

	indices.assign({
		0,1,2, 
		0,2,3,
		2,1,5,
		2,5,6,
		3,2,6,
		3,6,7,
		0,3,7,
		0,7,4,
		1,0,4,
		1,4,5,
		4,7,6,
		4,6,5,
	});
}

void MyMesh::draw() {
	if (indices.size() <= 0)
		return;

	auto stride = static_cast<GLsizei>(sizeof(vertices[0]));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, vertices[0].pos.data);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, vertices[0].color.data);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, stride, vertices[0].uv.data);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, vertices[0].normal.data);

	//---- set render state
	if (!doubleSided)
		glEnable(GL_CULL_FACE);

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//-- draw
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, indices.data());

	//---- restore render state
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (!doubleSided)
		glDisable(GL_CULL_FACE);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void MyMesh::clear() {
	vertices.clear();
	indices.clear();
}

void MyMesh::setColor(const Color4b& c) {
	for (auto& v : vertices) {
		v.color = c;
	}
}

class WavefrontObjReader {
public:
	static void loadFile(MyMesh& outInfo, StrView filename) {
		MemMapFile mm;
		mm.open(filename);
		loadMem(outInfo, mm.span(), filename);
	}

	static void loadMem(MyMesh& outInfo, ByteSpan data, StrView filename) {
		WavefrontObjReader inst;
		inst._loadMem(outInfo, data, filename);
	}

private:
	void _loadMem(MyMesh& outInfo, ByteSpan data, StrView filename) {
		_mesh = &outInfo;
		_mesh->clear();

		_srouce = StrView_make(data);

		auto* p = _srouce.begin();
		auto* ed = _srouce.end();
		while (p < ed) {
			String line;
			p = my_getline(line, p);
			_lineNumber++;

			_readLine(line.c_str());
		}
	}

	void _readLine(const char* p) {
		String token;
		p = my_gettoken(token, p);

		if (token == "v") {
			MyVec3f v;
			float w = 1;
			int ret = sscanf(p, "%f %f %f %f", &v.x, &v.y, &v.z, &w);
			if (ret < 3)
				throw SGE_ERROR("error read v");

			v /= w;
			_v.push_back(v);
			return;
		}

		if (token == "vt") {
			MyVec2f vt;
			int ret = sscanf(p, "%f %f", &vt.x, &vt.y);
			if (ret < 2)
				throw SGE_ERROR("error read vt");

			_vt.push_back(vt);
			return;
		}

		if (token == "vn" ) {
			MyVec3f vn;
			int ret = sscanf(p, "%f %f %f", &vn.x, &vn.y, &vn.z);
			if (ret < 3)
				throw SGE_ERROR("error read vn");

			_vn.push_back(vn);
			return;
		}

		if (token == "f") {
			_readFace(p);
			return;
		}
	}

	void _readFace(const char* p) {
		p = _readFaceVertex(p);
		p = _readFaceVertex(p);
		p = _readFaceVertex(p);
	}

	const char* _readFaceVertex(const char* p) {
		String token;
		p = my_gettoken(token, p);

		int v  = 0;
		int vt = 0;
		int vn = 0;

		auto* c = token.c_str();

		String tmp;
		c = my_gettoken(tmp, c, '/');

		if (sscanf(tmp.c_str(), "%d", &v) < 1)
			throw SGE_ERROR("error read f");

		if (c) {
			c = my_gettoken(tmp, c, '/');
			if (tmp.size()) {
				if (sscanf(tmp.c_str(), "%d", &vt) < 1)
					throw SGE_ERROR("error read f");
			}

			c = my_gettoken(tmp, c, '/');
			if (tmp.size()) {
				if (sscanf(tmp.c_str(), "%d", &vn) < 1)
					throw SGE_ERROR("error read f");
			}
		}

		//---------
		MyVertex_PosColorUv vtx;
		vtx.pos.set(0,0,0);
		vtx.normal.set(0,0,0);
		vtx.uv.set(0,0);
		vtx.color.set(255, 255, 255, 255);

		//---------
		if (v == 0)
			throw SGE_ERROR("error read f v = 0");

		if (v < 0) {
			if (static_cast<size_t>(-v) > _v.size())
				throw SGE_ERROR("error read f v");

			vtx.pos = _v[_v.size() - v];
		} else {
			if (static_cast<size_t>(v) > _v.size())
				throw SGE_ERROR("error read f v");
			vtx.pos = _v[v-1];
		}

		//---------
		if (vt == 0) {
			vtx.uv.set(0,0);
		} else if (vt < 0) {
			if (static_cast<size_t>(-vt) > _vt.size()) 
				throw SGE_ERROR("error read f vt");

			vtx.uv = _vt[_vt.size() - vt];
		} else {
			if (static_cast<size_t>(vt) > _vt.size())
				throw SGE_ERROR("error read f vt");
			vtx.uv = _vt[vt-1];
		}

		//---------
		if (vn == 0) {
			vtx.normal.set(0,0,0);
		} else if (vn < 0) {
			if (static_cast<size_t>(-vn) > _vn.size()) 
				throw SGE_ERROR("error read f vn");

			vtx.normal = _vn[_vn.size() - vn];
		} else {
			if (static_cast<size_t>(vn) > _vn.size())
				throw SGE_ERROR("error read f vn");
			vtx.normal = _vn[vn-1];
		}

		//---------
		_mesh->vertices.push_back(vtx);
		auto vi = _mesh->vertices.size() - 1;

		if (vi > std::numeric_limits<MyMesh::IndexType>::max())
			throw SGE_ERROR("error read f max index excess");

		_mesh->indices.push_back(static_cast<MyMesh::IndexType>(vi));
		return p;
	}

	Vector<MyVec3f> _v;
	Vector<MyVec2f> _vt;
	Vector<MyVec3f> _vn;

	int _lineNumber = 0;

	MyMesh* _mesh = nullptr;
	StrView _srouce;
};

void MyMesh::loadObjFile(const char* filename) {
	WavefrontObjReader::loadFile(*this, filename);
}

}