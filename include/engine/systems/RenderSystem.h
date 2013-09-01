#pragma once

#include <map>
#include <set>

#define GLFW_NO_GLU
#define GLFW_INCLUDE_GL3
#include <GL/glfw.h>

#include "../components/Camera.h"
#include "../components/MeshShape.h"
#include "../components/Renderer.h"
#include "../components/Transform.h"
#include "../Entity.h"
#include "../System.h"
#include "../utils/ShaderManager.h"

class RenderSystem : public System {
public:
	static const SystemType TYPE = SYSTEM_RENDER;

	RenderSystem(std::string windowTitle, int windowWidth = 1024, int windowHeight = 768);
	~RenderSystem();

	void update(double deltaTime);

	SystemType type() const;

	void setBackgroundColor(float red, float green, float blue);

private:
	struct VertexArrayObject {
		GLuint vertexArray;
		GLuint buffer;
		GLsizei vertexCount;
	};

	struct MeshCache {
		Entity *entity;

		VertexArrayObject vertexArrayObject;

		CameraType cameraType;
		std::string shader;
		bool blend;
		bool depthTest;
	};

	// TODO: Update this
	struct DrawOrderCompare {
		bool operator() (const MeshCache *cache1, const MeshCache *cache2) const {

			//if (cache1->entity->get<Renderer>()->depthTest() != cache2->entity->get<Renderer>()->depthTest())
			//	return cache2->entity->get<Renderer>()->depthTest();

			if (cache1->depthTest != cache2->depthTest)
				return cache1->depthTest > cache2->depthTest;

			if (cache1->blend != cache2->blend)
				return cache1->blend < cache2->blend;

			// Order by shader, to minimize shader switches
			if (cache1->shader == cache2->shader)
				return cache1 < cache2;

			return cache1->shader < cache2->shader;
		}
	};

	ShaderManager &shaderManager_;

	std::map<CameraType, Entity *> cameras_;

	// All attached entities with their caches
	std::map<Entity *, MeshCache *> meshCaches_;

	// Mesh caches in the correct render order
	std::map<CameraType, std::set<MeshCache *, DrawOrderCompare>> orderedMeshCaches_;

	std::set<Entity *> lights_;

	GLuint frameBuffer_;
	GLuint depthBuffer_;
	GLuint diffuseTexture_;
	GLuint positionTexture_;
	GLuint normalTexture_;
	GLuint lightTexture_;

	GLuint squareVertexArray_;
	GLuint squareBuffer_;

	VertexArrayObject lightVolume_;

	void addMeshCache(Entity *entity);
	void removeMeshCache(Entity *entity);
	void updateMeshCache(Entity *entity);

	void loadVertexArrayObject(MeshShape *mesh, VertexArrayObject *vertexArrayObject);

	void onChangeType(Entity *entity, int fromType, int toType);

	void onMessage(MessageType messageType, Entity *entity);

	void onAttach(Game *game);

	GLuint generateTexture(GLint internalFormat, GLenum type);
};