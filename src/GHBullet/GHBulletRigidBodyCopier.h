// Copyright Golden Hammer Software
#pragma once

class btCollisionShape;
class btRigidBody;
class btVector3;

class GHBulletRigidBodyCopier
{
public:
    static btRigidBody* cloneBody(btCollisionShape* collisionShape, const btRigidBody* src);
    static btRigidBody* cloneBody(btCollisionShape* shape, const btRigidBody* src, float mass, const btVector3& localInertia);
};
