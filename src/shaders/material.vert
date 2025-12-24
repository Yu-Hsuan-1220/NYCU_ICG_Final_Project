#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Walking animation uniforms
uniform float walkTime;
uniform int isWalking;       // 1 = walking, 0 = not walking
uniform float legThreshold;  // Y position ABOVE which vertices are considered "legs"

void main()
{
    vec3 pos = aPos;
    
    // Animate legs if walking - legs are at HIGHER Y values in this model
    if (isWalking == 1 && pos.y > legThreshold && pos.x < 5.0 && pos.x > -1.0)
    {
        // How much this vertex is part of the leg (0 at threshold, 1 at top)
        float legDepth = (pos.y - legThreshold) / 10.0;  // Adjust divisor based on model scale
        legDepth = clamp(legDepth, 0.0, 1.0);
        
        // Left leg (x < 0) and right leg (x > 0) move in opposite phases
        float phase = (pos.x < 0.0) ? 0.0 : 3.14159;  // 180 degree phase difference
        
        // Walking swing: forward and backward motion
        float swingSpeed = 8.0;  // How fast the legs swing
        float swingAmount = 10.0;  // How far the legs swing (in units)
        
        // Calculate swing based on time and phase
        float swing = sin(walkTime * swingSpeed + phase) * swingAmount * legDepth;
        
        // Move leg forward/backward (Z axis in local space)
        pos.z += swing;
        
        // Slight up/down motion as leg swings
        float lift = abs(sin(walkTime * swingSpeed + phase)) * 2.0 * legDepth;
        pos.y -= lift;  // Legs lift up (decrease Y since Y-up means higher)
    }
    if (isWalking == 1 && pos.y > legThreshold && pos.x < 10.0 && pos.x > 5.0)
    {
        // How much this vertex is part of the leg (0 at threshold, 1 at top)
        float legDepth = (pos.y - legThreshold) / 10.0;  // Adjust divisor based on model scale
        legDepth = clamp(legDepth, 0.0, 1.0);
        
        // Left leg (x < 0) and right leg (x > 0) move in opposite phases
        float phase = (pos.x < 0.0) ? 0.0 : 3.14159;  // 180 degree phase difference
        
        // Walking swing: forward and backward motion
        float swingSpeed = 8.0;  // How fast the legs swing
        float swingAmount = 10.0;  // How far the legs swing (in units)
        
        // Calculate swing based on time and phase
        float swing = sin(walkTime * swingSpeed + phase+1.57) * swingAmount * legDepth;
        
        // Move leg forward/backward (Z axis in local space)
        pos.z += swing;
        
        // Slight up/down motion as leg swings
        float lift = abs(sin(walkTime * swingSpeed + phase)) * 2.0 * legDepth;
        pos.y -= lift;  // Legs lift up (decrease Y since Y-up means higher)
    }
    
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    VertexColor = aColor;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
