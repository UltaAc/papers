uniform float progress;
uniform int direction;
uniform int alignment;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

vec4 getFromColor (vec2 uv) {
  return GskTexture(u_texture1, uv);
}

vec4 getToColor (vec2 uv) {
  return GskTexture(u_texture2, uv);
}

// License: MIT
// Author: Qiu Wenbo

vec4 transition(vec2 uv) {
  vec2 p=uv.xy/vec2(1.0).xy;
  vec4 a=getFromColor(p);
  vec4 b=getToColor(p);

  float r = mix(p.x, p.y, float(alignment));
  float half_progress = progress / 2.0;
  float interpolate = mix(1.0 - step(half_progress, r) * step(r, 1.0 - half_progress),
                          step(0.5 - half_progress, r) * step(r, 0.5 + half_progress),
                          float(direction));

  return mix(a, b, interpolate);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord, in vec2 resolution, in vec2 uv)
{
  fragColor = transition(uv);
}
