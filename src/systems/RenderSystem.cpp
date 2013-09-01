#include "engine/systems/RenderSystem.h"

#include "engine/components/Light.h"
#include "engine/utils/MeshLoader.h"

#define GLDBG printf("%s:%i - %x\n", __FILE__, __LINE__, glGetError());

RenderSystem::RenderSystem(std::string windowTitle, int windowWidth, int windowHeight) :
	shaderManager_(ShaderManager::getInstance()) {

	if (!glfwInit())
		return;

	// TODO: Check how much of an impact this has for the rest of the rendering
	// Enable AA for the HUD
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!glfwOpenWindow(windowWidth, windowHeight, 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
		return;

	glfwSetWindowTitle(windowTitle.c_str());

	glfwDisable(GLFW_MOUSE_CURSOR);

	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);

   	// For light volumes
   	glCullFace(GL_FRONT);

   	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenFramebuffers(1, &frameBuffer_);
	glGenRenderbuffers(1, &depthBuffer_);

	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);

	diffuseTexture_ = generateTexture(GL_RGBA, GL_UNSIGNED_BYTE);
	positionTexture_ = generateTexture(GL_RGBA32F, GL_FLOAT);
	normalTexture_ = generateTexture(GL_RGBA16F, GL_FLOAT);
	lightTexture_ = generateTexture(GL_RGBA, GL_UNSIGNED_BYTE);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseTexture_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, positionTexture_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normalTexture_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, lightTexture_, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat data[12] = {
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};

	// Load render quad
	glGenVertexArrays(1, &squareVertexArray_);
	glGenBuffers(1, &squareBuffer_);
	glBindVertexArray(squareVertexArray_);
	glBindBuffer(GL_ARRAY_BUFFER, squareBuffer_);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	// Load light volume
	MeshShape *lightVolumeMesh = MeshLoader::load("models/point_light_volume.obj");
	glGenVertexArrays(1, &lightVolume_.vertexArray);
	glGenBuffers(1, &lightVolume_.buffer);
	loadVertexArrayObject(lightVolumeMesh, &lightVolume_);

	glPolygonOffset(1.0f, 1.0f);
}

RenderSystem::~RenderSystem() {

}

void RenderSystem::update(double deltaTime) {
	int windowWidth, windowHeight;
	glfwGetWindowSize(&windowWidth, &windowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
	GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, buffers);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//shaderManager_.loadShader("shaders/deferred_first");
	glDrawBuffers(4, buffers);

	// Render all the standard meshes
	const Entity *defaultCamera = cameras_[CAMERA_DEFAULT];
	if (defaultCamera) {
		std::string lastShader;
		for (auto cache : orderedMeshCaches_[CAMERA_DEFAULT]) {
			const Entity *entity = cache->entity;
			const Renderer *renderer = entity->get<Renderer>();

			// TODO: Implement this in a more efficient way? Maybe remove from render collection when not visible.
			if (!renderer->visible())
				continue;

			if (renderer->shader() != lastShader) {
				shaderManager_.loadShader(renderer->shader());
				lastShader = renderer->shader();
				shaderManager_.setProjectionMatrix(defaultCamera->get<Camera>()->projection(windowWidth, windowHeight));
				shaderManager_.setViewMatrix(defaultCamera->get<Transform>()->transform().inverse());
			}

			if (!entity->get<Renderer>()->depthTest())
				glDisable(GL_DEPTH_TEST);

			if (entity->get<Renderer>()->blend())
				glEnable(GL_BLEND);

			if (entity->get<Renderer>()->overlayed())
				glEnable(GL_POLYGON_OFFSET_FILL);

			shaderManager_.setModelMatrix(entity->get<Transform>()->transform());

			glBindVertexArray(cache->vertexArrayObject.vertexArray);
			glDrawArrays(GL_TRIANGLES, 0, cache->vertexArrayObject.vertexCount);
			glBindVertexArray(0);

			if (entity->get<Renderer>()->blend())
				glDisable(GL_BLEND);

			if (entity->get<Renderer>()->overlayed())
				glDisable(GL_POLYGON_OFFSET_FILL);

			if (!entity->get<Renderer>()->depthTest())
				glEnable(GL_DEPTH_TEST);
		}
	}

	// Render lights
	shaderManager_.loadShader("shaders/point_light");
	shaderManager_.setResolution(windowWidth, windowHeight);
	GLenum lightBuffers[] = {GL_NONE, GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT3};

	shaderManager_.setProjectionMatrix(defaultCamera->get<Camera>()->projection(windowWidth, windowHeight));
	shaderManager_.setViewMatrix(defaultCamera->get<Transform>()->transform().inverse());

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDrawBuffers(4, lightBuffers);
	//glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, positionTexture_);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normalTexture_);
	for (Entity *light : lights_) {
		shaderManager_.setModelMatrix(light->get<Transform>()->transform());
		//shaderManager_.setLightPosition(light->get<Transform>()->transform().getOrigin());
		shaderManager_.setLight(light);
		glBindVertexArray(lightVolume_.vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, lightVolume_.vertexCount);
		glBindVertexArray(0);
	}
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	shaderManager_.loadShader("shaders/deferred_final");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightTexture_);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture_);

	glBindVertexArray(squareVertexArray_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Render HUD
	const Entity *hudCamera = cameras_[CAMERA_HUD];
	if (hudCamera) {
		std::string lastShader;
		for (auto cache : orderedMeshCaches_[CAMERA_HUD]) {
			const Entity *entity = cache->entity;
			const Renderer *renderer = entity->get<Renderer>();

			// TODO: Implement this in a more efficient way? Maybe remove from render collection when not visible.
			if (!renderer->visible())
				continue;

			if (renderer->shader() != lastShader) {
				shaderManager_.loadShader(renderer->shader());
				lastShader = renderer->shader();
				shaderManager_.setProjectionMatrix(hudCamera->get<Camera>()->projection(windowWidth, windowHeight));
				shaderManager_.setViewMatrix(hudCamera->get<Transform>()->transform().inverse());
			}

			if (entity->get<Renderer>()->blend())
				glEnable(GL_BLEND);

			shaderManager_.setModelMatrix(entity->get<Transform>()->transform());

			glBindVertexArray(cache->vertexArrayObject.vertexArray);
			glDrawArrays(GL_TRIANGLES, 0, cache->vertexArrayObject.vertexCount);
			glBindVertexArray(0);

			if (entity->get<Renderer>()->blend())
				glDisable(GL_BLEND);
		}
	}

	glEnable(GL_DEPTH_TEST);

	glfwSwapBuffers();
}

SystemType RenderSystem::type() const {
	return TYPE;
}

void RenderSystem::setBackgroundColor(float red, float green, float blue) {
	glClearColor(red, green, blue, 1.0);
}

void RenderSystem::addMeshCache(Entity *entity) {
	MeshCache *cache = new MeshCache();
	cache->entity = entity;
	cache->cameraType = entity->get<Renderer>()->cameraType();
	cache->shader = entity->get<Renderer>()->shader();
	cache->blend = entity->get<Renderer>()->blend();
	cache->depthTest = entity->get<Renderer>()->depthTest();

	glGenVertexArrays(1, &cache->vertexArrayObject.vertexArray);
	glGenBuffers(1, &cache->vertexArrayObject.buffer);
	meshCaches_[entity] = cache;

	orderedMeshCaches_[cache->cameraType].insert(cache);

	updateMeshCache(entity);
}

void RenderSystem::removeMeshCache(Entity *entity) {
	MeshCache *cache = meshCaches_[entity];

	meshCaches_.erase(entity);
	orderedMeshCaches_[cache->cameraType].erase(cache);

	glDeleteBuffers(1, &cache->vertexArrayObject.buffer);

	delete cache;
}

void RenderSystem::onChangeType(Entity *entity, int fromType, int toType) {
	const bool fromCamera = (fromType & Camera::TYPE) && (fromType & Transform::TYPE);
	const bool toCamera = (toType & Camera::TYPE) && (toType & Transform::TYPE);

	if (!fromCamera && toCamera) {
		// A camera was attached. Use it if a camera of its type is not already set.
		if (!cameras_[entity->get<Camera>()->cameraType()])
			cameras_[entity->get<Camera>()->cameraType()] = entity;
	} else if (fromCamera && !toCamera) {
		// A camera was detached. If it is the currently set camera of its type, reset it.
		if (cameras_[entity->get<Camera>()->cameraType()] == entity)
			cameras_.erase(entity->get<Camera>()->cameraType());
	}

	const bool fromMesh = (fromType & Renderer::TYPE) && (fromType & Transform::TYPE) && (fromType & MeshShape::TYPE);
	const bool toMesh = (toType & Renderer::TYPE) && (toType & Transform::TYPE) && (toType & MeshShape::TYPE);

	if (!fromMesh && toMesh) {
		// A mesh shape was attached
		addMeshCache(entity);
		entity->subscribe(MESSAGE_MESH_CHANGED, this);
	} else if (fromMesh && !toMesh) {
		// A mesh shape was detached
		entity->unsubscribe(MESSAGE_MESH_CHANGED, this);
		removeMeshCache(entity);
	}

	const bool fromLight = (fromType & Light::TYPE) && (fromType & Transform::TYPE);
	const bool toLight = (toType & Light::TYPE) && (toType & Transform::TYPE);

	if (!fromLight && toLight) {
		lights_.insert(entity);
	} else if (fromLight && !toLight) {
		lights_.erase(entity);
	}
}

void RenderSystem::onMessage(MessageType messageType, Entity *entity) {
	updateMeshCache(entity);
}

void RenderSystem::onAttach(Game *game) {

}

void RenderSystem::updateMeshCache(Entity *entity) {
	MeshCache *cache = meshCaches_[entity];
	loadVertexArrayObject(entity->get<MeshShape>(), &cache->vertexArrayObject);
}

void RenderSystem::loadVertexArrayObject(MeshShape *mesh, VertexArrayObject *vertexArrayObject) {
	struct OpenGlVertex {
		GLfloat position[3];
		GLfloat normal[3];
		GLubyte red;
		GLubyte green;
		GLubyte blue;
		GLubyte alpha;
	};

	const auto &triangles = mesh->triangles();
	vertexArrayObject->vertexCount = triangles.size() * 3;

	OpenGlVertex *vertexData = new OpenGlVertex[vertexArrayObject->vertexCount];

	OpenGlVertex *currentVertex = vertexData;
	for (const Triangle *triangle : triangles) {
		// Calculate triangle normal
		btVector3 a(triangle->vertices[1]->vector - triangle->vertices[0]->vector);
		btVector3 b(triangle->vertices[2]->vector - triangle->vertices[0]->vector);
		btVector3 normal = a.cross(b).normalized();

		for (int i = 0; i < 3; ++i) {
			currentVertex->position[0] = triangle->vertices[i]->vector.x();
			currentVertex->position[1] = triangle->vertices[i]->vector.y();
			currentVertex->position[2] = triangle->vertices[i]->vector.z();

			currentVertex->normal[0] = normal.x();
			currentVertex->normal[1] = normal.y();
			currentVertex->normal[2] = normal.z();

			currentVertex->red = triangle->color.red;
			currentVertex->green = triangle->color.green;
			currentVertex->blue = triangle->color.blue;
			currentVertex->alpha = triangle->color.alpha;

			currentVertex++;
		}
	}

	glBindVertexArray(vertexArrayObject->vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayObject->buffer);
	glBufferData(GL_ARRAY_BUFFER, vertexArrayObject->vertexCount * sizeof(OpenGlVertex), vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position
	glEnableVertexAttribArray(1); // Normal
	glEnableVertexAttribArray(2); // Color

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OpenGlVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OpenGlVertex), (void *)(sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(OpenGlVertex), (void *)(sizeof(GLfloat) * 6));

	glBindVertexArray(0);

	delete[] vertexData;
}

GLuint RenderSystem::generateTexture(GLint internalFormat, GLenum type) {
	int width, height;
	glfwGetWindowSize(&width, &height);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, type, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
}
