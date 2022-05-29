#include "ShaderParser.h"
#include "sge_core/file/MemMapFile.h"

namespace sge {

void ShaderParser::loadFile(ShaderInfo& outInfo, StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	loadMem(outInfo, mm, filename);
}

void ShaderParser::loadMem(ShaderInfo& outInfo, ByteSpan data, StrView filename) {
	ShaderParser inst;
	inst._loadMem(outInfo, data, filename);
}

void ShaderParser::_loadMem(ShaderInfo& outInfo, ByteSpan data, StrView filename) {
	outInfo.clean();
	_outInfo = &outInfo;
	reset(data, filename);

	// skip #if 0
	while (!_token.isIdentifier("Shader")) {
		nextToken();
	}

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
			if (_token.isOperator("]")) {
				nextToken();
				break;
			}
			//expectOperator(","); why
		}
	}

	skipNewlineTokens();

	{
		// prop type
		_readEnum(o.propType);

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
		if (_token.isOperator("}")) {
			nextToken();
			break;
		}
		if (_token.isNewline()) {
			nextToken();
			continue;
		}
		if (_token.isIdentifier("VsFunc")) {
			nextToken();
			readIdentifier(o.vsFunc);
			continue;
		}
		if (_token.isIdentifier("PsFunc")) {
			nextToken();
			readIdentifier(o.psFunc);
			continue;
		}
		return errorUnexpectedToken();
	}
}

} // namespace
