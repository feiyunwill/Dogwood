#pragma once

#include "Math\Algebra.h"
#include "Math\Transform.h"
#include "Rendering\Color.h"

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

class Material;
class ShaderProgram;

// TODO put debug draw shapes in their own file

class DebugPrimitive
{
public:
    virtual void    Draw(Matrix4x4& transform, ColorRGB& color, bool useDepth = true);

protected:
    GLuint          m_positionBufferID;
    GLuint          m_vertexArrayID;
    GLuint          m_ebo;
    Vector3*        m_positionBufferData;
    GLuint*         m_indices;
    ShaderProgram*  m_shader;

    int             m_numIndices;
};

class DebugSphere : public DebugPrimitive
{
public:
    ~DebugSphere();
    void            Init(float radius, int divisions);
};

class DebugCube : public DebugPrimitive
{
public:
    ~DebugCube();
    void            Init();
};

class DebugCapsule : public DebugPrimitive
{
public:
    ~DebugCapsule();
    void            Init(float radius, float height, int divisions, eAXIS axis);
    virtual void    Draw(Matrix4x4& transform, ColorRGB& color, bool useDepth = true);

private:
    eAXIS           m_axis;
};

struct Pyramid
{
public:
    void            Init(float base, float height);
    void            Draw(Matrix4x4& transform, ColorRGB& color);

private:
    GLuint          m_positionBufferID;
    GLuint          m_vertexArrayID;
    GLuint          m_ebo;
    Vector3         m_positionBufferData[4];
    GLuint          m_indices[4 * 3];           // 4 triangles * 3 points per triangle
    ShaderProgram*  m_shader;
};

struct Gnomon
{
public:
    void            Init(float arrowBase = 0.1f, float arrowHeight = 0.2f);
    void            Draw(Transform& transform);
    Transform       GetScaledTransform(Transform& transform);       // TODO ugh, hacks

private:
    GLuint          m_positionBufferID;
    GLuint          m_colorBufferID;
    GLuint          m_vertexArrayID;
    Vector3         m_positionBufferData[3 * 2];    // 3 lines * 2 points per line
    ColorRGB        m_colorBufferData[3 * 2];
    ShaderProgram*  m_shader;

    Pyramid         m_arrow;
    Matrix4x4       m_arrowTransforms[3];
};

class DebugDraw
{
public:
    const static int MAX_LINES_PER_FRAME = 20;  // TODO this is pretty lame. resize dynamically

    static DebugDraw& Singleton()
    {
        static DebugDraw    singleton;
        return singleton;
    }
    DebugDraw() {}

    void            Startup();
    void            Shutdown();

    void            DrawLine(Vector3& a, Vector3& b, ColorRGB& Color);       // draw line for one frame
    void            RenderLines();

    void            PrepareLineBuffer(Vector3* buffer, int count, GLuint &vao, GLuint &vbo);
    void            DrawLineBuffer(GLuint vao, GLuint vbo, Vector3* buffer, int size, ColorRGB color);

    void            DrawSphere(Matrix4x4& transform, ColorRGB color, bool useDepth = true);
    void            DrawCube(Matrix4x4& transform, ColorRGB color, bool useDepth = true);
    void            DrawCapsule(Matrix4x4& transform, ColorRGB color, bool useDepth = true);

    Material*       GetDebugMaterial();

private:
    void            SetupDebugMat();

    GLuint          m_vertexBufferID;
    GLuint          m_ColorBufferID;
    GLuint          m_vertexArrayID;
    Vector3         m_vertexBufferData[MAX_LINES_PER_FRAME * 2];
    ColorRGB        m_lineColors[MAX_LINES_PER_FRAME * 2];
    int             m_numLines = 0;

    Material*       m_material;
    ShaderProgram*  m_shader;

    DebugSphere     m_debugSphere;
    DebugCube       m_debugCube;
    DebugCapsule    m_debugCapsule;
};