#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "header/cube.h"
#include "header/Object.h"
#include "header/shader.h"
#include "header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
void updateCamera();
void applyOrbitDelta(float yawDelta, float pitchDelta, float radiusDelta);
unsigned int loadCubemap(std::vector<std::string> &mFileName);

struct material_t{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float gloss;
};

struct light_t{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct camera_t{
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    glm::vec3 target;

    float yaw;
    float pitch;
    float radius;
    float minRadius;
    float maxRadius;
    float orbitRotateSpeed;
    float orbitZoomSpeed;
    float minOrbitPitch;
    float maxOrbitPitch;
    bool enableAutoOrbit;
    float autoOrbitSpeed;
};

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// cube map 
unsigned int cubemapTexture;
unsigned int cubemapVAO, cubemapVBO;

GLFWwindow *g_window = nullptr;


// shader programs 
int shaderProgramIndex = 0;
std::vector<shader_program_t*> shaderPrograms;

glm::vec3 rocket_pos = glm::vec3(0.0f, 0.0f, 0.0f);

light_t light;
material_t material;
camera_t camera;




// people
Object* staticModel = nullptr;

// cubepeople
Object* cubeModel = nullptr;
bool isCube = false;
shader_program_t* cubemapShader;




unsigned int lightmapTexture;

Object *usagiModel = nullptr;
glm::mat4 usagiModelMatrix(1.0f);
shader_program_t *usagiShader = nullptr;

// usagi idle bounce animation
float usagiBounceTime = 0.0f;
const float usagiBounceSpeed = 2.5f;   // Speed of bounce cycle
const float usagiBounceHeight = 0.8f;  // Height of bounce
const float usagiSquashAmount = 0.15f; // Amount of squash/stretch
const glm::vec3 usagiBasePos = glm::vec3(-217.0f, 30.2f, -1.3f);

// chiikawa model
Object *chiikawaModel = nullptr;
glm::mat4 chiikawaModelMatrix(1.0f);
shader_program_t *chiikawaShader = nullptr;
bool chiikawaBlack = false;  // Turns black after rocket explosion

// chiikawa explosion hit effect
bool chiikawaHit = false;           // Whether chiikawa was hit by explosion
float chiikawaHitYOffset = 0.0f;    // Y offset from explosion (flies up 10 units)
float chiikawaSpinAngle = 0.0f;     // Current spin angle
const float chiikawaSpinSpeed = 1500.0f;  // High speed spinning (degrees per second)
const float chiikawaFlyUpHeight = 30.0f;  // Fly up 30 units

// chiikawa idle bounce animation
float chiikawaBounceTime = 0.0f;
const float chiikawaBounceSpeed = 3.0f;                            // Slightly faster bounce
const float chiikawaBounceHeight = 0.6f;                           // Height of bounce
const float chiikawaSquashAmount = 0.12f;                          // Amount of squash/stretch
const glm::vec3 chiikawaBasePos = glm::vec3(240.0f, 35.0f, -1.3f); // Position near usagi


// mega_knight model
Object *megaKnightModel = nullptr;
glm::mat4 megaKnightModelMatrix(1.0f);

// mega_knight jump animation
bool megaKnightJumping = false;
float megaKnightY = 300.0f;              // Current Y position (start in sky)
const float megaKnightStartY = 300.0f;   // Start high in the sky
const float megaKnightGroundY = 22.0f;    // Ground level
float megaKnightVelocityY = 0.0f;        // Current vertical velocity
const float megaKnightGravity = -500.0f; // Gravity acceleration

// mega_knight walking animation
bool megaKnightWalking = false;
float megaKnightX = 150.0f;              // Current X position
float megaKnightZ = -80.0f;              // Current Z position
const float megaKnightStartX = 150.0f;   // Initial X position
const float megaKnightStartZ = -80.0f;   // Initial Z position
const float megaKnightWalkSpeed = 20.0f; // Walk speed
float megaKnightWalkTime = 0.0f;         // Time for bobbing animation

// blue_princess model
Object *leftBluePrincessModel = nullptr;
glm::mat4 leftBluePrincessModelMatrix(1.0f);
Object *rightBluePrincessModel = nullptr;
glm::mat4 rightBluePrincessModelMatrix(1.0f);

// red_princess model
Object *leftRedPrincessModel = nullptr;
glm::mat4 leftRedPrincessModelMatrix(1.0f);
Object *rightRedPrincessModel = nullptr;
glm::mat4 rightRedPrincessModelMatrix(1.0f);

// mimimi animation
std::vector<unsigned int> mimimiFrames;
shader_program_t *spriteShader = nullptr;
unsigned int spriteVAO = 0, spriteVBO = 0;
bool mimimiPlaying = false;
int mimimiCurrentFrame = 0;
float mimimiFrameTime = 0.0f;
const float MIMIMI_FRAME_DURATION = 0.03f; // 30ms per frame
const int MIMIMI_FRAME_COUNT = 71;
int mimimiImageWidth = 0, mimimiImageHeight = 0; // Store image dimensions

// laugh animation
std::vector<unsigned int> laughFrames;
bool laughPlaying = false;
int laughCurrentFrame = 0;
float laughFrameTime = 0.0f;
const float LAUGH_FRAME_DURATION = 0.05f; // 50ms per frame
const int LAUGH_FRAME_COUNT = 40;
int laughImageWidth = 0, laughImageHeight = 0;

// ground
Object* groundPart1 = nullptr;
Object* groundPart2 = nullptr;
Object* groundPart3 = nullptr;
Object* rocketModel = nullptr;
shader_program_t* groundShader = nullptr;
shader_program_t* rocketShader = nullptr;

// Particle system
struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float age;
    float maxAge;
};

const int MAX_PARTICLES = 500;
Particle particles[MAX_PARTICLES];
int particleCount = 0;
unsigned int particleVAO, particleVBO;
shader_program_t* particleShader = nullptr;

// Rocket movement parameters
float rocketx = 0.0f;
float rockety = 0.0f;
float rotatez = 0.0f;

// Rocket flight parameters
bool rocketVisible = false;
bool rocketFlying = false;
glm::vec3 rocketStartPos = glm::vec3(-217.0f, 30.2f, -1.3f);
glm::vec3 rocketEndPos = glm::vec3(230.0f, 32.5f, -4.8f);
glm::vec3 rocketVelocity = glm::vec3(0.0f);
float rocketFlightTime = 0.0f;
float rocketTotalFlightTime = 3.0f;  // 總飛行時間（秒）
float rocketGravity = -50.0f;  // 重力加速度（調整拋物線弧度）

const int MAX_EXPLOSION_PARTICLES = 800;
Particle explosionParticles[MAX_EXPLOSION_PARTICLES];
bool explosionActive = false;
float explosionTime = 0.0f;
glm::vec3 explosionPos = glm::vec3(0.0f);
unsigned int explosionVAO, explosionVBO;
shader_program_t* explosionShader = nullptr;

// Ground burn effect
bool groundBurnEnabled = false;
glm::vec3 burnCenter = glm::vec3(230.0f, 32.5f, -4.8f);
float burnRadius = 70.0f;

// Crown model (appears after rocket lands)
Object* crownModel = nullptr;
glm::mat4 crownModelMatrix(1.0f);
bool crownVisible = false;
glm::vec3 crownPos = glm::vec3(180.0f, 30.0f, -8.0f);
float crownRotationAngle = 0.0f;




glm::mat4 modelMatrix(1.0f);

float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void ground_setup() {

    string obj1 = "../../src/asset/ground/ground_part1.obj";
    string tex1 = "../../src/asset/ground/texture/bake_mapa1_Difuse.png";
    groundPart1 = new Object(obj1);
    groundPart1->loadTexture(tex1);

    string obj2 = "../../src/asset/ground/ground_part2.obj";
    string tex2 = "../../src/asset/ground/texture/BAKE_2.png";
    groundPart2 = new Object(obj2);
    groundPart2->loadTexture(tex2);

    string obj3 = "../../src/asset/ground/ground_part3.obj";
    string tex3 = "../../src/asset/ground/texture/Bake_3.png";
    groundPart3 = new Object(obj3);
    groundPart3->loadTexture(tex3);

    string vpath = "../../src/shaders/ground.vert";
    string fpath = "../../src/shaders/ground.frag";
    groundShader = new shader_program_t();
    groundShader->create();
    groundShader->add_shader(vpath, GL_VERTEX_SHADER);
    groundShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    groundShader->link_shader();
}

void chiikawa_setup()
{
    // Path to chiikawa.obj (in the project root directory)
    std::string chiikawa_obj_path = "../../chiikawa.obj";

    // Load with materials (true = load MTL file)
    chiikawaModel = new Object(chiikawa_obj_path, true);

    // Load the texture for chiikawa
    chiikawaModel->loadTexture("../../chiikawa_face.png");

    // Set up the model matrix (position, scale, rotation)
    chiikawaModelMatrix = glm::mat4(1.0f);
    chiikawaModelMatrix = glm::translate(chiikawaModelMatrix, chiikawaBasePos);
    chiikawaModelMatrix = glm::scale(chiikawaModelMatrix, glm::vec3(5.0f)); // Slightly smaller than usagi
    chiikawaModelMatrix = glm::rotate(chiikawaModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Set up chiikawa shader (texture + lighting)
    std::string vpath = "../../src/shaders/chiikawa.vert";
    std::string fpath = "../../src/shaders/chiikawa.frag";
    chiikawaShader = new shader_program_t();
    chiikawaShader->create();
    chiikawaShader->add_shader(vpath, GL_VERTEX_SHADER);
    chiikawaShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    chiikawaShader->link_shader();
}

void usagi_setup()
{
    // Path to usagi.obj (in the project root directory)
    std::string usagi_obj_path = "../../usagi.obj";

    // Load with materials (true = load MTL file)
    usagiModel = new Object(usagi_obj_path, true);

    // Set up the model matrix (position, scale, rotation)
    usagiModelMatrix = glm::mat4(1.0f);
    usagiModelMatrix = glm::translate(usagiModelMatrix, glm::vec3(-217.0f, 30.2f, -1.3f));
    usagiModelMatrix = glm::scale(usagiModelMatrix, glm::vec3(20.0f)); // Adjust scale as needed
    usagiModelMatrix = glm::rotate(usagiModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Set up the material shader for usagi (no texture, uses material colors)
    std::string vpath = "../../src/shaders/material.vert";
    std::string fpath = "../../src/shaders/material.frag";
    usagiShader = new shader_program_t();
    usagiShader->create();
    usagiShader->add_shader(vpath, GL_VERTEX_SHADER);
    usagiShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    usagiShader->link_shader();
}

void megaknight_setup()
{
    // Path to mega_knight.obj (in the project root directory)
    std::string megaknight_obj_path = "../../mega_knight.obj";

    // Load with materials (true = load MTL file)
    megaKnightModel = new Object(megaknight_obj_path, true);

    // Set up the model matrix (position in sky, scale, rotation)
    megaKnightModelMatrix = glm::mat4(1.0f);
    megaKnightModelMatrix = glm::translate(megaKnightModelMatrix, glm::vec3(-120.0f, megaKnightY, 80.0f)); // Start in sky
    megaKnightModelMatrix = glm::scale(megaKnightModelMatrix, glm::vec3(2.0f));                            // Adjust scale
    megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void blue_princess_setup()
{
    // Path to blue_princess.obj (in the project root directory)
    std::string blue_princess_obj_path = "../../blue_princess.obj";

    // Load left blue princess
    leftBluePrincessModel = new Object(blue_princess_obj_path, true);
    leftBluePrincessModelMatrix = glm::mat4(1.0f);
    leftBluePrincessModelMatrix = glm::translate(leftBluePrincessModelMatrix, glm::vec3(-140.0f, 5.0f, 80.0f)); // Left position
    leftBluePrincessModelMatrix = glm::scale(leftBluePrincessModelMatrix, glm::vec3(0.3f));
    leftBluePrincessModelMatrix = glm::rotate(leftBluePrincessModelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));

    // Load right blue princess
    rightBluePrincessModel = new Object(blue_princess_obj_path, true);
    rightBluePrincessModelMatrix = glm::mat4(1.0f);
    rightBluePrincessModelMatrix = glm::translate(rightBluePrincessModelMatrix, glm::vec3(-140.0f, 5.0f, -80.0f)); // Right position
    rightBluePrincessModelMatrix = glm::scale(rightBluePrincessModelMatrix, glm::vec3(0.3f));
    rightBluePrincessModelMatrix = glm::rotate(rightBluePrincessModelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
}

void red_princess_setup()
{
    // Path to red_princess.obj (in the project root directory)
    std::string red_princess_obj_path = "../../red_princess.obj";

    // Load left red princess
    leftRedPrincessModel = new Object(red_princess_obj_path, true);
    leftRedPrincessModelMatrix = glm::mat4(1.0f);
    leftRedPrincessModelMatrix = glm::translate(leftRedPrincessModelMatrix, glm::vec3(165.0f, 5.0f, -80.0f)); // Left position
    leftRedPrincessModelMatrix = glm::scale(leftRedPrincessModelMatrix, glm::vec3(0.3f));
    leftRedPrincessModelMatrix = glm::rotate(leftRedPrincessModelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));

    // Load right red princess
    rightRedPrincessModel = new Object(red_princess_obj_path, true);
    rightRedPrincessModelMatrix = glm::mat4(1.0f);
    rightRedPrincessModelMatrix = glm::translate(rightRedPrincessModelMatrix, glm::vec3(165.0f, 5.0f, 80.0f)); // Right position
    rightRedPrincessModelMatrix = glm::scale(rightRedPrincessModelMatrix, glm::vec3(0.3f));
    rightRedPrincessModelMatrix = glm::rotate(rightRedPrincessModelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
}

void mimimi_setup()
{
    // Load all mimimi animation frames
    std::string mimimiDir = "../../mimimi/";
    for (int i = 0; i < MIMIMI_FRAME_COUNT; i++)
    {
        char filename[256];
        snprintf(filename, sizeof(filename), "%sframe_%03d.png", mimimiDir.c_str(), i);

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        // Force load as RGBA (4 channels) to ensure color is preserved
        unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 4);
        if (data)
        {
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            // Don't generate mipmaps - use linear filtering only

            // Use CLAMP_TO_BORDER with transparent color to avoid edge repetition
            float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            mimimiFrames.push_back(textureID);
            // Store dimensions from first frame
            if (mimimiImageWidth == 0)
            {
                mimimiImageWidth = width;
                mimimiImageHeight = height;
            }
            std::cout << "Loaded: " << filename << " (" << width << "x" << height << ")" << std::endl;
        }
        else
        {
            std::cerr << "Failed to load mimimi frame: " << filename << std::endl;
        }
        stbi_image_free(data);
    }

    // Create sprite shader
    std::string vpath = "../../src/shaders/sprite.vert";
    std::string fpath = "../../src/shaders/sprite.frag";
    spriteShader = new shader_program_t();
    spriteShader->create();
    spriteShader->add_shader(vpath, GL_VERTEX_SHADER);
    spriteShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    spriteShader->link_shader();

    // Create sprite quad VAO for 3D world-space rendering
    float quadVertices[] = {
        // positions (X, Y, Z)         // texcoords
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom-right
        0.5f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right

        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
        0.5f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        -0.5f, 1.0f, 0.0f, 0.0f, 1.0f  // top-left
    };

    glGenVertexArrays(1, &spriteVAO);
    glGenBuffers(1, &spriteVBO);
    glBindVertexArray(spriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // 3D position (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Texture coords (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    std::cout << "Mimimi animation loaded: " << mimimiFrames.size() << " frames" << std::endl;
}

void laugh_setup()
{
    // Load all laugh animation frames
    std::string laughDir = "../../laugh/";
    for (int i = 0; i < LAUGH_FRAME_COUNT; i++)
    {
        char filename[256];
        snprintf(filename, sizeof(filename), "%sframe_%03d.png", laughDir.c_str(), i);

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 4);
        if (data)
        {
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            laughFrames.push_back(textureID);
            if (laughImageWidth == 0)
            {
                laughImageWidth = width;
                laughImageHeight = height;
            }
            std::cout << "Loaded laugh: " << filename << " (" << width << "x" << height << ")" << std::endl;
        }
        else
        {
            std::cerr << "Failed to load laugh frame: " << filename << std::endl;
        }
        stbi_image_free(data);
    }
    std::cout << "Laugh animation loaded: " << laughFrames.size() << " frames" << std::endl;
}

void rocket_setup(){

    string obj = "../../src/asset/rocket/rocket.obj";
    string tex = "../../src/asset/rocket/rocket.png"; 
    rocketModel = new Object(obj);
    rocketModel->loadTexture(tex);

    string vpath = "../../src/shaders/rocket.vert";
    string fpath = "../../src/shaders/rocket.frag";
    rocketShader = new shader_program_t();
    rocketShader->create();
    rocketShader->add_shader(vpath, GL_VERTEX_SHADER);
    rocketShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    rocketShader->link_shader();
}

void crown_setup(){
    // Load crown model with MTL materials
    std::string crown_obj_path = "../../src/asset/crown/crown.obj";
    crownModel = new Object(crown_obj_path, true);  // true = load MTL file
    
    // Set up initial model matrix
    crownModelMatrix = glm::mat4(1.0f);
    crownModelMatrix = glm::translate(crownModelMatrix, crownPos);
    crownModelMatrix = glm::scale(crownModelMatrix, glm::vec3(10.0f));  // Adjust scale as needed
}

void particle_setup(){
    // Initialize all particles as dead
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].age = 999.0f;
        particles[i].maxAge = 2.0f;
    }

    // Create particle shader with geometry shader
    string vpath = "../../src/shaders/partical.vert";
    string gpath = "../../src/shaders/partical.geom";
    string fpath = "../../src/shaders/partical.frag";
    
    particleShader = new shader_program_t();
    particleShader->create();
    particleShader->add_shader(vpath, GL_VERTEX_SHADER);
    particleShader->add_shader(gpath, GL_GEOMETRY_SHADER);
    particleShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    particleShader->link_shader();

    // Create VAO and VBO for particles
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(0);
    
    // Velocity attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
    glEnableVertexAttribArray(1);
    
    // Color attribute (location = 2)
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glEnableVertexAttribArray(2);
    
    // Age attribute (location = 3)
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, age));
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0);
}

void explosion_setup(){
    // Initialize all explosion particles as dead
    for (int i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
        explosionParticles[i].age = 999.0f;
        explosionParticles[i].maxAge = 3.0f;
    }

    // Create explosion shader with geometry shader
    string vpath = "../../src/shaders/explosion.vert";
    string gpath = "../../src/shaders/explosion.geom";
    string fpath = "../../src/shaders/explosion.frag";
    
    explosionShader = new shader_program_t();
    explosionShader->create();
    explosionShader->add_shader(vpath, GL_VERTEX_SHADER);
    explosionShader->add_shader(gpath, GL_GEOMETRY_SHADER);
    explosionShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    explosionShader->link_shader();

    // Create VAO and VBO for explosion particles
    glGenVertexArrays(1, &explosionVAO);
    glGenBuffers(1, &explosionVBO);
    
    glBindVertexArray(explosionVAO);
    glBindBuffer(GL_ARRAY_BUFFER, explosionVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_EXPLOSION_PARTICLES * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
    
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
    glEnableVertexAttribArray(0);
    
    // Velocity attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
    glEnableVertexAttribArray(1);
    
    // Color attribute (location = 2)
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glEnableVertexAttribArray(2);
    
    // Age attribute (location = 3)
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, age));
    glEnableVertexAttribArray(3);
    
    glBindVertexArray(0);
}

void model_setup(){
#if defined(__linux__) || defined(__APPLE__)
    std::string obj_path = "..\\..\\src\\asset\\obj\\Mei_Run.obj";
    std::string cube_obj_path = "..\\..\\src\\asset\\obj\\cube.obj";
    std::string texture_path = "..\\..\\src\\asset\\texture\\Mei_TEX.png";
#else
    std::string obj_path = "..\\..\\src\\asset\\obj\\Mei_Run.obj";
    std::string texture_path = "..\\..\\src\\asset\\texture\\Mei_TEX.png";
    std::string cube_obj_path = "..\\..\\src\\asset\\obj\\cube.obj";
#endif

    staticModel = new Object(obj_path);
    staticModel->loadTexture(texture_path);
    cubeModel = new Object(cube_obj_path);

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100.0f));
}

void camera_setup(){
    camera.worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.yaw = 90.0f;
    camera.pitch = 10.0f;
    camera.radius = 400.0f;
    camera.minRadius = 150.0f;
    camera.maxRadius = 800.0f;
    camera.orbitRotateSpeed = 60.0f;
    camera.orbitZoomSpeed = 400.0f;
    camera.minOrbitPitch = -80.0f;
    camera.maxOrbitPitch = 80.0f;
    camera.target = glm::vec3(0.0f);

    camera.enableAutoOrbit = false;
    camera.autoOrbitSpeed = 20.0f;

    updateCamera();
}

void updateCamera(){
    float yawRad = glm::radians(camera.yaw);
    float pitchRad = glm::radians(camera.pitch);
    float cosPitch = cos(pitchRad);

    camera.position.x = camera.target.x + camera.radius * cosPitch * cos(yawRad);
    camera.position.y = camera.target.y + camera.radius * sin(pitchRad);
    camera.position.z = camera.target.z + camera.radius * cosPitch * sin(yawRad);

    camera.front = glm::normalize(camera.target - camera.position);
    camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
}

void applyOrbitDelta(float yawDelta, float pitchDelta, float radiusDelta) {
    camera.yaw += yawDelta;
    camera.pitch = glm::clamp(camera.pitch + pitchDelta, camera.minOrbitPitch, camera.maxOrbitPitch);
    camera.radius = glm::clamp(camera.radius + radiusDelta, camera.minRadius, camera.maxRadius);
    updateCamera();
}

void light_setup(){
    light.position = glm::vec3(1000.0, 1000.0, 0.0);
    light.ambient = glm::vec3(1.0);
    light.diffuse = glm::vec3(1.0);
    light.specular = glm::vec3(1.0);
}

void material_setup(){
    material.ambient = glm::vec3(0.5);
    material.diffuse = glm::vec3(1.0);
    material.specular = glm::vec3(0.7);
    material.gloss = 50.0;
}

void shader_setup(){
#if defined(__linux__) || defined(__APPLE__)
    std::string shaderDir = "..\\..\\src\\shaders\\";
#else
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> shadingMethod = {
        "default"
    };

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    }
}

void cubemap_setup(){
#if defined(__linux__) || defined(__APPLE__)
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#else
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> faces
    {
        cubemapDir + "right.jpg",
        cubemapDir + "left.jpg",
        cubemapDir + "top.jpg",
        cubemapDir + "bottom.jpg",
        cubemapDir + "front.jpg",
        cubemapDir + "back.jpg"
    };
    cubemapTexture = loadCubemap(faces);   

    std::string vpath = shaderDir + "cubemap.vert";
    std::string fpath = shaderDir + "cubemap.frag";
    
    cubemapShader = new shader_program_t();
    cubemapShader->create();
    cubemapShader->add_shader(vpath, GL_VERTEX_SHADER);
    cubemapShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    cubemapShader->link_shader();

    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void setup(){
    light_setup();
    usagi_setup();
    megaknight_setup();
    chiikawa_setup();
    blue_princess_setup();
    red_princess_setup();
    laugh_setup();
    mimimi_setup();
    ground_setup();
    rocket_setup();
    crown_setup();
    particle_setup();
    explosion_setup();
    model_setup();
    shader_setup();
    camera_setup();
    cubemap_setup();
    material_setup();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);
}

void update(){
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastFrame;
    lastFrame = currentTime;

    // Update window title with camera position every 0.1 seconds
    static float printTimer = 0.0f;
    printTimer += deltaTime;
    if (printTimer >= 0.1f)
    {
        char title[256];
        snprintf(title, sizeof(title), "Camera: (%.2f, %.2f, %.2f)",
                 camera.position.x, camera.position.y, camera.position.z);
        glfwSetWindowTitle(g_window, title);
        printTimer = 0.0f;
    }

    // Usagi idle bounce animation (slime-like)
    usagiBounceTime += deltaTime * usagiBounceSpeed;
    float bounceOffset = sin(usagiBounceTime) * usagiBounceHeight;
    // Squash when down, stretch when up
    float squashStretch = 1.0f + sin(usagiBounceTime) * usagiSquashAmount;
    float inverseSquash = 1.0f - sin(usagiBounceTime) * usagiSquashAmount * 0.5f;

    usagiModelMatrix = glm::mat4(1.0f);
    usagiModelMatrix = glm::translate(usagiModelMatrix, usagiBasePos + glm::vec3(0.0f, bounceOffset, 0.0f));
    usagiModelMatrix = glm::scale(usagiModelMatrix, glm::vec3(20.0f * inverseSquash, 20.0f * squashStretch, 20.0f * inverseSquash));
    usagiModelMatrix = glm::rotate(usagiModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Chiikawa idle bounce animation (slime-like, slightly offset phase)
    chiikawaBounceTime += deltaTime * chiikawaBounceSpeed;
    float chiikawaBounceOffset = sin(chiikawaBounceTime) * chiikawaBounceHeight;
    float chiikawaSquashStretch = 1.0f + sin(chiikawaBounceTime) * chiikawaSquashAmount;
    float chiikawaInverseSquash = 1.0f - sin(chiikawaBounceTime) * chiikawaSquashAmount * 0.5f;

    // Update chiikawa hit effect (spinning after explosion)
    if (chiikawaHit) {
        chiikawaSpinAngle += chiikawaSpinSpeed * deltaTime;
        if (chiikawaSpinAngle >= 360.0f) {
            chiikawaSpinAngle -= 360.0f;  // Keep spinning continuously
        }
    }

    chiikawaModelMatrix = glm::mat4(1.0f);
    chiikawaModelMatrix = glm::translate(chiikawaModelMatrix, chiikawaBasePos + glm::vec3(0.0f, chiikawaBounceOffset + chiikawaHitYOffset, 0.0f));
    chiikawaModelMatrix = glm::scale(chiikawaModelMatrix, glm::vec3(10.0f * chiikawaInverseSquash, 10.0f * chiikawaSquashStretch, 10.0f * chiikawaInverseSquash));
    if (chiikawaHit) {
        // Rapid spinning on multiple axes for chaotic effect
        chiikawaModelMatrix = glm::rotate(chiikawaModelMatrix, glm::radians(chiikawaSpinAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        chiikawaModelMatrix = glm::rotate(chiikawaModelMatrix, glm::radians(chiikawaSpinAngle * 0.7f), glm::vec3(1.0f, 0.0f, 0.0f));
        chiikawaModelMatrix = glm::rotate(chiikawaModelMatrix, glm::radians(chiikawaSpinAngle * 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
    } else {
        chiikawaModelMatrix = glm::rotate(chiikawaModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }


    if (camera.enableAutoOrbit) {
        float yawDelta = camera.autoOrbitSpeed * deltaTime;
        applyOrbitDelta(yawDelta, 0.0f, 0.0f);
    }
    if (mimimiPlaying)
    {
        mimimiFrameTime += deltaTime;
        if (mimimiFrameTime >= MIMIMI_FRAME_DURATION)
        {
            mimimiFrameTime = 0.0f;
            mimimiCurrentFrame++;
            if (mimimiCurrentFrame >= MIMIMI_FRAME_COUNT)
            {
                mimimiCurrentFrame = 0;
                mimimiPlaying = false; // Stop after one loop, or remove this line to loop forever
            }
        }
    }

    if (laughPlaying)
    {
        laughFrameTime += deltaTime;
        if (laughFrameTime >= LAUGH_FRAME_DURATION)
        {
            laughFrameTime = 0.0f;
            laughCurrentFrame++;
            if (laughCurrentFrame >= LAUGH_FRAME_COUNT)
            {
                laughCurrentFrame = 0;
                laughPlaying = false; // Stop after one loop
            }
        }
    }
    
    // Update mega_knight jump animation
    if (megaKnightJumping)
    {
        megaKnightVelocityY += megaKnightGravity * deltaTime;
        megaKnightY += megaKnightVelocityY * deltaTime;

        // Check if landed
        if (megaKnightY <= megaKnightGroundY)
        {
            megaKnightY = megaKnightGroundY;
            megaKnightJumping = false;
            megaKnightVelocityY = 0.0f;
        }

        // Update model matrix with new Y position
        megaKnightModelMatrix = glm::mat4(1.0f);
        megaKnightModelMatrix = glm::translate(megaKnightModelMatrix, glm::vec3(megaKnightX, megaKnightY, megaKnightZ));
        megaKnightModelMatrix = glm::scale(megaKnightModelMatrix, glm::vec3(2.0f));
        megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Update mega_knight walking animation
    if (megaKnightWalking && !megaKnightJumping)
    {
        megaKnightWalkTime += deltaTime;

        // Move forward (in -X direction)
        megaKnightX -= megaKnightWalkSpeed * deltaTime;

        // Add a bobbing motion for walking effect
        float bobAmount = sin(megaKnightWalkTime * 10.0f) * 1.0f;
        float currentY = megaKnightGroundY + bobAmount;

        // Update model matrix
        megaKnightModelMatrix = glm::mat4(1.0f);
        megaKnightModelMatrix = glm::translate(megaKnightModelMatrix, glm::vec3(megaKnightX, currentY, megaKnightZ));
        megaKnightModelMatrix = glm::scale(megaKnightModelMatrix, glm::vec3(2.0f));
        megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // crown rotation
    if (crownVisible){
        crownRotationAngle += 45.0f * deltaTime; // Rotate 45 degrees per second
        crownPos.y = 30.0f + sin(glm::radians(crownRotationAngle * 4)) * 4.0f; // Bobbing effect
        crownModelMatrix = glm::mat4(1.0f);
        crownModelMatrix = glm::translate(crownModelMatrix, crownPos);
        crownModelMatrix = glm::rotate(crownModelMatrix, glm::radians(crownRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        crownModelMatrix = glm::scale(crownModelMatrix, glm::vec3(0.1f));
    }
    
    // Update rocket flight
    if (rocketFlying) {
        rocketFlightTime += deltaTime;
        
        // Calculate progress (0 to 1)
        float t = rocketFlightTime / rocketTotalFlightTime;
        
        if (t >= 1.0f) {
            // Rocket reached destination - trigger explosion!
            rocketFlying = false;
            rocketVisible = false;
            t = 1.0f;
            
            // Chiikawa turns black from explosion
            chiikawaBlack = true;
            
            // Chiikawa gets hit - flies up and spins
            chiikawaHit = true;
            chiikawaHitYOffset = chiikawaFlyUpHeight;
            
            // Start explosion at rocket end position
            explosionActive = true;
            explosionTime = 0.0f;
            explosionPos = rocketEndPos;
            
            // Enable ground burn effect
            groundBurnEnabled = true;
            burnCenter = rocketEndPos;
            
            // Show crown at landing position
            crownVisible = true;
            crownModelMatrix = glm::mat4(1.0f);
            crownModelMatrix = glm::translate(crownModelMatrix, crownPos);
            crownModelMatrix = glm::rotate(crownModelMatrix, glm::radians(crownRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            crownModelMatrix = glm::scale(crownModelMatrix, glm::vec3(0.1f));
            
            // Initialize explosion particles
            for (int i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
                // Random direction (spherical distribution)
                float theta = (rand() % 360) * 3.14159f / 180.0f;
                float phi = (rand() % 180) * 3.14159f / 180.0f;
                float speed = 20.0f + (rand() % 100) / 2.0f;  // Random speed
                
                explosionParticles[i].position = explosionPos;
                explosionParticles[i].velocity = glm::vec3(
                    sin(phi) * cos(theta) * speed,
                    sin(phi) * sin(theta) * speed + 15.0f,  // Slight upward bias
                    cos(phi) * speed
                );
                
                // Random yellow-orange-red color
                float colorRand = (rand() % 100) / 100.0f;
                explosionParticles[i].color = glm::vec4(
                    1.0f,
                    0.3f + colorRand * 0.5f,
                    colorRand * 0.2f,
                    1.0f
                );
                explosionParticles[i].age = 0.0f;
                explosionParticles[i].maxAge = 2.0f + (rand() % 100) / 50.0f;  // Random lifetime
            }
        }
        
        if (rocketVisible) {
            // Linear interpolation for X and Z
            float x = rocketStartPos.x + (rocketEndPos.x - rocketStartPos.x) * t;
            float z = rocketStartPos.z + (rocketEndPos.z - rocketStartPos.z) * t;
            
            // Parabolic motion for Y: y = y0 + vy0*t + 0.5*g*t^2
            // Calculate initial velocity to hit target
            float totalT = rocketTotalFlightTime;
            float vy0 = (rocketEndPos.y - rocketStartPos.y - 0.5f * rocketGravity * totalT * totalT) / totalT;
            float actualTime = rocketFlightTime;
            float y = rocketStartPos.y + vy0 * actualTime + 0.5f * rocketGravity * actualTime * actualTime;
            
            // Update rocket position
            rocket_pos = glm::vec3(x, y, z);
            rocketx = x;
            rockety = y;
            
            // Calculate velocity for rotation (derivative of position)
            float vx = (rocketEndPos.x - rocketStartPos.x) / totalT;
            float vy = vy0 + rocketGravity * actualTime;
            float vz = (rocketEndPos.z - rocketStartPos.z) / totalT;
            rocketVelocity = glm::vec3(vx, vy, vz);
            
            // Calculate rotation angle (rocket faces movement direction)
            // rotatez is rotation around Z axis (for XY plane movement)
            rotatez = glm::degrees(atan2(vy, vx));
        }
    }
}

void render(){
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = glm::lookAt(camera.position - glm::vec3(0.0f, 0.2f, 0.1f), camera.position + camera.front, camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 3000.0f);


    glm::mat4 groundModelMatrix = glm::mat4(1.0f);
    groundModelMatrix = glm::translate(groundModelMatrix, glm::vec3(0.0f, -10.0f, 0.0f)); 
    groundModelMatrix = glm::scale(groundModelMatrix, glm::vec3(3.0f));

    groundShader->use();
    
    groundShader->set_uniform_value("model", groundModelMatrix);

    groundShader->set_uniform_value("view", view);
    groundShader->set_uniform_value("projection", projection);
    groundShader->set_uniform_value("viewPos", camera.position); // 如果 Shader 有算 Specular 會用到
    
    groundShader->set_uniform_value("burnEnabled", (int)groundBurnEnabled);
    groundShader->set_uniform_value("burnCenter", burnCenter);
    groundShader->set_uniform_value("burnRadius", burnRadius);

    groundPart1->draw(); 

    groundPart2->draw();

    groundPart3->draw();

    groundShader->release();

    if (rocketVisible) {
        glm::mat4 rocketModelMatrix = glm::mat4(1.0f);
        rocketModelMatrix = glm::translate(rocketModelMatrix, rocket_pos);
        
        // Rotate rocket to face movement direction in 3D
        // Calculate yaw (rotation around Y axis) and pitch (rotation around Z axis)
        glm::vec3 dir = glm::normalize(rocketVelocity);
        float yaw = atan2(dir.z, dir.x);  // Rotation in XZ plane
        float pitch = atan2(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));  // Elevation angle
        
        rocketModelMatrix = glm::rotate(rocketModelMatrix, -yaw, glm::vec3(0.0f, 1.0f, 0.0f));  // Yaw
        rocketModelMatrix = glm::rotate(rocketModelMatrix, pitch, glm::vec3(0.0f, 0.0f, 1.0f));  // Pitch
        rocketModelMatrix = glm::rotate(rocketModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));  // Initial orientation
        
        rocketModelMatrix = glm::scale(rocketModelMatrix, glm::vec3(1.0f));
        rocketShader->use();
        rocketShader->set_uniform_value("model", rocketModelMatrix);
        rocketShader->set_uniform_value("view", view);
        rocketShader->set_uniform_value("projection", projection);
        rocketShader->set_uniform_value("viewPos", camera.position);
        rocketShader->set_uniform_value("ourTexture", 0);
        rocketModel->draw();
        rocketShader->release();
    }
    
    // Render crown if visible
    if (crownVisible) {
        usagiShader->use();  // Reuse material shader
        usagiShader->set_uniform_value("model", crownModelMatrix);
        usagiShader->set_uniform_value("view", view);
        usagiShader->set_uniform_value("projection", projection);
        usagiShader->set_uniform_value("viewPos", camera.position);
        usagiShader->set_uniform_value("isWalking", 0);
        
        usagiShader->set_uniform_value("light_position", light.position);
        usagiShader->set_uniform_value("light_ambient", light.ambient);
        usagiShader->set_uniform_value("light_diffuse", light.diffuse);
        usagiShader->set_uniform_value("light_specular", light.specular);
        
        usagiShader->set_uniform_value("useVertexColor", 1);  // Use MTL colors
        usagiShader->set_uniform_value("material_ambient", glm::vec3(0.5f));
        usagiShader->set_uniform_value("material_diffuse", glm::vec3(1.0f, 0.84f, 0.0f));  // Gold color
        usagiShader->set_uniform_value("material_specular", glm::vec3(1.0f));
        usagiShader->set_uniform_value("material_gloss", 100.0f);
        
        crownModel->draw();
        usagiShader->release();
    }

    

    // ==========================================
    // Particle System
    // ==========================================
    
    // Emit particles from rocket bottom (only when rocket is visible)
    static int emissionIndex = 0;
    
    if (rocketVisible && rocketFlying) {
        // Calculate rotated emission point and direction
        glm::vec3 localBottomPos = glm::vec3(0.0f, -1.5f, 0.0f);
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotatez - 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 rotatedBottomPos = glm::vec3(rotationMatrix * glm::vec4(localBottomPos, 1.0f));
        glm::vec3 rocketBottomPos = rocket_pos + rotatedBottomPos;
        
        // Direction for flame (opposite to rocket velocity)
        glm::vec3 flameDir = -glm::normalize(rocketVelocity);
        
        int emitCount = 8;  // More particles per frame for denser flame
        for (int i = 0; i < emitCount; i++) {
            int idx = emissionIndex % MAX_PARTICLES;
            
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 30.0f + (rand() % 100) / 5.0f;  // Much faster particles
            
            particles[idx].position = rocketBottomPos;
            
            glm::vec3 baseVel = glm::vec3(
                cos(angle) * speed * 1.0f,
                0.0f,
                sin(angle) * speed * 1.0f
            );
            particles[idx].velocity = baseVel + flameDir * speed * 1.5f;
            particles[idx].color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);  // Orange
            particles[idx].age = 0.0f;
            particles[idx].maxAge = 1.0f;  // Shorter life for tighter trail
            
            emissionIndex++;
        }
        if (particleCount < MAX_PARTICLES) particleCount = MAX_PARTICLES;
    }
    
    // Update particles (CPU-side physics)
    int activeParticles = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].age < particles[i].maxAge) {
            particles[i].velocity.y -= 9.8f * deltaTime;
            particles[i].position += particles[i].velocity * deltaTime;
            particles[i].age += deltaTime;
            activeParticles++;
        } else {
            particles[i].age = 999.0f;
        }
    }
    
    // Render particles
    if (activeParticles > 0) {
        particleShader->use();
        particleShader->set_uniform_value("view", view);
        particleShader->set_uniform_value("projection", projection);
        
        // Update particle buffer
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_PARTICLES * sizeof(Particle), particles);
        
        // Render with blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        
        glBindVertexArray(particleVAO);
        glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);
        glBindVertexArray(0);
        
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        
        particleShader->release();
    }

    // ==========================================
    // Explosion Particle System
    // ==========================================
    if (explosionActive) {
        explosionTime += deltaTime;
        
        // Update explosion particles
        int activeExplosionParticles = 0;
        for (int i = 0; i < MAX_EXPLOSION_PARTICLES; i++) {
            if (explosionParticles[i].age < explosionParticles[i].maxAge) {
                // Apply gravity (slower for explosion effect)
                explosionParticles[i].velocity.y -= 15.0f * deltaTime;
                // Apply drag to slow down
                explosionParticles[i].velocity *= (1.0f - 0.5f * deltaTime);
                // Update position
                explosionParticles[i].position += explosionParticles[i].velocity * deltaTime;
                explosionParticles[i].age += deltaTime;
                activeExplosionParticles++;
            } else {
                explosionParticles[i].age = 999.0f;
            }
        }
        
        // Check if explosion is finished
        if (activeExplosionParticles == 0 || explosionTime > 4.0f) {
            explosionActive = false;
        }
        
        // Render explosion particles
        if (activeExplosionParticles > 0) {
            explosionShader->use();
            explosionShader->set_uniform_value("view", view);
            explosionShader->set_uniform_value("projection", projection);
            
            // Update explosion particle buffer
            glBindBuffer(GL_ARRAY_BUFFER, explosionVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_EXPLOSION_PARTICLES * sizeof(Particle), explosionParticles);
            
            // Render with additive blending for glow effect
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(GL_FALSE);
            
            glBindVertexArray(explosionVAO);
            glDrawArrays(GL_POINTS, 0, MAX_EXPLOSION_PARTICLES);
            glBindVertexArray(0);
            
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            
            explosionShader->release();
        }
    }
    // ==========================================
    // Rendering Usagi
    // ==========================================
    usagiShader->use();
    usagiShader->set_uniform_value("model", usagiModelMatrix);
    usagiShader->set_uniform_value("view", view);
    usagiShader->set_uniform_value("projection", projection);
    usagiShader->set_uniform_value("viewPos", camera.position);

    // Disable walking animation for usagi
    usagiShader->set_uniform_value("isWalking", 0);

    usagiShader->set_uniform_value("light_position", light.position);
    usagiShader->set_uniform_value("light_ambient", light.ambient);
    usagiShader->set_uniform_value("light_diffuse", light.diffuse);
    usagiShader->set_uniform_value("light_specular", light.specular);

    // Use vertex colors from MTL file (1 = true, 0 = false)
    usagiShader->set_uniform_value("useVertexColor", 1);

    // Fallback material colors (not used when useVertexColor is true)
    usagiShader->set_uniform_value("material_ambient", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_diffuse", glm::vec3(0.98f, 0.85f, 0.58f));
    usagiShader->set_uniform_value("material_specular", glm::vec3(0.3f));
    usagiShader->set_uniform_value("material_gloss", 10.0f);

    usagiModel->draw();
    usagiShader->release();

    // ==========================================
    // Rendering Chiikawa (texture + lighting)
    // ==========================================
    chiikawaShader->use();
    chiikawaShader->set_uniform_value("model", chiikawaModelMatrix);
    chiikawaShader->set_uniform_value("view", view);
    chiikawaShader->set_uniform_value("projection", projection);
    chiikawaShader->set_uniform_value("viewPos", camera.position);

    chiikawaShader->set_uniform_value("light_position", light.position);
    chiikawaShader->set_uniform_value("light_ambient", light.ambient);
    chiikawaShader->set_uniform_value("light_diffuse", light.diffuse);
    chiikawaShader->set_uniform_value("light_specular", light.specular);
    chiikawaShader->set_uniform_value("material_gloss", 32.0f);
    chiikawaShader->set_uniform_value("isBlack", chiikawaBlack);

    chiikawaModel->draw();
    chiikawaShader->release();

    // ==========================================
    // Rendering Mega Knight
    // ==========================================
    usagiShader->use(); // Reuse the same material shader
    usagiShader->set_uniform_value("model", megaKnightModelMatrix);
    usagiShader->set_uniform_value("view", view);
    usagiShader->set_uniform_value("projection", projection);
    usagiShader->set_uniform_value("viewPos", camera.position);

    // Walking animation uniforms
    usagiShader->set_uniform_value("walkTime", megaKnightWalkTime);
    usagiShader->set_uniform_value("isWalking", megaKnightWalking ? 1 : 0);
    usagiShader->set_uniform_value("legThreshold", 5.0f); // Vertices ABOVE Y=5 in local space are legs (model Y: -8.6 to 15.4)

    usagiShader->set_uniform_value("light_position", light.position);
    usagiShader->set_uniform_value("light_ambient", light.ambient);
    usagiShader->set_uniform_value("light_diffuse", light.diffuse);
    usagiShader->set_uniform_value("light_specular", light.specular);

    // Use vertex colors from MTL file
    usagiShader->set_uniform_value("useVertexColor", 1);

    usagiShader->set_uniform_value("material_ambient", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_diffuse", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_specular", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_gloss", 200.0f);

    megaKnightModel->draw();
    usagiShader->release();

    // ==========================================
    // Rendering Blue Princess (Left)
    // ==========================================
    usagiShader->use(); // Reuse the same material shader
    usagiShader->set_uniform_value("model", leftBluePrincessModelMatrix);
    usagiShader->set_uniform_value("view", view);
    usagiShader->set_uniform_value("projection", projection);
    usagiShader->set_uniform_value("viewPos", camera.position);

    usagiShader->set_uniform_value("isWalking", 0);

    usagiShader->set_uniform_value("light_position", light.position);
    usagiShader->set_uniform_value("light_ambient", light.ambient);
    usagiShader->set_uniform_value("light_diffuse", light.diffuse);
    usagiShader->set_uniform_value("light_specular", light.specular);

    usagiShader->set_uniform_value("useVertexColor", 1);

    usagiShader->set_uniform_value("material_ambient", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_diffuse", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_specular", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_gloss", 100.0f);

    leftBluePrincessModel->draw();
    usagiShader->release();

    // ==========================================
    // Rendering Blue Princess (Right)
    // ==========================================
    usagiShader->use();
    usagiShader->set_uniform_value("model", rightBluePrincessModelMatrix);
    usagiShader->set_uniform_value("view", view);
    usagiShader->set_uniform_value("projection", projection);
    usagiShader->set_uniform_value("viewPos", camera.position);

    usagiShader->set_uniform_value("isWalking", 0);

    usagiShader->set_uniform_value("light_position", light.position);
    usagiShader->set_uniform_value("light_ambient", light.ambient);
    usagiShader->set_uniform_value("light_diffuse", light.diffuse);
    usagiShader->set_uniform_value("light_specular", light.specular);

    usagiShader->set_uniform_value("useVertexColor", 1);

    usagiShader->set_uniform_value("material_ambient", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_diffuse", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_specular", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_gloss", 100.0f);

    rightBluePrincessModel->draw();
    usagiShader->release();

    // ==========================================
    // Rendering Red Princess (Left)
    // ==========================================
    usagiShader->use();
    usagiShader->set_uniform_value("model", leftRedPrincessModelMatrix);
    usagiShader->set_uniform_value("view", view);
    usagiShader->set_uniform_value("projection", projection);
    usagiShader->set_uniform_value("viewPos", camera.position);

    usagiShader->set_uniform_value("isWalking", 0);

    usagiShader->set_uniform_value("light_position", light.position);
    usagiShader->set_uniform_value("light_ambient", light.ambient);
    usagiShader->set_uniform_value("light_diffuse", light.diffuse);
    usagiShader->set_uniform_value("light_specular", light.specular);

    usagiShader->set_uniform_value("useVertexColor", 1);

    usagiShader->set_uniform_value("material_ambient", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_diffuse", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_specular", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_gloss", 100.0f);

    leftRedPrincessModel->draw();
    usagiShader->release();

    // ==========================================
    // Rendering Red Princess (Right)
    // ==========================================
    usagiShader->use();
    usagiShader->set_uniform_value("model", rightRedPrincessModelMatrix);
    usagiShader->set_uniform_value("view", view);
    usagiShader->set_uniform_value("projection", projection);
    usagiShader->set_uniform_value("viewPos", camera.position);

    usagiShader->set_uniform_value("isWalking", 0);

    usagiShader->set_uniform_value("light_position", light.position);
    usagiShader->set_uniform_value("light_ambient", light.ambient);
    usagiShader->set_uniform_value("light_diffuse", light.diffuse);
    usagiShader->set_uniform_value("light_specular", light.specular);

    usagiShader->set_uniform_value("useVertexColor", 1);

    usagiShader->set_uniform_value("material_ambient", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_diffuse", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_specular", glm::vec3(0.5f));
    usagiShader->set_uniform_value("material_gloss", 100.0f);

    rightRedPrincessModel->draw();
    usagiShader->release();
   
    cubemapShader->use();

    glm::mat4 view_skybox = glm::mat4(glm::mat3(view));

    cubemapShader->set_uniform_value("view_skybox", view_skybox);
    cubemapShader->set_uniform_value("projection", projection);
    cubemapShader->set_uniform_value("skybox", 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    
    glBindVertexArray(cubemapVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    cubemapShader->release();
    // ==========================================
    // Rendering Mimimi Animation (3D World-space beside Usagi)
    // ==========================================
    if (mimimiPlaying && !mimimiFrames.empty())
    {
        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        spriteShader->use();

        // Position beside usagi: usagi is at (-217, 30.2, -1.3)
        glm::mat4 spriteModel = glm::mat4(1.0f);
        spriteModel = glm::translate(spriteModel, glm::vec3(-200.0f, 50.0f, -30.3f)); // Beside usagi
        spriteModel = glm::scale(spriteModel, glm::vec3(65.0f));                      // Adjust size
        spriteModel = glm::rotate(spriteModel, glm::radians(90.0f), glm::vec3(0, 1, 0));

        spriteShader->set_uniform_value("model", spriteModel);
        spriteShader->set_uniform_value("view", view);
        spriteShader->set_uniform_value("projection", projection);
        spriteShader->set_uniform_value("spriteTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mimimiFrames[mimimiCurrentFrame]);

        glBindVertexArray(spriteVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        spriteShader->release();
        glDisable(GL_BLEND);
    }
    // ==========================================
    // Rendering Laugh Animation (3D World-space beside Chiikawa)
    // ==========================================
    if (laughPlaying && !laughFrames.empty())
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        spriteShader->use();

        // Position beside chiikawa: chiikawa is at (240, 35, -1.3)
        glm::mat4 spriteModel = glm::mat4(1.0f);
        spriteModel = glm::translate(spriteModel, glm::vec3(220.0f, 55.0f, -30.3f)); // Beside chiikawa
        spriteModel = glm::scale(spriteModel, glm::vec3(50.0f));                     // Adjust size
        spriteModel = glm::rotate(spriteModel, glm::radians(90.0f), glm::vec3(0, 1, 0));

        spriteShader->set_uniform_value("model", spriteModel);
        spriteShader->set_uniform_value("view", view);
        spriteShader->set_uniform_value("projection", projection);
        spriteShader->set_uniform_value("spriteTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, laughFrames[laughCurrentFrame]);

        glBindVertexArray(spriteVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        spriteShader->release();
        glDisable(GL_BLEND);
    }
    //glDepthFunc(GL_LESS);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW3-Static Model", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    g_window = window;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    setup();
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        update(); 
        render(); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    delete staticModel;
    delete cubeModel;

    delete groundPart1;
    delete groundPart2;
    delete groundPart3;
    delete rocketModel;
    delete crownModel;

    
    for (auto shader : shaderPrograms) {
        delete shader;
    }
    delete cubemapShader;
    delete groundShader;
    delete rocketShader;
    delete particleShader;
    delete explosionShader;
    glDeleteVertexArrays(1, &particleVAO);
    glDeleteBuffers(1, &particleVBO);
    glDeleteVertexArrays(1, &explosionVAO);
    glDeleteBuffers(1, &explosionVBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec2 orbitInput(0.0f);
    float zoomInput = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        orbitInput.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        orbitInput.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        orbitInput.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        orbitInput.y -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        zoomInput -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        zoomInput += 1.0f;

    if (orbitInput.x != 0.0f || orbitInput.y != 0.0f || zoomInput != 0.0f) {
        float yawDelta = orbitInput.x * camera.orbitRotateSpeed * deltaTime;
        float pitchDelta = orbitInput.y * camera.orbitRotateSpeed * deltaTime;
        float radiusDelta = zoomInput * camera.orbitZoomSpeed * deltaTime;
        applyOrbitDelta(yawDelta, pitchDelta, radiusDelta);
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // Start mimimi animation on E press
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        mimimiPlaying = true;
        mimimiCurrentFrame = 0;
        mimimiFrameTime = 0.0f;
    }
    // Start laugh animation on L press
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        laughPlaying = true;
        laughCurrentFrame = 0;
        laughFrameTime = 0.0f;
    }
    // Start mega_knight jump on M press
    if (key == GLFW_KEY_M && action == GLFW_PRESS && !megaKnightJumping)
    {
        megaKnightJumping = true;
        megaKnightY = megaKnightStartY; // Start from high up
        megaKnightVelocityY = 0.0f;     // Start with zero velocity (free fall)
    }

    // Toggle mega_knight walking on G press
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        megaKnightWalking = !megaKnightWalking;
        megaKnightWalkTime = 0.0f;

        // Reset position when stopping walk
        if (!megaKnightWalking)
        {
            megaKnightX = megaKnightStartX;
            megaKnightZ = megaKnightStartZ;
            // Update model matrix to initial position
            megaKnightModelMatrix = glm::mat4(1.0f);
            megaKnightModelMatrix = glm::translate(megaKnightModelMatrix, glm::vec3(megaKnightX, megaKnightGroundY, megaKnightZ));
            megaKnightModelMatrix = glm::scale(megaKnightModelMatrix, glm::vec3(2.0f));
            megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            megaKnightModelMatrix = glm::rotate(megaKnightModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }
    if (key == GLFW_KEY_0 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 0;
    if (key == GLFW_KEY_1 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 1;
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 2;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        shaderProgramIndex = 3;
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        shaderProgramIndex = 4;
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        shaderProgramIndex = 5;
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
        shaderProgramIndex = 6;
    if (key == GLFW_KEY_7 && action == GLFW_PRESS)
        shaderProgramIndex = 7;
    if (key == GLFW_KEY_8 && action == GLFW_PRESS)
        shaderProgramIndex = 8;
    if( key == GLFW_KEY_9 && action == GLFW_PRESS)
        isCube = !isCube;
    
    // R key to launch rocket
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        if (!rocketFlying) {
            rocketVisible = true;
            rocketFlying = true;
            rocketFlightTime = 0.0f;
            rocket_pos = rocketStartPos;
            rocketx = rocketStartPos.x;
            rockety = rocketStartPos.y;
        }
    }
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// append
unsigned int loadTexture(const char *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = (nrComponents == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

unsigned int loadCubemap(vector<std::string>& faces)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}  
