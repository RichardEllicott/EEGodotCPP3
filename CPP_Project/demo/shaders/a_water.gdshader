/*

from:
https://godotshaders.com/shader/another-water-shader/

had no actual usage of foam_noise, trying this, for best results set noise with gradient going black white black

*/


shader_type spatial;
//render_mode cull_disabled;
//render_mode cull_disabled, depth_draw_always, blend_mix;
render_mode cull_disabled;



// functions for sin -> gernster adapted from:
// https://www.shadertoy.com/view/XdByzm

// main function seems to be S2GW_SinToGWave
// no idea why such a mess!!

const float Pi = 3.14159265359;

float S2GW_InvLerp(float _a, float _b, float _r) { return (_a - _r) / (_a - _b); }

float S2GW_Pow4(float _x){
	float _x2 = _x * _x;
    return _x2 * _x2;
	}

float S2GW_Root4(float _x){
    //return rsqrt(rsqrt(x));
	return pow(_x, 0.25);
	}

float S2GW_Pow4Pow2Lerp(float _x, float _l){
	float _x2 = _x * _x;
    return (_l + (1.0 - _l) * _x2) * _x2;
	}

float S2GW_InvPow4Pow2Lerp(float _x, float _l){
    return sqrt(0.5 * (_l - sqrt(_l * _l + (4.0 - 4.0 * _l) * _x))/(_l - 1.0));
	}

float S2GW_SinToGWave00(float x, float l){
    return 1.0 - S2GW_InvPow4Pow2Lerp(1.0 - x, l);
	}

float S2GW_SinToGWave0(float x, float s){
    float l = mix(0.001, 0.08, s);
    float ss = 1.0 - S2GW_Pow4Pow2Lerp(1.0 - s, l);
    return S2GW_SinToGWave00(x * ss, l);
	}

float S2GW_SinToGWave0N(float x, float s){
    return S2GW_SinToGWave0(x, s) / s;
}

float S2GW_SinToGWave(float x, float s){
    return S2GW_SinToGWave0(x * 0.5 + 0.5, s) * 2.0 - s;
}

float S2GW_SinToGWaveN(float x, float s){
    return S2GW_SinToGWave0(x * 0.5 + 0.5, s) / s * 2.0 - 1.0;
}

float S2GW_InvGW(float y, float s){
    return acos(y/s) - sqrt(s*s - y*y);
}

float S2GW_InvGW0(float y, float s){
    return S2GW_InvGW((y * 2.0 - 1.0) * s, s) / Pi;
}



uniform sampler2D DEPTH_TEXTURE : hint_depth_texture, repeat_disable, filter_nearest;

uniform float depth_fade_distance : hint_range(0.0, 10.0) = 1.0;
uniform float absorbance : hint_range(0.0, 10.0) = 2.0;

uniform vec3 shallow_color : source_color = vec3(0.22, 0.66, 1.0);
uniform vec3 deep_color : source_color = vec3(0.0, 0.25, 0.45);

uniform float foam_amount : hint_range(0.0, 2.0) = 0.2;
uniform vec3 foam_color : source_color = vec3(1);
uniform sampler2D foam_noise : hint_default_white;
uniform vec2 foam_scale = vec2(1,1);

uniform float roughness : hint_range(0.0, 1.0) = 0.05;

uniform sampler2D wave_texture;

uniform float wave_scale = 4.0; // scale the textures
uniform float height_scale = 0.15;
varying float wave_height;
varying vec3 uv_world_pos;

uniform sampler2D normal1;
uniform vec2 wave_dir1 = vec2(1.0, 0.0);
uniform sampler2D normal2;
uniform vec2 wave_dir2 = vec2(0.0, 1.0);
uniform float wave_speed : hint_range(0.0, 0.2) = 0.015;

vec3 screen(vec3 base, vec3 blend){
	return 1.0 - (1.0 - base) * (1.0 - blend);
}


float skewed_sin(float x, float bias) {
    float warped_x = x + bias * sin(x);
    return sin(warped_x);
}


uniform float extra_wave_height = 0.25;
uniform float extra_wave_gernster = 0.5; // -1 is off
uniform float extra_wave_skew = 0.5;
uniform float extra_wave_speed = 0.5;
uniform vec3 extra_wave_direction = vec3(1,0,1);

void vertex() {
	// Vertext displacement for waves
	uv_world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	wave_height = texture(wave_texture, uv_world_pos.xz / wave_scale + TIME * wave_speed).r;
	//VERTEX.y += wave_height * height_scale;
    
    
    
    
    
    // my gernster
    float wave_pos = dot(extra_wave_direction, uv_world_pos); // using dot product to allow wave direction
    //wave_pos = uv_world_pos.x; // test just x
        
    //float sin_pos = sin((TIME + wave_pos) * wave_speed2 + 1.0);  // Higher frequency and phase shift
    float sin_pos = skewed_sin((TIME + wave_pos) * extra_wave_speed + 1.0, extra_wave_skew);  // Higher frequency and phase shift

    if (extra_wave_gernster > -1.0){
        sin_pos = S2GW_SinToGWave(sin_pos, extra_wave_gernster);
    }
    // add sine
    VERTEX.y += sin_pos * extra_wave_height;
    
    
    
}

uniform sampler2D reflection_probe : hint_default_white; // Use a 2D texture for reflection probe data




void fragment()
{
    
    
    
    
	// Depth texture magic
	float depth = texture(DEPTH_TEXTURE, SCREEN_UV, 0.0).r;
  	vec3 ndc = vec3(SCREEN_UV * 2.0 - 1.0, depth);
	vec4 world = INV_VIEW_MATRIX * INV_PROJECTION_MATRIX * vec4(ndc, 1.0);
	float depth_texture_y = world.y / world.w;
	float vertex_y = (INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).y;
	float vertical_depth = vertex_y - depth_texture_y;

	// Changes the color of geometry behind it as the water gets deeper
	float depth_fade_blend = exp(-vertical_depth / depth_fade_distance);
	depth_fade_blend = clamp(depth_fade_blend, 0.0, 1.0);

	// Makes the water more transparent as it gets more shallow
	float alpha_blend = -vertical_depth * absorbance;
	alpha_blend = clamp(1.0 - exp(alpha_blend), 0.0, 1.0);

	// Small layer of foam
    
    // added texture support (wasn't there)
    
    vec2 foam_speed = vec2(1,-0.25) / 8.0;
    vec2 foam_uv = uv_world_pos.xz / foam_scale + foam_speed * TIME;
    vec4 foam_sample = texture(foam_noise, foam_uv);
    
    float depth_offset = -foam_sample.r / 8.0;
    
	float foam_blend = clamp(1.0 - ((vertical_depth +depth_offset) / foam_amount), 0.0, 1.0);
	//vec3 foam = foam_blend * foam_color * foam_sample.rgb;
	vec3 foam = foam_blend * foam_color * foam_color.rgb;


	// Mix them all together
	vec3 color_out = mix(deep_color, shallow_color, depth_fade_blend);
	color_out = screen(color_out, foam);

	// Normal maps
	vec2 normal_offset1 = (TIME * wave_dir1) * wave_speed;
	vec2 normal_offset2 = (TIME * wave_dir2) * wave_speed;
	vec3 normal_blend = mix(texture(normal1, uv_world_pos.xz / wave_scale + normal_offset1), texture(normal2, uv_world_pos.xz / wave_scale + normal_offset2), 0.5).rgb;

	ALBEDO = color_out;
	ALPHA = alpha_blend;
	ROUGHNESS = roughness;
	NORMAL_MAP = normal_blend;
}