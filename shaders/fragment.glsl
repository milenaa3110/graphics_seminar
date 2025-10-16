#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in float Distance;

uniform vec3 particleColor;
uniform sampler2D cloudMask;

void main() {
    vec4 mask = texture(cloudMask, TexCoord);

    if (mask.a < 0.1)
        discard;

    float distanceFade = clamp(1.0 - (Distance / 30.0), 0.0, 1.0);

    FragColor = vec4(particleColor, mask.a * 0.7 * distanceFade);
}
