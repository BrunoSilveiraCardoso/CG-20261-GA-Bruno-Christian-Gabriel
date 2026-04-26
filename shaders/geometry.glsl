#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;  // Only output 3 vertices, not 6!

in vec3 Normal[];
in vec3 FragPos[];

out vec3 vNormal;
out vec3 vFragPos;
out vec3 vBarycentric;

void main() {
    // Just output the triangle normally with barycentric coordinates
    // The fragment shader will handle drawing the wireframe
    for(int i = 0; i < 3; i++) {
        vNormal = Normal[i];
				vFragPos = FragPos[i];

        // Set barycentric coordinates for edge detection
        vec3 barycentric = vec3(0.0);
        barycentric[i] = 1.0;
        vBarycentric = barycentric;
        
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
