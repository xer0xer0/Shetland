#ifndef OBJECT_H
#define OBJECT_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Model.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

class Object
{
public:
	Object(std::string _meshPath, std::shared_ptr<Material> _mat, std::vector<Light> _lights);
	~Object();


	glm::vec3 GetPosition() { return position; }
    void SetPosition(glm::vec3 _position) { position = _position; }
	std::shared_ptr<Material> GetMaterial() {
		return material;
	}

	void CreateObject();
	void DrawObject();

    void RotateWorldMatrix(float _angle, float _x, float _y, float _z);
    void MoveWorldMatrix(float _x, float _y, float _z);

    glm::mat4 worldMatrix;
    GLuint transformPos;

private:

    std::shared_ptr<Model> model;
	std::shared_ptr<Material> material;
    std::vector<Light> lights;

    GLuint worldLoc;
    GLuint worldInvTransPos;
    GLuint viewLoc;
    GLuint projLoc;
    GLuint cameraPosLoc;
    
    MatLocations matLocs;

    glm::vec3 position;

	unsigned int indices[6] = {
		0, 1, 3,
		1, 2, 3
	};
};

#endif

