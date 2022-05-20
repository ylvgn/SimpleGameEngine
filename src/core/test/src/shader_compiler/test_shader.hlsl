Shader "Mytest" {
	Properties {
		Color4f color = {1,1,1,1}
		float f .2f
		int i = 1e5; // hello
	}

	/*
		blablabla
		blablab1
	*/

	Pass {
		VsFunc VShader // vertex shader
		PsFunc PShader /* pixel shader */
	}
}