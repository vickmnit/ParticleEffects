attribute vec3 aPos;
attribute vec4 aCol;
uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 mvp;
varying vec4 color;
void main()
{
   gl_Position = mvp * projection * modelview * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   color = aCol;
}
