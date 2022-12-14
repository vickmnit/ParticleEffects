attribute vec3 aPos;
attribute vec2 aTexCoord;
uniform mat4 modelview;
uniform mat4 projection;
varying vec2 texCoord;
void main()
{
   gl_Position = projection * modelview * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	texCoord = aTexCoord;
}
