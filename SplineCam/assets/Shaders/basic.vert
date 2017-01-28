#version 330 core

layout (location = 0) in vec3 pos;

uniform vec3 posOffset;

void main()
{
	gl_Position = vec4(pos.x + posOffset.x, pos.y + posOffset.y, pos.z + posOffset.z, 1.0f);
}
