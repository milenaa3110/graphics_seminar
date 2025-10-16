#version 410 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;
out float Distance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec3 CameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 CameraUp = vec3(view[0][1], view[1][1], view[2][1]);
    vec3 vertexPosition =
    vec3(model * vec4(0.0, 0.0, 0.0, 1.0)) +
    CameraRight * aPos.x +
    CameraUp * aPos.y;
    gl_Position = projection * view * vec4(vertexPosition, 1.0);
    TexCoord = aPos.xy + 0.5;
    vec4 viewPos = view * model * vec4(0.0, 0.0, 0.0, 1.0);
    Distance = abs(viewPos.z);
}
