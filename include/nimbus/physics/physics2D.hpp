#pragma once
#include "nimbus/core/common.hpp"
#include "nimbus/core/utility.hpp"

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
        float radius = 0.5f;

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

    void update(float deltaTime);

    ref<RigidBody> addRigidBody(const RigidBodySpec& spec);
    void           removeRigidBody(ref<RigidBody>& p_body);

   private:
    const int32_t k_solverVelocityIterations = 6;
    const int32_t k_solverPositionIterations = 2;

    struct WorldData;
    WorldData* mp_worldData;

    uint32_t _bodyType(BodyType bodyType) const;
};

}  // namespace nimbus