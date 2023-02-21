#version 430 core
out vec4 FragColor;

in vec4 vertexPos; // the input variable from the vertex shader (same name and same type)
uniform vec4 vertexColor;
uniform vec4 centerPoint;
uniform int dataSize;
uniform float epsilon = 0.003;
layout(std430, binding = 3) buffer layoutName
{
     float data[];
};
void main()
{
    vec3 color = vec3(.0);
    vec2 vPos = vertexPos.xy;

    float minDist = 2;
    vec3 minColor = vec3(1, 1, 1);

    for(int i = 0;i < dataSize;i++) {
        int index = i * 5;
        vec2 cPos = vec2(data[index], data[index + 1]);
        vec3 vCol = vec3(data[index + 2], data[index + 3], data[index + 4]);

        float dist = distance(vPos, cPos);
        if(dist < minDist) {
            if(minDist - dist < epsilon) minColor = vec3(0,0,0);
            else minColor = vec3(1,1,1);

            minDist = dist;
        } else if(dist - minDist < epsilon) minColor = vec3(0, 0, 1);
    }

    if(minDist < .4) color += minColor;
    if(minDist < 0.007) color = vec3(0, 0, 0);

    //FragColor = vec4(data[0], 0, 0, 1 - step(0.8, dist));
    FragColor = vec4(color, 1);
}
