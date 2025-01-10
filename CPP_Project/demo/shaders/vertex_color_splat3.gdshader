//

//<MACRO>
// simple RGB splat shader that is based on vertex color (by default)
//
// fades between 3 texture sets
//
// 17/07/2024 by Richard Elicott
//


shader_type spatial;

// options
//#define TRIPLANAR // not working yet!
//#define SPLATMAP // use a splatmap instead of vertex color
//#define HUESPLAT // experimental hue based splat

//#define ORM // Occlusion, Roughness, Metallic combined

//#define _NORMAL

//#define _ROUGHNESS
//#define _METALLIC
//#define _EMISSION // needs a seperate map for the colors
//#define _AO


#define DEFAULT_SCALE vec2(1.0, 1.0) // the only way to set default vec2

uniform float normal_map_depth = 1.0;

uniform float normal1_depth = 1.0;
uniform float normal2_depth = 1.0;
uniform float normal3_depth = 1.0;

uniform vec2 uv_offset;
uniform vec2 uv_scale = DEFAULT_SCALE;

uniform sampler2D albedo1: hint_default_black;
uniform sampler2D albedo2: hint_default_black;
uniform sampler2D albedo3: hint_default_black;

uniform sampler2D gradient: hint_default_white;



#ifdef _NORMAL
uniform sampler2D normal1: hint_normal;
uniform sampler2D normal2: hint_normal;
uniform sampler2D normal3: hint_normal;
#endif


#ifdef ORM
uniform sampler2D orm1;
uniform sampler2D orm2;
uniform sampler2D orm3;
#endif

#ifdef _AO
uniform sampler2D ao1: hint_default_white;
uniform sampler2D ao2: hint_default_white;
uniform sampler2D ao3: hint_default_white;
#endif

#ifdef _ROUGHNESS
uniform sampler2D roughness1: hint_default_white;
uniform sampler2D roughness2: hint_default_white;
uniform sampler2D roughness3: hint_default_white;
#endif

#ifdef _METALLIC
uniform sampler2D metallic1: hint_default_black;
uniform sampler2D metallic2: hint_default_black;
uniform sampler2D metallic3: hint_default_black;
#endif

#ifdef _EMISSION
uniform sampler2D emission1: hint_default_black;
uniform sampler2D emission2: hint_default_black;
uniform sampler2D emission3: hint_default_black;
#endif

#ifdef SPLATMAP
uniform sampler2D splatmap: hint_default_black;
uniform vec2 splatmap_offset;
uniform vec2 splatmap_scale;
#endif

//void vertex() {
//
//}

//ifdef HUESPLAT
// https://godotshaders.com/shader/hsv-adjustment/
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

//#endif


varying vec3 vertex_VERTEX;
varying vec3 vertex_NORMAL;

void vertex() {
    // Pass the world position to the fragment shader
    vertex_VERTEX = VERTEX.xyz;
    vertex_NORMAL = NORMAL;
}

uniform float gradient_scale = 1.0;

void fragment() {

    // vertex color based splat shader for vertex painted meshes
    // transparency not used by default

    vec2 uv = UV;
    vec4 color = COLOR; // vertex color

    #ifdef TRIPLANAR
    #endif

    #ifdef HUESPLAT //experimental
    color.rbg = rgb2hsv(color.rgb);
    color.g = 1.0;
    color.b = 1.0;
    color.rbg = hsv2rgb(color.rgb);
    #endif

    uv += uv_offset; // uv scaling (to get final position)
    uv *= uv_scale;

    #ifdef SPLATMAP
    color = texture(splatmap, (uv + splatmap_offset) * splatmap_scale);
    #endif



    #define GENERATED_ALBEDO
    #ifdef GENERATED_ALBEDO


    vec4 water_color = vec4(0,0,1,1);
    vec4 rock_color = vec4(0.5,0.5,0.5,1);
    
    
   

    
    // add each texture based on the vertex color (COLOR)
    vec4 _albedo = texture(albedo1, uv) * color.r;
    _albedo += texture(albedo2, uv) * color.g;
    _albedo += texture(albedo3, uv) * color.b;
    

    float height = vertex_VERTEX.y; // Height in object space (Y coordinate)
     vec4 gradient_color = texture(gradient, vec2(height * gradient_scale, 0.0)) ;
    _albedo = gradient_color;
    
    
    //float val = vertex_NORMAL.y;
    //_albedo = vec4(val,val,val, 1.0);
    
    #endif


    ALBEDO = _albedo.rgb;

    //ALBEDO = color.rgb; // debug vertex color

    #ifdef _NORMAL
    NORMAL_MAP = texture(normal1, uv).rgb * color.x * normal1_depth;
    NORMAL_MAP += texture(normal2, uv).rgb * color.y * normal2_depth;
    NORMAL_MAP += texture(normal3, uv).rgb * color.z * normal2_depth;
    #endif

    NORMAL_MAP_DEPTH = normal_map_depth;

    ROUGHNESS = 1.0; // default value (optional)
    METALLIC = 0.0; // default value (optional)




    #ifdef ORM // using AO, Rough, Metal combined textures

    vec4 _orm1 = texture(orm1, uv) * color.x;
    vec4 _orm2 = texture(orm2, uv) * color.y;
    vec4 _orm3 = texture(orm3, uv) * color.z;

    // i think this is correct
    AO = _orm1.r + _orm2.r + _orm3.r;
    ROUGHNESS = _orm1.g + _orm2.g + _orm3.g;
    METALLIC = _orm1.b + _orm2.b + _orm3.b;

    #endif

    #ifdef _ROUGHNESS
    ROUGHNESS = texture(roughness1, uv).x * color.r;
    ROUGHNESS += texture(roughness2, uv).x * color.g;
    ROUGHNESS += texture(roughness3, uv).x * color.b;
    #endif

    #ifdef _METALLIC
    METALLIC = texture(metallic1, uv).x * color.r;
    METALLIC += texture(metallic2, uv).x * color.g;
    METALLIC += texture(metallic3, uv).x * color.b;
    #endif

    #ifdef _EMISSION
    EMISSION = texture(emission1, uv).rgb * color.r;
    EMISSION += texture(emission2, uv).rgb * color.g;
    EMISSION += texture(emission3, uv).rgb * color.b;
    #endif
    
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
