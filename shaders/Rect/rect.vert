#version 450
layout(push_constant) uniform HUDQuad {
    vec4 pos_rect;
    vec4 uv_rect;
    mat4 vp;
} pc;

layout(location=0) out vec2 frag_uv;

void main() {
    vec2 pos[4] = vec2[4](
        vec2(pc.pos_rect.x, pc.pos_rect.y),
        vec2(pc.pos_rect.x + pc.pos_rect.z, pc.pos_rect.y),
        vec2(pc.pos_rect.x, pc.pos_rect.y - pc.pos_rect.w),
        vec2(pc.pos_rect.x + pc.pos_rect.z, pc.pos_rect.y - pc.pos_rect.w)
        
    );
    vec2 p = pos[gl_VertexIndex];
    gl_Position = pc.vp * vec4(p, 0, 1.0);

    vec2 uv[4] = vec2[4](
        vec2(pc.uv_rect.x, pc.uv_rect.y),
        vec2(pc.uv_rect.z, pc.uv_rect.y),
        vec2(pc.uv_rect.x, pc.uv_rect.w),
        vec2(pc.uv_rect.z, pc.uv_rect.w)
    );
    frag_uv = uv[gl_VertexIndex];
}