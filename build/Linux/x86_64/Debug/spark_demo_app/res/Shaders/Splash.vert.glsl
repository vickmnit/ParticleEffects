attribute vec3 aPos;
attribute vec4 aCol;
attribute vec2 aTexCoord;
uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 mvp;
varying vec2 texCoord;
varying vec4 color;
void main()
{
   gl_Position = mvp * projection * modelview * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	texCoord = aTexCoord;
	color = aCol;
}
