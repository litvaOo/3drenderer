#include "texture.h"
#include "vector.h"

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
  vec2_t ac = vec2_sub(c, a);
  vec2_t ab = vec2_sub(b, a);
  vec2_t ap = vec2_sub(p, a);
  vec2_t pc = vec2_sub(c, p);
  vec2_t pb = vec2_sub(b, p);

  float full_s = (ac.x * ab.y - ac.y * ab.x);
  vec3_t res;
  res.x = (pc.x * pb.y - pc.y * pb.x) / full_s;
  res.y = (ac.x * ap.y - ap.x * ac.y) / full_s;
  res.z = 1.0 - res.x - res.y;
  return res;
}

tex2_t tex2_clone(tex2_t *t) { return (tex2_t){t->u, t->v}; }
