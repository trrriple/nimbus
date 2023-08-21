#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/utility.hpp"

namespace nimbus
{

typedef void* Body2D;

class Physics2D : public refCounted
{
   public:
    enum class BodyType
    {
        STATIC = 0,
        KINEMATIC,
        DYNAMIC
    };

    struct RigidBodySpec
    {
        glm::vec2 position        = {0.0f, 0.0f};
        f32_t     angle           = 0.0f;
        glm::vec2 linearVelocity  = {0.0f, 0.0f};
        f32_t     angularVelocity = 0.0f;
        f32_t     linearDamping   = 0.0f;
        f32_t     angularDamping  = 0.0f;
        bool      allowSleep      = true;
        bool      awake           = true;
        bool      fixedRotation   = false;
        bool      bullet          = false;
        BodyType  type            = BodyType::STATIC;
        bool      enabled         = true;
        f32_t     gravityScale    = 1.0f;
    };

    enum class ShapeType
    {
        NONE,
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
        f32_t radius = 0.5f;

        Circle() : Shape(ShapeType::CIRCLE)
        {
        }
    };

    struct FixtureSpec
    {
        Shape* shape                = nullptr;
        f32_t  friction             = 0.2f;
        f32_t  restitution          = 0.0f;
        f32_t  restitutionThreshold = 1.0f;
        f32_t  density              = 1.0f;
        bool   isSensor             = false;
    };

    class RigidBody : public refCounted
    {
       public:
        struct RigidBodyData;

        RigidBodyData*  p_data;
        util::Transform transform;
        bool            inWorld        = false;
        std::string     name           = "setMe";
        ref<RigidBody>  p_collidedWith = nullptr;
        void*           p_userData     = nullptr;

        void addFixture(const FixtureSpec& fixtureSpec, const util::Transform& transform);

        util::Transform& getTransform();
        glm::vec2        getVelocity();
        void             forceTransform();
        void             forceVelocity(const glm::vec2& velocity);
        void             impulse(const glm::vec2& velocity);
        void             halt();
        void             removeFromWorld();

        RigidBody();
        ~RigidBody();
    };

    Physics2D();
    ~Physics2D();

    void update(f32_t deltaTime);

    ref<RigidBody> addRigidBody(const RigidBodySpec& spec);
    void           removeRigidBody(ref<RigidBody>& p_body);

   private:
    const i32_t k_solverVelocityIterations = 8;
    const i32_t k_solverPositionIterations = 3;

    struct WorldData;
    WorldData* mp_worldData;

    u32_t _bodyType(BodyType bodyType) const;
};

}  // namespace nimbus