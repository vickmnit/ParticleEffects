attribute vec3 aPos;
attribute vec4 aCol;
varying vec4 color;
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   color = aCol;
}
