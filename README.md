# engine
Engine is a work in progress entity-component-system based 3D game engine.

Some highlights:

* Physics support
* Deferred rendering used to optimize for many light sources
* Support for loading .obj models

# Sample
```c++
// We are going to use render, physics and input systems
attach(new RenderSystem("monkey"));
attach(new PhysicsSystem());
attach(new InputSystem());

// Set up the controllable player
Entity *player = new Entity();
player->attach(new Transform(0.0, 2.0, 0.0));
player->attach(new PhysicsCharacter());
player->attach(new InputController());
player->attach(new Camera(CAMERA_DEFAULT));
attach(player);

// Set up a point light
Entity *light = new Entity();
light->attach(new Transform(1.0, 2.0, 0.0));
light->attach(new Light(LIGHT_POINT, Color(255, 255, 255)));
attach(light);

// Set up a monkey
Entity *monkey = new Entity();
monkey->attach(new Transform(2.0, 1.0, 0.0));
monkey->attach(MeshLoader::load("monkey.obj", Color(200, 150, 50)));
monkey->attach(new Physics(PHYSICS_DYNAMIC));
monkey->attach(new Renderer());
attach(monkey);

// Set up the ground
Entity *ground = new Entity();
ground->attach(new Transform(0.0, 0.0, 0.0));
ground->attach(ShapeGenerator::plane(Color(200, 50, 50), 50.0, 50.0));
ground->attach(new Physics(PHYSICS_STATIC));
ground->attach(new Renderer());
attach(ground);
```

The sample code above produces the following scene:

![Sample screenshot](https://raw.github.com/rools/engine/gh-pages/screenshot.png)

# Build
The following libraries are required to build:

* [Bullet](http://bulletphysics.org/)
* [GLFW 2.7.x](http://www.glfw.org/)
* [OpenGL Extension Wrangler Library (GLEW)](http://glew.sourceforge.net/)
* [OpenGL Mathematics (GLM)](http://glm.g-truc.net/)

To build the library along with the samples, just run `make`.