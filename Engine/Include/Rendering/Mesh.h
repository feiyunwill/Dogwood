#pragma once

#include "..\Math\Transform.h"
#include "..\Scene\Resource.h"
#include "ShaderProgram.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <vector>

class Material;
struct ResourceInfo;

class Mesh : public Resource
{
public:
    Mesh(std::string filename, ResourceInfo* resourceInfo);

    void Render(Transform& transform, Material* material);
    void Delete();

private:
    GLuint      m_vao;
    GLuint      m_vboPosition;
    GLuint      m_vboNormal;
    GLuint      m_vboUV;
    GLuint      m_ebo;

    GLsizei     m_vertexCount;
    GLsizei     m_indexedVertexCount;

    GLenum      m_drawMode;
};