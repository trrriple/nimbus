#pragma once
#include "nimbus/common.hpp"
#include "nimbus/utility.hpp"

namespace nimbus
{

typedef void* Body2D;

class Physics2D
{
   public:
    enum class BodyType
    {
        STATIC,
        KINEMATIC,
        DYNAMIC
    };

    struct RigidBodySpec
    {
        glm::vec2 position        = {0.0f, 0.0f};
        float     angle           = 0.0f;
        glm::vec2 linearVelocity  = {0.0f, 0.0f};
        float     angularVelocity = 0.0f;
        float     linearDamping   = 0.0f;
        float     angularDamping  = 0.0f;
        bool      allowSleep      = true;
        bool      awake           = true;
        bool      fixedRotation   = false;
        bool      bullet          = false;
        BodyType  type            = BodyType::STATIC;
        bool      enabled         = true;
        float     gravityScale    = 1.0f;
    };

    enum class ShapeType
    {
        RECTANGLE,
        CIRCLE,
    };

    struct Shape
    {
        ShapeType type;
        glm::vec2 offset = {0.0f, 0.0f};

        Shape(ShapeType t) : type(t)
        {
        }
    };

    struct Rectangle : Shape
    {
        glm::vec2 size = {0.5f, 0.5f};

        Rectangle() : Shape(ShapeType::RECTANGLE)
        {
        }
    };

    struct Circle : Shape
    {
        float radius = 0.0f;

        Circle() : Shape(ShapeType::CIRCLE)
        {
        }
    };

    struct FixtureSpec
    {
        Shape* shape                = nullptr;
        float  friction             = 0.2f;
        float  restitution          = 0.0f;
        float  restitutionThreshold = 1.0f;
        float  density              = 0.0f;
        float  isSensor             = false;
    };

    Physics2D();
    ~Physics2D();

    Body2D addRigidBody(const RigidBodySpec& spec);

    void addFixture(Body2D&                body,
                    const FixtureSpec&     FixtureSpec,
                    const util::Transform& transform);

   private:
    struct WorldData;
    WorldData* m_worldData;

    uint32_t _bodyType(BodyType bodyType) const;
};

}  // namespace nimbus