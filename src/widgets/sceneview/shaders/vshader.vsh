attribute vec4 vertexCoordinatesVec4Attribute;
attribute vec2 textureCoordinatesVec2Attribute;
uniform mat4 modelViewProjectionMat4Uniform;
varying vec2 fragmenShaderCoordinatesVec2Varying;

void main(void)
{
    gl_Position = modelViewProjectionMat4Uniform * vertexCoordinatesVec4Attribute;
    fragmenShaderCoordinatesVec2Varying = textureCoordinatesVec2Attribute;
}

