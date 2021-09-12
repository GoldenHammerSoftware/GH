// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"
#include <vector>
#include "GHMath/GHPoint.h"

class GHBulletPhysicsSim;
class btRigidBody;
class btCollisionShape;
class btCompoundShape;

class GHVertexBuffer;
class GHVertexStream;
class btTriangleIndexVertexArray;
class GHModel;
class btConvexHullShape;
class GHXMLObjFactory;

class GHBulletObjectXMLLoader : public GHXMLObjLoader
{
public:
    GHBulletObjectXMLLoader(GHBulletPhysicsSim& physicsSim,
                            const GHIdentifierMap<int>& enumStore,
							GHStringIdFactory& hashTable,
							const GHXMLObjFactory& xmlObjFactory);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    void fillBulletObject(btRigidBody*& ret, btCollisionShape* shape, const GHXMLNode& node, GHPropertyContainer& extraData, bool isKinematic, bool recalculateTensor) const;
    btCollisionShape* createCollisionShape(const GHXMLNode& node, GHPropertyContainer& extraData, GHPoint3& outOffset) const;
    
    btCollisionShape* createMultiShape(const std::vector<GHXMLNode*>& nodes, GHPropertyContainer& extraData, GHPoint3& outOffset) const;
    void addToMultiShape(btCompoundShape* multiShape, btCollisionShape* shape, const GHXMLNode& node, const GHPropertyContainer& extraData) const;
    btCollisionShape* createBox(const GHXMLNode& node, const GHPropertyContainer& extraData) const;
    btCollisionShape* createSphere(const GHXMLNode& node, const GHPropertyContainer& extraData) const;
    btCollisionShape* createCylinder(const GHXMLNode& node, const GHPropertyContainer& extraData) const;
    btCollisionShape* createPlane(const GHXMLNode& node, const GHPropertyContainer& extraData) const;
    btCollisionShape* createConvexHull(const GHXMLNode& node, const GHPropertyContainer& extraData) const;
    btCollisionShape* createMesh(const GHXMLNode& node, const GHPropertyContainer& extraData, bool useStaticMeshType) const;
    btCollisionShape* createCapsule(const GHXMLNode& node, const GHPropertyContainer& extraData) const;
    btCollisionShape* createHeightfield(const GHXMLNode& node, const GHPropertyContainer& extraData, GHPoint3& outOffset) const;
    
    void handleMeshNotFound(btCollisionShape*& outShape, const GHXMLNode& node, GHPropertyContainer& extraData, const char* intendedType) const;
    
private: //refactored portions of createMesh
    class TriangleFiller
    {
    public:
        virtual ~TriangleFiller(void) { }
        virtual void preallocate(int numVerts, int numIndicies) = 0;
        virtual void addTriangle(const float* point1, const float* point2, const float* point3) = 0;
    };
	// returns false if no triangles
    bool fillTriangles(TriangleFiller& ret, const GHModel* model, const GHIdentifier& geoId) const;
    const GHVertexStream* getPositionStream(const GHVertexBuffer*vb, int& outPositionOffset, int& outVertexStride) const;
    
protected:
    GHBulletPhysicsSim& mPhysicsSim;
    const GHIdentifierMap<int>& mEnumStore;
	GHStringIdFactory& mStringHashTable;
	const GHXMLObjFactory& mXMLObjFactory;
};
