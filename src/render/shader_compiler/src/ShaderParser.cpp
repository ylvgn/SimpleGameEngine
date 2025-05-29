#include "ShaderParser.h"
#include "sge_core/file/MemMapFile.h"

namespace sge {

void ShaderParser::readFile(ShaderInfo& outInfo, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	readMem(outInfo, mm, filename);
}

void ShaderParser::readMem(ShaderInfo& outInfo, ByteSpan data, StrView filename) {
	ShaderParser inst;
	inst._readMem(outInfo, data, filename);
}

void ShaderParser::_readMem(ShaderInfo& outInfo, ByteSpan data, StrView filename) {
	outInfo.clear();
	_outInfo = &outInfo;
	reset(data, filename);
	skipNewlineTokens();

	if (_token.isIdentifier("Shader")) {
		_readShader();
		return;
	}

	error("missing Shader tag");
}

void ShaderParser::_readShader() {
	// token start as 'Shader'
	nextToken();

	// optional shader name
	if (_token.isString()) {
		readString(_outInfo->name);
	}

	expectOperator("{");

	for (;;) {
		if (_token.isOperator("}")) {
			nextToken();
			break;
		}
		if (_token.isNewline()) {
			nextToken();
			continue;
		}
		if (_token.isIdentifier("Properties")) {
			_readProperties();
			continue;
		}
		if (_token.isIdentifier("Pass")) {
			_readPass();
			continue;
		}
		return errorUnexpectedToken();
	}
}

void ShaderParser::_readProperties() {
	// token start as 'Properties'
	nextToken();
	skipNewlineTokens();

	expectOperator("{");
	for (;;) {
		skipNewlineTokens();
		if (_token.isOperator("}")) {
			nextToken();
			break;
		}
		if (_token.isNewline()) {
			nextToken();
			continue;
		}

		_readProperty();
	}
}

void ShaderParser::_readProperty() {
	// token start as '[' or identity
	auto& o = _outInfo->props.emplace_back();

	// optional attribute
	if (_token.isOperator("[")) {
		nextToken(); // skip '['
		while (!_token.isNone()) {
			skipNewlineTokens();

			if (_token.isIdentifier("DisplayName")) {
				nextToken();
				expectOperator("=");
				readString(o.displayName);
			}
			// !!<------ add new token feature here!!

			if (_token.isOperator("]")) {
				nextToken();
				break;
			}
			expectOperator(",");
		}
	}

	skipNewlineTokens();

	{
		// prop type
		readEnum(o.propType);

		// prop name
		readIdentifier(o.name);
	}

	// optional defaultValue
	if (_token.isOperator("=")) {
		nextToken(); // skip '='
		while (!_token.isNewline() && !_token.isNone()) {
			o.defaultValue += _token.str;
			nextToken();
		}
	}

	// final token must be '\n'
	expectNewline();
}

void ShaderParser::_readPass() {
	// token start as 'Pass'
	nextToken();
	skipNewlineTokens();

	auto& o = _outInfo->passes.emplace_back();

	// optional pass name
	if (_token.isString()) {
		readString(o.name);
	}

	expectOperator("{");
	for (;;) {
		skipNewlineTokens();

		if (_token.isOperator("}"))	{ nextToken(); break; }
		if (_token.isNewline())		{ nextToken(); continue; }

		if (_token.isIdentifier("Cull")) { nextToken(); readEnum(o.renderState.cull); continue; }

		if (_token.isIdentifier("DepthTest") ) { nextToken(); readEnum(o.renderState.depthTest.op); continue; }
		if (_token.isIdentifier("DepthWrite")) { nextToken(); readBool(o.renderState.depthTest.writeMask); continue; }
#if 0 // TODO
		if (_token.isIdentifier("StencilTest")) { nextToken(); readEnum(o.renderState.stencilTest.op); continue; }
		if (_token.isIdentifier("StencilRef") ) { nextToken(); readNumber(o.renderState.stencilTest.ref); continue; }
#endif
		if (_token.isIdentifier("BlendRGB")  ) { nextToken(); _readBlendFunc(o.renderState.blend.rgb); continue; }
		if (_token.isIdentifier("BlendAlpha")) { nextToken(); _readBlendFunc(o.renderState.blend.alpha); continue; }

		if (_token.isIdentifier("Wireframe")) { nextToken(); readBool(o.renderState.wireframe); continue; }

		if (_token.isIdentifier("VsFunc")) { nextToken(); readIdentifier(o.vsFunc); continue; }
		if (_token.isIdentifier("PsFunc")) { nextToken(); readIdentifier(o.psFunc); continue; }
		if (_token.isIdentifier("CsFunc")) { nextToken(); readIdentifier(o.csFunc); continue; }
		return errorUnexpectedToken();
	}
}

void ShaderParser::_readBlendFunc(RenderState::BlendFunc& v) {
	readEnum(v.op);
	readEnum(v.srcFactor);
	readEnum(v.dstFactor);
}

} // namespace sge
