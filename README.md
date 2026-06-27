# NYCU_ICG_Final_Project

[Demo video](https://youtu.be/rK46j1RQTGY)

## Introduction

This is our final project for **NYCU Introduction to Computer Graphics (ICG)**. It is a
real-time, interactive 3D scene built from scratch with **OpenGL 3.3 (core profile)**,
**GLFW**, **GLAD** and **GLM**, extended from our HW3 static-model framework.

### Story

Chiikawa and Usagi are having a match in *Clash Royale*. When Chiikawa places its favorite
card — the infamous **Mega Knight** — it thinks it's going to win and starts spamming the
**laughing emote** to provoke Usagi. Usagi answers by firing a **rocket** straight at
Chiikawa's king tower, crushing both the tower and Chiikawa itself, and then responds with a
friendly **Goblin crying emote**. Our project animates this story in a Clash Royale arena.
A core creative theme runs through it: *"the BM (Bad Manners) player shall eventually be
punished."*

The scene is a Clash Royale–style arena: Clash Royale characters (blue/red princesses and
a Mega Knight) stand around the field, while the *Chiikawa* characters (Usagi, the yellow one,
and Chiikawa, the white one) sit in the middle. A skybox surrounds everything, the ground is a
textured model, and the camera orbits the arena. The fun part is the **keyboard-triggered
effects** — emotes, character animations, and the rocket that flies in, explodes, and crowns
the winner.

Our goal was to go beyond simply loading and lighting models, and instead show off the
programmable pipeline: **geometry shaders** for particle effects, **vertex shaders** for
procedural motion, and **sprite-based emotes** for expression symbols.

## What We Did

### 1. Particle effects with geometry shaders
Two GPU particle systems, both using a `vertex → geometry → fragment` pipeline. Each
particle is uploaded as a single **point**, and the geometry shader expands it into a
**camera-facing billboard quad** (built from the `view` matrix's right/up vectors), so the
sprites always face the viewer. CPU-side physics (gravity, drag, lifetime) drive the motion,
and additive blending gives the glow.

- **Rocket flame trail** ([src/shaders/partical.geom](src/shaders/partical.geom)) — orange
  particles continuously emitted from the rocket's nozzle while it flies. The emission point is
  a local offset `(0, -1.5, 0)` rotated by the rocket's current orientation and added to its
  world position, so the flame stays attached to the tilted tail. A radial `cos/sin` spread
  gives the plume a circular cross-section instead of a thin line. To keep the trail dense but
  short: 8 particles emitted per frame, a short `maxAge` of 1.0s, and a fixed-size **circular
  buffer** (modulo indexing) that overwrites the oldest particles with no growing memory cost.
- **Explosion** ([src/shaders/explosion.geom](src/shaders/explosion.geom)) — on impact, 800
  particles burst outward in a spherical distribution. Color is animated by age over a 3-second
  lifespan to mimic thermal cooling: yellow (`age < 0.3`) → orange → red → dark grey/smoke
  (`0.6 < age < 1.0`). The billboards grow over their lifetime, and alpha fades to zero.
- **Procedural glow in the fragment shader** ([src/shaders/explosion.frag](src/shaders/explosion.frag),
  [src/shaders/partical.frag](src/shaders/partical.frag)) — instead of a texture, each quad's
  fragment computes the Euclidean distance from the quad center `(0.5, 0.5)`; fragments with
  `dist > 0.5` are discarded to carve out a circle, and a non-linear alpha decay produces a
  soft glowing falloff.

### 2. Vertex-shader motion animation
- **Walking animation** ([src/shaders/material.vert](src/shaders/material.vert)) — legs are
  detected in the vertex shader by a Y threshold and X range, then swung forward/backward with
  `sin(walkTime)`. The left and right legs run 180° out of phase, with a small vertical lift,
  producing a procedural walk cycle entirely on the GPU. Used by the Mega Knight (key **G**).
- Other motion is driven CPU-side via the model matrix: the rocket's **parabolic flight**,
  the Mega Knight's **gravity-based jump** from the sky (key **M**), the **squash-and-stretch
  idle bounce** of Usagi and Chiikawa, and the **rotating/bobbing crown**.

### 3. Expression symbols (emotes)
To bridge 2D digital media and the 3D scene, the emotes are originally GIFs whose frames we
extracted into individual PNGs, then play back **frame-by-frame** (~30 ms each) as
**sprite animations** on world-space billboards ([src/shaders/sprite.frag](src/shaders/sprite.frag))
with alpha blending — the same texture-mapping idea as HW3, driven in real time:
- **Goblin crying emote** — Usagi's reply, 71 frames, plays next to Usagi (key **E**).
- **Laughing emote** — Chiikawa's provocation, 40 frames, plays next to Chiikawa (key **L**).

### 4. Scene, lighting and extra effects
- **Skybox** via a cubemap.
- Multiple **OBJ/MTL models** rendered with Phong lighting — material-color shading for the
  Clash Royale characters and crown, texture + lighting for Chiikawa.
- **Blender authoring** — the Royal Arena was imported as an `.fbx`, found to contain three
  objects, and exported separately as `.obj` + `.mtl` with their textures bound. Models that
  shipped uncoloured (crown, blue/red princesses) were hand-coloured in Blender's edit mode
  before export.
- **Ground burn effect** ([src/shaders/ground.frag](src/shaders/ground.frag)) — the ground is
  scorched black in a radius around the rocket's impact point.
- **Rocket → explosion → reward chain**: pressing **R** launches a rocket on a parabolic arc;
  on impact it triggers the explosion, burns the ground, makes Chiikawa react (spin and turn
  black), and procedurally spawns a **rotating golden victory crown** at the landing site as
  high-reward visual feedback — the BM player gets punished, the winner gets crowned.
- **Orbiting camera** that circles the arena with zoom.

## Tech Stack

OpenGL 3.3 core · GLFW · GLAD · GLM · stb_image · CMake. Custom OBJ/MTL loader and shader
program wrapper. Models authored/edited in Blender.

## Team & Work Assignment

- **李佑軒** — geometry shader for particle explosion, story design
- **蔡凱旭** — static model loading and vertex-shader animation functions
- **蔡承志** — Blender authoring and model adjustment

## Camera Movement

**W/S/A/D:**  move foward/backward/left/right

**Shift/Space:**  move down/up

## Event Trigger

**E**: mimimimi Emote

**L**: Laughing Emote

**M**: Summon Mega knight

**G**: Mega knight walking

**R**: Rocket

## References (Models)

- [Princess](https://sketchfab.com/3d-models/princess-b9eb9fab96fa40d587bf8e678607da20)
- [Crown](https://www.myminifactory.com/object/3d-print-clash-royale-crown-76718)
- [Mega Knight](https://sketchfab.com/3d-models/mega-knight-clash-royale-reflective-model-f9433b282528498a94c8a48f3fe9210a)
- [Royal Arena (Clash Royale)](https://sketchfab.com/3d-models/arena-7-royal-arena-clash-royale-c02abc1d56914736b4b042c1993e3760)
- [Rocket](https://www.turbosquid.com/3d-models/3d-rocket---clash-royale-model-1525232)
- [Usagi](https://sketchfab.com/3d-models/usagi-edeb7465e9894aa7b0d205216ffde45d)
- [Chiikawa](https://sketchfab.com/3d-models/chiikawa-7322ae143a3e425884c238923ebbe5de)
