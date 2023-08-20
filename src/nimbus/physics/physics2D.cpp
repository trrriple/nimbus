#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/physics/physics2D.hpp"
#include "box2d/box2d.h"

namespace nimbus
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback class for Box2D Contacts/Collisions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        Physics2D::RigidBody* p_bodyA
            = reinterpret_cast<Physics2D::RigidBody*>(contact->GetFixtureA()->GetUserData().pointer);
        Physics2D::RigidBody* p_bodyB
            = reinterpret_cast<Physics2D::RigidBody*>(contact->GetFixtureB()->GetUserData().pointer);

        p_bodyA->p_collidedWith = p_bodyB;
        p_bodyB->p_collidedWith = p_bodyA;
    }

    void EndContact(b2Contact* contact) override
    {
        NB_UNUSED(contact);
    }

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
    {
        NB_UNUSED(contact);
        NB_UNUSED(oldManifold);
    }

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
    {
        NB_UNUSED(contact);
        NB_UNUSED(impulse);
    }

    void registerSave()
    {
    }

   private:
    Physics2D* p_physics2D = nullptr;
    // std::unordered_map< Physics2D::RigidBody*> collisionSaves;
};

struct Physics2D::WorldData
{
    std::shared_ptr<b2World> p_world = nullptr;
    scope<ContactListener>   p_cl    = nullptr;
};

struct Physics2D::RigidBody::RigidBodyData
{
    b2Body*    p_body;
    b2Fixture* p_fixture;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Physics2D::Physics2D() : mp_worldData(new Physics2D::WorldData())
{
    NB_PROFILE_DETAIL();

    mp_worldData->p_world = std::make_shared<b2World>(b2Vec2(0.0f, -9.81f));

    mp_worldData->p_cl = genScope<ContactListener>(this);

    mp_worldData->p_world->SetContactListener(mp_worldData->p_cl.get());
}
Physics2D::~Physics2D()
{
    NB_PROFILE_DETAIL();

    delete mp_worldData;
}

void Physics2D::update(f32_t deltaTime)
{
    NB_PROFILE();

    mp_worldData->p_world->Step(deltaTime, k_solverVelocityIterations, k_solverPositionIterations);
}

ref<Physics2D::RigidBody> Physics2D::addRigidBody(const RigidBodySpec& spec)
{
    NB_PROFILE_DETAIL();

    ref<Physics2D::RigidBody> p_rbody = ref<Physics2D::RigidBody>::gen();

    b2BodyDef bodyDef;

    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(&p_rbody);

    bodyDef.position        = b2Vec2(spec.position.x, spec.position.y);
    bodyDef.angle           = spec.angle;
    bodyDef.linearVelocity  = b2Vec2(spec.linearVelocity.x, spec.linearVelocity.y);
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
    p_rbody->inWorld        = true;

    return p_rbody;
}

void Physics2D::removeRigidBody(ref<Physics2D::RigidBody>& p_body)
{
    NB_PROFILE_DETAIL();

    NB_CORE_ASSERT(p_body->inWorld, "Not in world");

    mp_worldData->p_world->DestroyBody(p_body->p_data->p_body);

    p_body->inWorld = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RigidBody Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Physics2D::RigidBody::RigidBody() : p_data(new RigidBodyData())
{
    NB_PROFILE_DETAIL();
}

Physics2D::RigidBody::~RigidBody()
{
    NB_PROFILE_DETAIL();

    delete p_data;
}

void Physics2D::RigidBody::addFixture(const FixtureSpec& fixtureSpec, const util::Transform& transform)
{
    NB_PROFILE_DETAIL();

    b2FixtureDef fixtureDef;

    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    ///////////////////////////
    // Set Shape specifics
    ///////////////////////////
    NB_CORE_ASSERT(fixtureSpec.shape, "Fixture Shape cannot be null!");

    if (fixtureSpec.shape->type == ShapeType::RECTANGLE)
    {
        const Rectangle* rect = static_cast<const Rectangle*>(fixtureSpec.shape);

        b2PolygonShape* shape = new b2PolygonShape();
        shape->SetAsBox(rect->size.x * transform.getScale().x,
                        rect->size.y * transform.getScale().y,
                        b2Vec2(rect->offset.x, rect->offset.y),
                        0.0f);

        fixtureDef.shape = shape;
    }
    else if (fixtureSpec.shape->type == ShapeType::CIRCLE)
    {
        const Circle*  circle = static_cast<const Circle*>(fixtureSpec.shape);
        b2CircleShape* shape  = new b2CircleShape();
        shape->m_p.Set(circle->offset.x, circle->offset.y);
        shape->m_radius  = transform.getScale().x * circle->radius;
        fixtureDef.shape = shape;
    }

    ///////////////////////////
    // General attributes
    ///////////////////////////
    fixtureDef.friction             = fixtureSpec.friction;
    fixtureDef.restitution          = fixtureSpec.restitution;
    fixtureDef.restitutionThreshold = fixtureSpec.restitutionThreshold;
    fixtureDef.density              = fixtureSpec.density;
    fixtureDef.isSensor             = fixtureSpec.isSensor;

    p_data->p_body->CreateFixture(&fixtureDef);
    delete fixtureDef.shape;
}

util::Transform& Physics2D::RigidBody::getTransform()
{
    NB_PROFILE_TRACE();

    NB_CORE_ASSERT(inWorld, "Not in world");

    const auto& position = p_data->p_body->GetPosition();
    transform.setTranslationX(position.x);
    transform.setTranslationY(position.y);
    transform.setRotationZ(p_data->p_body->GetAngle());
    return transform;
}

glm::vec2 Physics2D::RigidBody::getVelocity()
{
    NB_PROFILE_TRACE();

    NB_CORE_ASSERT(inWorld, "Not in world");

    auto velocity = p_data->p_body->GetLinearVelocity();
    return {velocity.x, velocity.y};
}

void Physics2D::RigidBody::forceTransform()
{
    NB_PROFILE_TRACE();

    NB_CORE_ASSERT(inWorld, "Not in world");

    p_data->p_body->SetTransform(b2Vec2(transform.getTranslation().x, transform.getTranslation().y),
                                 transform.getRotation().z);
}

void Physics2D::RigidBody::forceVelocity(const glm::vec2& velocity)
{
    NB_PROFILE_TRACE();

    NB_CORE_ASSERT(inWorld, "Not in world");

    p_data->p_body->SetLinearVelocity({velocity.x, velocity.y});
}

void Physics2D::RigidBody::impulse(const glm::vec2& impulse)
{
    NB_PROFILE_TRACE();

    NB_CORE_ASSERT(inWorld, "Not in world");

    p_data->p_body->ApplyLinearImpulseToCenter({impulse.x, impulse.y}, true);
}

void Physics2D::RigidBody::halt()
{
    NB_PROFILE_TRACE();

    NB_CORE_ASSERT(inWorld, "Not in world");

    if (p_data->p_body->GetType() != b2_staticBody)
    {
        p_data->p_body->SetLinearVelocity({0.0f, 0.0f});
        p_data->p_body->SetAwake(false);
    }
}

void Physics2D::RigidBody::removeFromWorld()
{
    NB_PROFILE_DETAIL();

    NB_CORE_ASSERT(inWorld, "Not in world");

    b2World* p_world = p_data->p_body->GetWorld();

    p_world->DestroyBody(p_data->p_body);

    inWorld = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
u32_t Physics2D::_bodyType(BodyType bodyType) const
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
            NB_CORE_ASSERT(false, "Unkown body type %i", bodyType);
            return b2_staticBody;
        }
    }
}

}  // namespace nimbus