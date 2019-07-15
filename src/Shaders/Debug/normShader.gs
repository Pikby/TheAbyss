#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;




const float MAGNITUDE = 0.3;

void GenerateLine(int index)
{

  vec3 norm = -normalize(cross(gl_in[0].gl_Position.xyz-gl_in[1].gl_Position.xyz,gl_in[0].gl_Position.xyz-gl_in[2].gl_Position.xyz));
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(vs_out[index].FinNormal, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}
