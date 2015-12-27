#pragma once

#include "tinyxml2.h"
#include "Math/Algebra.h"
#include "Math/Transform.h"
#include "Rendering/Color.h"

class DebugCapsule;
class GameObjectBase;

class Collider
{
public:
    enum ColliderType { SPHERE_COLLIDER, BOX_COLLIDER, CAPSULE_COLLIDER };

    Collider(GameObjectBase* gameObject);
    virtual ~Collider();

    static Collider*        LoadFromXML(GameObjectBase* gameObject, tinyxml2::XMLElement* xml);
    static void             AddToGameObject(GameObjectBase* gameObject, ColliderType type);

    virtual void            LoadFromXML(tinyxml2::XMLElement* xml) = 0;
    virtual void            Serialize(tinyxml2::XMLNode* parentNode, tinyxml2::XMLDocument& rootDoc) = 0;
    virtual ColliderType    GetType() = 0;
    virtual float           GetWorldspaceBoundingRadius() = 0;
    virtual void            DebugDraw(ColorRGB color, bool useDepth = true);

    Vector3                 GetWorldPosition();
    GameObjectBase*         GetGameObject();
    Transform&              GetTransform();

    bool                    IsStatic();
    Vector3                 GetCenter();

    void                    SetStatic(bool isStatic);
    void                    SetCenter(Vector3 center);

protected:
    bool                    m_isStatic;
    GameObjectBase*         m_gameObject;
    Transform               m_transform;
    Vector3                 m_center;
};

class SphereCollider : public Collider
{
public:
    SphereCollider(GameObjectBase* gameObject, float radius = 1.0f);

    virtual void            LoadFromXML(tinyxml2::XMLElement* xml);
    virtual void            Serialize(tinyxml2::XMLNode* parentNode, tinyxml2::XMLDocument& rootDoc);
    virtual ColliderType    GetType();
    virtual float           GetWorldspaceBoundingRadius();
    virtual void            DebugDraw(ColorRGB color, bool useDepth = true);

    float                   GetLocalRadius();
    void                    SetLocalRadius(float radius);

private:
    float                   m_radius;
};

class BoxCollider : public Collider
{
public:
    BoxCollider(GameObjectBase* gameObject);

    virtual void            LoadFromXML(tinyxml2::XMLElement* xml);
    virtual void            Serialize(tinyxml2::XMLNode* parentNode, tinyxml2::XMLDocument& rootDoc);
    virtual ColliderType    GetType();
    virtual float           GetWorldspaceBoundingRadius();
    virtual void            DebugDraw(ColorRGB color, bool useDepth = true);

    Vector3                 GetLocalSize();
    void                    SetLocalSize(Vector3 size);

    Vector3                 GetWorldScaleHalfsize();

private:
    Vector3                 m_size;
};

class CapsuleCollider : public Collider
{
public:
    CapsuleCollider(GameObjectBase* gameObject, float radius = 1, float height = 2, eAXIS axis = AXIS_Y);
    ~CapsuleCollider();

    virtual void            LoadFromXML(tinyxml2::XMLElement* xml);
    virtual void            Serialize(tinyxml2::XMLNode* parentNode, tinyxml2::XMLDocument& rootDoc);
    virtual ColliderType    GetType();
    virtual float           GetWorldspaceBoundingRadius();
    virtual void            DebugDraw(ColorRGB color, bool useDepth = true);

    float                   GetLocalRadius();
    float                   GetLocalHeight();
    eAXIS                   GetAxis();

    void                    SetLocalRadius(float radius);
    void                    SetLocalHeight(float height);
    void                    SetAxis(eAXIS axis);

private:
    float                   CalculateWorldRadius();
    float                   CalculateWorldHeight();

    void                    RefreshDebugInfo();

    float                   m_radius;
    float                   m_height;
    eAXIS                   m_axis;

    DebugCapsule*           m_debugCapsule;
};