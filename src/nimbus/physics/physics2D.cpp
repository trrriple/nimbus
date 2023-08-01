#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/physics/physics2D.hpp"
#include "box2d/box2d.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////
// Callback class for Box2D Contacts/Collisions
////////////////////////////////////////////////////////////////////////////////
class ContactListener : public b2ContactListener
{
   public:
    ContactListener(Physics2D* p_physics2D) : p_physics2D(p_physics2D)
    {
    }
    ~ContactListener()
    {
        p_physics2D = nullptr;
    }

    void BeginContact(b2Contact* contact) override
    {
        Physics2D::RigidBody* p_bodyA = reinterpret_cast<Physics2D::RigidBody*>(
            contact->GetFixtureA()->GetUserData().pointer);
        Physics2D::RigidBody* p_bodyB = reinterpret_cast<Physics2D::RigidBody*>(
            contact->GetFixtureB()->GetUserData().pointer);

        p_bodyA->p_collidedWith = p_bodyB->shared_from_this();
        p_bodyB->p_collidedWith = p_bodyA->shared_from_this();
    }

    void EndContact(b2Contact* contact) override
    {
        UNUSED(contact);
    }

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
    {
        UNUSED(contact);
        UNUSED(oldManifold);
    }

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
    {
        UNUSED(contact);
        UNUSED(impulse);
    }

    void registerSave()
    {
        
    }

   private:
    Physics2D*                                p_physics2D = nullptr;
    // std::unordered_map< Physics2D::RigidBody*> collisionSaves;
};

struct Physics2D::WorldData
{
    ref<b2World>           p_world = nullptr;
    scope<ContactListener> p_cl    = nullptr;
};

struct Physics2D::RigidBody::RigidBodyData
{
    b2Body*    p_body;
    b2Fixture* p_fixture;
};

////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////
Physics2D::Physics2D() : mp_worldData(new Physics2D::WorldData())
{
    mp_worldData->p_world = makeRef<b2World>(b2Vec2(0.0f, -9.81f));

    mp_worldData->p_cl = makeScope<ContactListener>(this);

    mp_worldData->p_world->SetContactListener(mp_worldData->p_cl.get());
}
Physics2D::~Physics2D()
{
    delete mp_worldData;
}

void Physics2D::update(float deltaTime)
{
    mp_worldData->p_world->Step(
        deltaTime, k_solverVelocityIterations, k_solverPositionIterations);
}


ref<Physics2D::RigidBody> Physics2D::addRigidBody(const RigidBodySpec& spec)
{
    ref<Physics2D::RigidBody> p_rbody = makeRef<Physics2D::RigidBody>();

    b2BodyDef bodyDef;

    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(&p_rbody);

    bodyDef.position = b2Vec2(spec.position.x, spec.position.y);
    bodyDef.angle    = spec.angle;
    bodyDef.linearVelocity
        = b2Vec2(spec.linearVelocity.x, spec.linearVelocity.y);
    bodyDef.angularVelocity = spec.angularVelocity;
    bodyDef.linearDamping   = spec.linearDamping;
    bodyDef.angularDamping  = spec.angularDamping;
    bodyDef.allowSleep      = spec.allowSleep;
    bodyDef.awake           = spec.awake;
    bodyDef.fixedRotation   = spec.fixedRotation;
    bodyDef.bullet          = spec.bullet;
    bodyDef.type            = static_cast<b2BodyType>(_bodyType(spec.type));
    bodyDef.enabled         = spec.enabled;
    bodyDef.gravityScale    = spec.gravityScale;

    p_rbody->p_data->p_body = mp_worldData->p_world->CreateBody(&bodyDef);

    return p_rbody;
}

void Physics2D::removeRigidBody(ref<Physics2D::RigidBody>& body)
{
    mp_worldData->p_world->DestroyBody(body->p_data->p_body);
}

////////////////////////////////////////////////////////////////////////////////
// RigidBody Functions
////////////////////////////////////////////////////////////////////////////////
Physics2D::RigidBody::RigidBody() : p_data(new RigidBodyData())
{
}

Physics2D::RigidBody::~RigidBody()
{
    delete p_data;
} 

void Physics2D::RigidBody::addFixture(const FixtureSpec&     fixtureSpec,
                                      const util::Transform& transform)
{
    b2FixtureDef fixtureDef;

    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    ///////////////////////////
    // Set Shape specifics
    ///////////////////////////
    NM_CORE_ASSERT(fixtureSpec.shape, "Fixture Shape cannot be null!");

    if (fixtureSpec.shape->type == ShapeType::RECTANGLE)
    {
        const Rectangle* rect
            = static_cast<const Rectangle*>(fixtureSpec.shape);

        b2PolygonShape rectShape;
        rectShape.SetAsBox(rect->size.x * transform.scale.x,
                           rect->size.y * transform.scale.y,
                           b2Vec2(rect->offset.x, rect->offset.y),
                           0.0f);

        fixtureDef.shape = &rectShape;
    }
    else if (fixtureSpec.shape->type == ShapeType::CIRCLE)
    {
        const Circle* circle = static_cast<const Circle*>(fixtureSpec.shape);
        b2CircleShape circleShape;
        circleShape.m_p.Set(circle->offset.x, circle->offset.y);
        circleShape.m_radius = transform.scale.x * circle->radius;

        fixtureDef.shape = &circleShape;
    }

    ///////////////////////////
    // General attributes
    ///////////////////////////
    fixtureDef.friction             = fixtureSpec.friction;
    fixtureDef.restitution          = fixtureSpec.restitution;
    fixtureDef.restitutionThreshold = fixtureSpec.restitutionThreshold;
    fixtureDef.density              = fixtureSpec.density;
    fixtureDef.isSensor             = fixtureSpec.isSensor;

    p_data->p_fixture =   p_data->p_body->CreateFixture(&fixtureDef);
}

util::Transform& Physics2D::RigidBody::getTransform()
{
    const auto& position    = p_data->p_body->GetPosition();
    transform.translation.x = position.x;
    transform.translation.y = position.y;
    transform.rotation.z    = p_data->p_body->GetAngle();

    return transform;
}

glm::vec2 Physics2D::RigidBody::getVelocity()
{
    auto velocity = p_data->p_body->GetLinearVelocity();
    return {velocity.x, velocity.y};
}

void Physics2D::RigidBody::forceTransform()
{
    p_data->p_body->SetTransform(
        b2Vec2(transform.translation.x, transform.translation.y),
        transform.rotation.z);
}

void Physics2D::RigidBody::forceVelocity(const glm::vec2& velocity)
{
    p_data->p_body->SetLinearVelocity({velocity.x, velocity.y});
}

void Physics2D::RigidBody::impulse(const glm::vec2& impulse)
{
    p_data->p_body->ApplyLinearImpulseToCenter({impulse.x, impulse.y}, true);
}

////////////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////////////
uint32_t Physics2D::_bodyType(BodyType bodyType) const
{
    switch (bodyType)
    {
        case (BodyType::STATIC):
        {
            return b2_staticBody;
        }
        case (BodyType::KINEMATIC):
        {
            return b2_kinematicBody;
        }
        case (BodyType::DYNAMIC):
        {
            return b2_dynamicBody;
        }
        default:
        {
            NM_CORE_ASSERT(false, "Unkown body type %i", bodyType);
            return b2_staticBody;
        }
    }
}

}  // namespace nimbus