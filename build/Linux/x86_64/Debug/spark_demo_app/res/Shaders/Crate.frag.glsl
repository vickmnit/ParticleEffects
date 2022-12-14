varying vec2 texCoord;
uniform sampler2D crateTexture;
void main()
{
   gl_FragColor = texture2D(crateTexture, texCoord);
}
