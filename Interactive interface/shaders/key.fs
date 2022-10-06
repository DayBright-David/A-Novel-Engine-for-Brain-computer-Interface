#version 330 core
out vec4 FragColor;
uniform float grayScale;
void main()
{
   FragColor = vec4(grayScale, grayScale, grayScale, 1.0f);
}