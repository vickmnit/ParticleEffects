attribute vec3 aPos;
varying vec4 color;
uniform mat4 modelview;
uniform mat4 projection;
void main()
{
   gl_Position = projection * modelview * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
