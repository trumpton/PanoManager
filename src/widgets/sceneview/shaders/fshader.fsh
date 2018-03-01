uniform sampler2D textureDataSampler2DUniform;
varying vec2 fragmenShaderCoordinatesVec2Varying;

void main(void)
{
    gl_FragColor = texture2D(textureDataSampler2DUniform, fragmenShaderCoordinatesVec2Varying);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) ;  // Debug (set all pixels to red)
}
