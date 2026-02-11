#version 450 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 TexCoord;
flat in uint instanceIndex;

struct InstanceData {
    vec4 color;
    
    vec2 position;
	int useWorldPos;
	int hasTexture;

    int useDepth;
    int pad[3];
    
    sampler2D tex;
    vec2 size;

    vec4 source;
};

layout(std430, binding = 0) buffer InstanceBuffer{ InstanceData instances[]; };

uniform vec2 start;
uniform vec2 end;
uniform float pixelSize;
uniform bool tight;

bool line(vec2 p, vec2 p1, vec2 p2, float width) {
    vec2 lineDir = normalize(p2 - p1);
    vec2 pointDir = p - p1;
    float projection = dot(pointDir, lineDir);
    vec2 closestPoint = p1 + projection * lineDir;
    float dist = length(p - closestPoint);
    return dist <= width;
}

bool parabola(vec2 p, vec2 p1, vec2 p2, float width) {
    float a = (p2.y - p1.y) / ((p2.x - p1.x) * (p2.x - p1.x));
    float parabolaY = a * (p.x - p1.x) * (p.x - p1.x) + p1.y;
    float dist = abs(p.y - parabolaY);
    return dist <= width;
}

bool parabolaX(vec2 p, vec2 p1, vec2 p2, float width) {
    float a = (p2.y - p1.y) / ((p2.x - p1.x) * (p2.x - p1.x));
    float discriminant = (p.y - p1.y) / a;
    if (discriminant < 0)
        return false;

    float x1 = p1.x + sqrt(discriminant);
    float x2 = p1.x - sqrt(discriminant);
    return (abs(p.x - x1) <= width || abs(p.x - x2) <= width);
}

bool sqrtCurve(vec2 p, vec2 p1, vec2 p2, float width) {
    float normX = (p.x - p1.x) / (p2.x - p1.x);
    float sqrtY = sqrt(normX);
    sqrtY = mix(p1.y, p2.y, sqrtY);
    float dist = abs(p.y - sqrtY);
    return dist <= width;
}

bool sqrtCurveX(vec2 p, vec2 p1, vec2 p2, float width) {
    float normY = (p.y - p1.y) / (p2.y - p1.y);
    float normX = normY * normY;
    float x = p1.x + (p2.x - p1.x) * normX;
    float dist = abs(p.x - x);
    return dist <= width;
}

void main() {
    InstanceData data = instances[instanceIndex];

    vec2 p1 = start * pixelSize;
    vec2 p2 = end * pixelSize;
    vec2 size = abs(end - start);

    bool lineAlpha = false;
    
    float blockSize = pixelSize;

    vec2 roundedLoc = floor(gl_FragCoord.xy / blockSize) * blockSize;

    float width = 1.499f;
    
    if (tight) // Linear
        lineAlpha = line(roundedLoc, p1, p2, width);
    else {// Parabola / sqrt
        // using || to account for the steepness, prolly a more optimal solution but this works for now
        if (p2.y > p1.y)
            lineAlpha = parabola(roundedLoc, p1, p2, width) || parabolaX(roundedLoc, p1, p2, width);
        else
            lineAlpha = sqrtCurve(roundedLoc, p1, p2, width) || sqrtCurveX(roundedLoc, p1, p2, width);
    }

    FragColor = vec4(data.color.xyz, lineAlpha);
}