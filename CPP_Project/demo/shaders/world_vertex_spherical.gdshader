/*

testing world curvature simulation

*/
shader_type spatial;

uniform float radius = 128.0;

uniform float normal_step = 1.0;


float get_height(vec3 position){
    float distSq = position.x * position.x + position.z * position.z;
    float yOffset = sqrt(max(0.0, radius * radius - distSq)); // max is to ensure you don't get negative values



    return yOffset;
}



vec3 get_normal(vec3 position) {

    // Get the heights at the neighboring points along the x and z axes
    float heightX = get_height(position + vec3(normal_step, 0.0, 0.0));  // Neighbor along the x-axis
    float heightZ = get_height(position + vec3(0.0, 0.0, normal_step));  // Neighbor along the z-axis

    // Calculate the gradient (the slope in the x and z directions)
    float dHeightX = heightX - get_height(position);  // Change in height along x
    float dHeightZ = heightZ - get_height(position);  // Change in height along z

    // The normal is the gradient in the x and z directions
    vec3 normal = normalize(vec3(-dHeightX, 1.0, -dHeightZ));

    return normal;
}


varying vec3 world_normal; // Declare a varying variable to pass to the fragment shader


uniform vec3 color: source_color = vec3(0,0,1);

void vertex() {

    // Get the world position by multiplying the vertex with the MODEL_MATRIX
    vec4 WORLD_VERTEX = MODEL_MATRIX * vec4(VERTEX, 1.0);


    //float distSq = WORLD_VERTEX.x * WORLD_VERTEX.x + WORLD_VERTEX.z * WORLD_VERTEX.z;
    //float yOffset = sqrt(max(0.0, radius * radius - distSq)); // max is to ensure you don't get negative values


    float height = get_height(WORLD_VERTEX.xyz);



    VERTEX.y = height - radius;

        if (height <= 0.0){
        VERTEX.y -= radius;
    }


    world_normal = get_normal(WORLD_VERTEX.xyz);

    NORMAL = world_normal;

    //VERTEX.y += yOffset - radius;
    //VERTEX.y = yOffset - radius;

    //VERTEX.y += get_height(WORLD_VERTEX.xyz);


    //float x = length(VERTEX);

    //x = tan(radians(x));

    //VERTEX.y += x;
    //VERTEX.y -= 1.0;


	// Called for every vertex the material is visible on.
}

void fragment() {

    ALBEDO = color;

    //NORMAL = world_normal;


	// Called for every pixel the material is visible on.
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
