varying vec2 texCoord;
varying vec4 color;
uniform sampler2D ourTexture;
void main()
{
	gl_FragColor = color * texture2D(ourTexture, texCoord);
}
