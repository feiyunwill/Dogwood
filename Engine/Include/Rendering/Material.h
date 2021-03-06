#pragma once

#include "Math\Transform.h"
#include "Color.h"
#include "GameComponent.h"
#include "ShaderProgram.h"

#include <unordered_map>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

using std::unordered_map;
using std::vector;

class MeshInstance;
class Texture;

class Material
{
public:
    void            SetMesh(MeshInstance* mesh);
    void            SetShader(ShaderProgram* shader);
    void            SetColor(GLint paramID, ColorRGB color);
    void            SetColor(string paramName, ColorRGB color);
    void            SetTexture(GLint paramID, Texture* texture);
    void            SetTexture(string paramName, Texture* texture);

    MeshInstance*   GetMesh();
    ShaderProgram*  GetShader();
    ColorRGB        GetColor(GLint paramID);
    ColorRGB        GetColor(string paramName);
    Texture*        GetTexture(GLint paramID);
    Texture*        GetTexture(string paramName);
    unordered_map<GLint, ColorRGB>& GetColorList();
    unordered_map<GLint, Texture*>&  GetTextureList();

    void            ApplyMaterial(Transform& transform, GLint posVBO, GLint normVBO, GLint uvVBO, bool useUVs);
    void            UnapplyMaterial();

    Material*       DeepCopy();

private:
    void            SetUniformParam(GLint paramID, ColorRGB& color);
    void            SetAttribParam(GLint paramID, GLint buffer, int size);
    void            DisableAttribArray(GLint paramID);

    MeshInstance*                   m_mesh;
    ShaderProgram*                  m_shader;
    unordered_map<GLint, ColorRGB>  m_colors;
    unordered_map<GLint, Texture*>  m_textures;

    GLint                           m_positionParamID;
    GLint                           m_normalParamID;
    GLint                           m_uvParamID;
    GLint                           m_modelID;
};