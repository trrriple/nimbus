#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/physics/physics2D.hpp"
#include "box2d/box2d.h"

namespace nimbus
{

struct Physics2D::WorldData
{
    scope<b2World>      world = nullptr;
};

Physics2D::Physics2D() : m_worldData(new Physics2D::WorldData())
{
    m_worldData->world = makeScope<b2World>(b2Vec2(0.0f, -9.81f));
}
Physics2D::~Physics2D()
{
    delete m_worldData;
}

void Physics2D::update(float deltaTime)
{
    m_worldData->world->Step(
        deltaTime, k_solverVelocityIterations, k_solverPositionIterations);
}

Physics2D::RigidBody Physics2D::addRigidBody(const RigidBodySpec& spec)
{
    b2BodyDef bodyDef;

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

    b2Body* p_body = m_worldData->world->CreateBody(&bodyDef);

    Physics2D::RigidBody body;

    body.p_data = p_body;

    return body;
}

void Physics2D::RigidBody::addFixture(const FixtureSpec&     fixtureSpec,
                                      const util::Transform& transform)
{
    b2FixtureDef fixtureDef;

    ///////////////////////////
    // Set Shape specifics
    ///////////////////////////
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
    }

    ///////////////////////////
    // General attributes
    ///////////////////////////
    fixtureDef.friction             = fixtureSpec.friction;
    fixtureDef.restitution          = fixtureSpec.restitution;
    fixtureDef.restitutionThreshold = fixtureSpec.restitutionThreshold;
    fixtureDef.density              = fixtureSpec.density;
    fixtureDef.isSensor             = fixtureSpec.isSensor;

    b2Body* p_body = static_cast<b2Body*>(p_data);

    p_body->CreateFixture(&fixtureDef);
}

util::Transform& Physics2D::RigidBody::updateTransform()
{
    b2Body* p_body = static_cast<b2Body*>(p_data);

    const auto& position    = p_body->GetPosition();
    transform.translation.x = position.x;
    transform.translation.y = position.y;
    transform.rotation.z    = p_body->GetAngle();

    return transform;
}

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