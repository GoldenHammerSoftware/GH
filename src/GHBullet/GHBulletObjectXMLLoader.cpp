// Copyright Golden Hammer Software
#include "GHBulletObjectXMLLoader.h"
#include "GHXMLNode.h"
#include "GHBulletPhysicsSim.h"
#include "GHPhysicsShapeType.h"
#include "GHBulletBtInclude.h"
#include "GHMath/GHFloat.h"
#include "GHBulletPhysicsObject.h"
#include "GHModel.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHGeometryRenderable.h"
#include "GHGeometry.h"
#include "GHVertexBuffer.h"
#include "GHIndexBufferUtil.h"
#include "GHVertexStream.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHHeightFieldProperties.h"
#include "GHHeightField.h"
#include "GHUtils/GHProfiler.h"
#include "GHString/GHStringIdFactory.h"
#include "GHXMLObjFactory.h"
#include "GHBulletRigidBodyCopier.h"

GHBulletObjectXMLLoader::GHBulletObjectXMLLoader(GHBulletPhysicsSim& physicsSim,
	const GHIdentifierMap<int>& enumStore,
	GHStringIdFactory& hashTable,
	const GHXMLObjFactory& xmlObjFactory)
	: mPhysicsSim(physicsSim)
	, mEnumStore(enumStore)
	, mStringHashTable(hashTable)
	, mXMLObjFactory(xmlObjFactory)
{

}

void* GHBulletObjectXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHBulletPhysicsObject* ret = new GHBulletPhysicsObject(mPhysicsSim);
	populate(ret, node, extraData);
	return ret;
}

void GHBulletObjectXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHPROFILESCOPE("GHBulletObjectXMLLoader::populate", GHString::CHT_REFERENCE)

		GHBulletPhysicsObject* ret = reinterpret_cast<GHBulletPhysicsObject*>(obj);

	GHProperty prevPhysicsPropOnExtraData = extraData.getProperty(GHEntityHashes::P_PHYSICSOBJECT);
	extraData.setProperty(GHEntityHashes::P_PHYSICSOBJECT, ret);

	GHPoint3 offset(0, 0, 0);
	node.readAttrFloatArr("offset", offset.getArr(), 3);
	GHPoint3 rotation(0, 0, 0);
	node.readAttrFloatArr("rotation", rotation.getArr(), 3);

	GHRefCountedType<btCollisionShape>* finalShape = ret->getCollisionShape();

	btCollisionShape* shape = 0;
	const std::vector<GHXMLNode*>& shapeNodes = node.getChildren();
	GHRect<float, 3> bounds;
	if (shapeNodes.size() == 0 && !finalShape)
	{
		shape = new btBoxShape(btVector3(.5, .5, .5));
	}
	else if (shapeNodes.size() == 1)
	{
		shape = createCollisionShape(*shapeNodes[0], extraData, offset);
	}
	else if (shapeNodes.size() > 0)
	{
		shape = createMultiShape(shapeNodes, extraData, offset);
	}

	bool recalculateTensor = false;
	if (shape)
	{
		ret->setCollisionShape(new GHRefCountedType<btCollisionShape>(shape));
		finalShape = ret->getCollisionShape();
		recalculateTensor = true;

		GHTransform offsetTrans;
		if (rotation.lenSqr() == 0) {
			offsetTrans.becomeIdentity();
		}
		else {
			offsetTrans.becomeYawPitchRollRotation(rotation[0], rotation[1], rotation[2]);
		}
		offsetTrans.setTranslate(offset);

		//If "offsetTransform" exists, it overrides previous offset specifications.
		// This is open for being changed if it is burdensome.
		node.readAttrFloatArr("offsetTransform", offsetTrans.getMatrix().getArr(), 16);

		ret->setOffset(offsetTrans);
	}

	//kinematic bodies are also not dynamic (I think??)
	bool isKinematic = false;
	node.readAttrBool("kinematic", isKinematic);
	ret->setIsKinematic(isKinematic);

	btRigidBody* rigidBody = ret->getBulletObject();
	fillBulletObject(rigidBody, finalShape->get(), node, extraData, isKinematic, recalculateTensor);

	if (rigidBody != ret->getBulletObject())
	{
		ret->setBulletObject(rigidBody);
	}

	int collisionLayer;
	if (node.readAttrEnum("collisionLayer", collisionLayer, mEnumStore))
	{
		ret->setCollisionLayer(collisionLayer);
	}

	const char* contactPointFrictionOverride = node.getAttribute("contactPointFrictionOverride");
	if (contactPointFrictionOverride)
	{ 
		GHBulletContactAddedFrictionOverride* frictionOverride = (GHBulletContactAddedFrictionOverride*)mXMLObjFactory.load(contactPointFrictionOverride, &extraData);
		if (frictionOverride)
		{
			ret->setContactAddedFrictionOverride(frictionOverride);
			ret->debugging = true;
		}
		else
		{
			GHDebugMessage::outputString("Error: unable to load contactPointFrictionOverride with file %s", contactPointFrictionOverride);
		}
	}

	//put the previous physics object property back on extraData for correct recursive loading
	extraData.setProperty(GHEntityHashes::P_PHYSICSOBJECT, prevPhysicsPropOnExtraData);
}

void GHBulletObjectXMLLoader::fillBulletObject(btRigidBody*& ret, btCollisionShape* shape, const GHXMLNode& node, GHPropertyContainer& extraData, bool isKinematic, bool recalculateTensor) const
{
    float mass = 1;
    bool newMass = node.readAttrFloat("mass", mass);
    
    bool isDynamic = !GHFloat::isZero(mass);
    bool newDynamicness = node.readAttrBool("dynamic", isDynamic);

	if (!isDynamic) {
		mass = 0;
	}
    
    recalculateTensor = recalculateTensor || newDynamicness || newMass;
    if (recalculateTensor && !newMass && ret)
    {
        if (GHFloat::isZero(ret->getInvMass())) {
            mass = 0;
        }
        else {
            mass = 1.f/ret->getInvMass();
        }
    }
    
    if (recalculateTensor && !newDynamicness)
    {
        isDynamic = !GHFloat::isZero(mass);
    }
    
    if (recalculateTensor)
    {
        btVector3 localInertia(0, 0, 0);
        if (isDynamic)
        {
            shape->calculateLocalInertia(mass, localInertia);
        }

        if (ret)
        {
            // Old body will be cleaned up when this is passed to the GHBulletPhysicsObject. Don't delete it here.
            //  but we do need to make sure that old values from the inherited object are kept, even if we wind up overriding some of them.
            ret = GHBulletRigidBodyCopier::cloneBody(shape, ret, mass, localInertia);
        }
        else
        {
            btMotionState* motionState = new btDefaultMotionState;
            btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, localInertia);
            ret = new btRigidBody(info);

            //Put default values that need to be set only on the 1st time through (so configured values will pass through the clone).
            ret->setCollisionFlags(ret->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        }
    }
    
    GHPoint3 angularFactor;
    if (node.readAttrFloatArr("angularFactor", angularFactor.getArr(), 3))
    {
        ret->setAngularFactor(btVector3(angularFactor[0], angularFactor[1], angularFactor[2]));
    }

	GHPoint3 linearFactor;
	if (node.readAttrFloatArr("linearFactor", linearFactor.getArr(), 3))
	{
		ret->setLinearFactor(btVector3(linearFactor[0], linearFactor[1], linearFactor[2]));
	}
    
    bool useSweptCollision = !GHFloat::isZero(ret->getCcdSweptSphereRadius());
    if (node.readAttrBool("swept", useSweptCollision))
    {
        if (useSweptCollision)
        {
            float radius;
            // btSphereShape does not override getBoundingSphere.
            //  btCollisionShape::getBoundingSphere calls getBoundingBox and then returns 
            //  the sphere bounding that box. This results in a huge overfit for btSphereShapes.
            // I don't know whether the resulting behavior is intended by Bullet or properly a bug,
            //  but using this overfit for swept collisions was causing problems for us
            //  (eg, a ball resting on the floor given a large velocity would react as if it 
            //   was penetrating the floor and bounce off it). Setting the swept sphere radius
            //     to the actual sphere's radius fixes this issue.
            // Note that this will only work if the top-level shape is a sphere shape (rather than, say a multi-shape)
            if (shape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
            {
                const btSphereShape* sphereShape = (const btSphereShape*)shape;
                radius = sphereShape->getRadius();
            }
            else
            {
                btVector3 center;
                shape->getBoundingSphere(center, radius);
            }

            
            ret->setCcdMotionThreshold(radius);
            ret->setCcdSweptSphereRadius(radius);
        }
        else
        {
            //todo: find real default values
            ret->setCcdMotionThreshold(0);
            ret->setCcdSweptSphereRadius(0);
        }
    }
    
    float friction = 0;
    if (node.readAttrFloat("friction", friction))
    {
        ret->setFriction(friction);
    }
    
    float restitution = 0;
    if (node.readAttrFloat("restitution", restitution))
    {
        ret->setRestitution(restitution);
    }

	float linearDamping = ret->getLinearDamping(), angularDamping = ret->getAngularDamping();
	node.readAttrFloat("linearDamping", linearDamping);
	node.readAttrFloat("angularDamping", angularDamping);
	if (linearDamping != 0 || angularDamping != 0)
	{
		ret->setDamping(linearDamping, angularDamping);
	}
    
	float linearSleepThreshold = ret->getLinearSleepingThreshold();
	float angularSleepThreshold = ret->getAngularSleepingThreshold();
	node.readAttrFloat("linearSleepThreshold", linearSleepThreshold);
	node.readAttrFloat("angularSleepThreshold", angularSleepThreshold);
	ret->setSleepingThresholds(linearSleepThreshold, angularSleepThreshold);

	if (isKinematic) {
		ret->setCollisionFlags(ret->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		ret->setActivationState(DISABLE_DEACTIVATION);
	}

	bool enableCollisionResponse = true;
	node.readAttrBool("enableCollisionResponse", enableCollisionResponse);
	if (!enableCollisionResponse) {
		ret->setCollisionFlags(ret->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}




    bool enableDebugDraw = false;
    if (node.readAttrBool("enableDebugDraw", enableDebugDraw))
    {
        if (enableDebugDraw) {
            ret->setCollisionFlags(ret->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        }
        else {
            ret->setCollisionFlags(ret->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        }
    }
    
}

void GHBulletObjectXMLLoader::handleMeshNotFound(btCollisionShape*& outShape, const GHXMLNode& node, GHPropertyContainer& extraData, const char* intendedType) const
{
    if (!outShape)
    {
        GHDebugMessage::outputString("Error: %s specified for the physics shape type, but there is no model on the entity. Using PS_BOX instead", intendedType);
        outShape = createBox(node, extraData);
    }
}

btCollisionShape* GHBulletObjectXMLLoader::createCollisionShape(const GHXMLNode& node, GHPropertyContainer& extraData, GHPoint3& outOffset) const
{
    const int* typePtr = mEnumStore.find(node.getName());
    if (!typePtr)
    {
        const char* typeStr = node.getAttribute("type");
        if (typeStr) {
            typePtr = mEnumStore.find(typeStr);
        }
    }
    
    GHPhysicsShapeType::Enum type = typePtr ? (GHPhysicsShapeType::Enum)*typePtr : GHPhysicsShapeType::PS_BOX;
    
    //outOffset.setCoords(0,0,0);
    btCollisionShape* outShape = 0;
    switch (type) 
    {
        case GHPhysicsShapeType::PS_CYLINDER:
            outShape = createCylinder(node, extraData);
            break;
        case GHPhysicsShapeType::PS_SPHERE:
            outShape = createSphere(node, extraData);
            break;
        case GHPhysicsShapeType::PS_CAPSULE:
            outShape = createCapsule(node, extraData);
            break;
        case GHPhysicsShapeType::PS_PLANE:
            outShape = createPlane(node, extraData);
            break;
        case GHPhysicsShapeType::PS_HEIGHTFIELD:
            outShape = createHeightfield(node, extraData, outOffset);
            break;
        case GHPhysicsShapeType::PS_STATICMESH:
            outShape = createMesh(node, extraData, true);
            handleMeshNotFound(outShape, node, extraData, "PS_STATICMESH");
            break;
        case GHPhysicsShapeType::PS_MESH:
            outShape = createMesh(node, extraData, false);
            handleMeshNotFound(outShape, node, extraData, "PS_MESH");
            break;
        case GHPhysicsShapeType::PS_CONVEXHULL:
            outShape = createConvexHull(node, extraData);
            handleMeshNotFound(outShape, node, extraData, "PS_CONVEXHULL");
            break;
        case GHPhysicsShapeType::PS_BOX:
        default:
            outShape = createBox(node, extraData);
            break;
    }
    
    float margin = 0;
    if (node.readAttrFloat("margin", margin))
    {
        GHDebugMessage::outputString("replacing margin %f with %f", outShape->getMargin(), margin);
        outShape->setMargin(margin);
    }
    
    return outShape;
}

btCollisionShape* GHBulletObjectXMLLoader::createMultiShape(const std::vector<GHXMLNode*>& nodes, GHPropertyContainer& extraData, GHPoint3& outOffset) const
{
    btCompoundShape* ret = new btCompoundShape();
    
    size_t numChildren = nodes.size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        const GHXMLNode* childNode = nodes[i];
        btCollisionShape* childShape = createCollisionShape(*childNode, extraData, outOffset);
        addToMultiShape(ret, childShape, *childNode, extraData);
    }
    
    return ret;
}

void GHBulletObjectXMLLoader::addToMultiShape(btCompoundShape* multiShape, btCollisionShape* shape, const GHXMLNode& node, const GHPropertyContainer& extraData) const
{    
    float rot[3] = {0};
    node.readAttrFloatArr("rotation", rot, 3);
    btTransform transform;
    transform.getBasis().setEulerZYX(rot[0], rot[1], rot[2]);
    transform.getOrigin().setValue(0,0,0);
    node.readAttrFloatArr("translate", (btScalar*)transform.getOrigin(), 3);
    
    multiShape->addChildShape(transform, shape);
}

btCollisionShape* GHBulletObjectXMLLoader::createBox(const GHXMLNode& node, const GHPropertyContainer& extraData) const
{
    btVector3 halfExtents(1,1,1);
    node.readAttrFloatArr("extents", (btScalar*)halfExtents, 3);
    halfExtents *= .5f;
    return new btBoxShape(halfExtents);
}

btCollisionShape* GHBulletObjectXMLLoader::createSphere(const GHXMLNode& node, const GHPropertyContainer& extraData) const
{
    float radius = 1;
    node.readAttrFloat("radius", radius);
    return new btSphereShape(radius);
}

btCollisionShape* GHBulletObjectXMLLoader::createCylinder(const GHXMLNode& node, const GHPropertyContainer& extraData) const
{
    btVector3 halfExtents(1,1,1);
    node.readAttrFloatArr("extents", (btScalar*)halfExtents, 3);
    halfExtents *= .5f;
    return new btCylinderShape(halfExtents);
}

btCollisionShape* GHBulletObjectXMLLoader::createCapsule(const GHXMLNode& node, const GHPropertyContainer& extraData) const
{
    int axis = 1;
    node.readAttrInt("axis", axis);
    
    float radius = 1, height = 1;
    node.readAttrFloat("radius", radius);
    node.readAttrFloat("height", height);
    
    switch (axis)
    {
        case 0:
            return new btCapsuleShapeX(radius, height);
            break;
        case 2:
            return new btCapsuleShapeZ(radius, height);
            break;
        default:
            return new btCapsuleShape(radius, height);
            break;
    }
}

btCollisionShape* GHBulletObjectXMLLoader::createPlane(const GHXMLNode& node, const GHPropertyContainer& extraData) const
{
    btVector3 planeNormal(0, 1, 0);
    node.readAttrFloatArr("normal", (btScalar*)planeNormal, 3);
    
    float distFromOrigin = 0;
    node.readAttrFloat("distFromOrigin", distFromOrigin);
    
    return new btStaticPlaneShape(planeNormal, distFromOrigin);
}


const GHVertexStream* GHBulletObjectXMLLoader::getPositionStream(const GHVertexBuffer*vb, int& outPositionOffset, int& outVertexStride) const
{
    const std::vector<GHVertexStreamPtr*>& streams = vb->getStreams();
    size_t numStreams = streams.size();
    for (size_t s = 0; s < numStreams; ++s)
    {
        const GHVertexStream* vertexStream = streams[s]->get();
        const GHVertexStreamFormat& format = vertexStream->getFormat();
        size_t numComponents = format.getComponents().size();
        for (size_t c = 0; c < numComponents; ++c)
        {
            const GHVertexStreamFormat::ComponentEntry& componentEntry = format.getComponents()[c];
            if (componentEntry.mComponent == GHVertexComponentShaderID::SI_POS)
            {
                outPositionOffset = componentEntry.mOffset*sizeof(float);
                outVertexStride = format.getVertexSize()*sizeof(float);
                return vertexStream;
            }
        }
    }
    return 0;
}

bool GHBulletObjectXMLLoader::fillTriangles(TriangleFiller& ret, const GHModel* model, const GHIdentifier& geoId) const
{
	if (!model || !model->getRenderable()) {
		GHDebugMessage::outputString("Failed to find renderable for GHBulletObjectXMLLoader::fillTriangles");
		return false;
	}
    
    GHTransform rootTransInv = model->getSkeleton()->getLocalTransform();
    rootTransInv.invert();

	std::vector<GHGeometry*> geometryList;
	for (size_t i = 0; i < model->getRenderable()->getGeometry().size(); ++i)
	{
		if (geoId.isNull() || model->getRenderable()->getGeometry()[i]->getId() == geoId)
		{
			geometryList.push_back(model->getRenderable()->getGeometry()[i]);
		}
	}
    size_t numGeometries = geometryList.size();
    
    // first count up totals for preallocate
    int numIndicies = 0;
    int numVertices = 0;
    for (size_t g = 0; g < numGeometries; ++g)
    {
        GHGeometry* geo = geometryList[g];
        const GHVertexBuffer* vb = geo->getVB()->get();
        const GHVBBlitter* blitter = vb->getBlitter()->get();
        if (blitter->getType() == GHVBBlitter::BT_INDEX)
        {
            const GHVBBlitterIndex* indexBufferPtr = (const GHVBBlitterIndex*)blitter;
            numIndicies += indexBufferPtr->getNumIndices();
        }
        if (vb->getStreams().size())
        {
            GHVertexStream* vStream = vb->getStreams()[0]->get();
            numVertices += vStream->getNumVerts();
        }
    }
	if (!numIndicies || !numVertices) {
		return false;
	}
    ret.preallocate(numVertices, numIndicies);
    
    // then populate the actual mesh
    for (size_t g = 0; g < numGeometries; ++g)
    {
        GHGeometry* geo = geometryList[g];
        GHTransform geoTrans;
        rootTransInv.mult(geo->getTransform()->getTransform(), geoTrans);
        const GHVertexBuffer* vb = geo->getVB()->get();
        
        int positionOffset = 0;
        int vertexStride = 0;
        const GHVertexStream* positionStream = getPositionStream(vb, positionOffset, vertexStride);
        
        if (positionStream != 0)
        {
            const GHVBBlitter* blitter = vb->getBlitter()->get();
            if (blitter->getType() == GHVBBlitter::BT_INDEX)
            {
                const GHVBBlitterIndex* indexBufferPtr = (const GHVBBlitterIndex*)blitter;
                
                int numTriangles = indexBufferPtr->getNumIndices()/3;
                
                const unsigned short* indexBuffer = indexBufferPtr->lockReadBuffer();
                const float* vertexBuffer = positionStream->lockReadBuffer();
                
                const char* vbArr = (const char*)vertexBuffer;
                vbArr += positionOffset;
                
                for (int i = 0; i < numTriangles; ++i)
                {
                    const float* vPtr1 = (const float*)&vbArr[indexBuffer[i*3]*vertexStride];
                    const float* vPtr2 = (const float*)&vbArr[indexBuffer[i*3+1]*vertexStride];
                    const float* vPtr3 = (const float*)&vbArr[indexBuffer[i*3+2]*vertexStride];
                    
                    GHPoint3 pt1, pt2, pt3;
                    pt1.setCoords(vPtr1);
                    pt2.setCoords(vPtr2);
                    pt3.setCoords(vPtr3);
                    
                    geoTrans.mult(pt1, pt1);
                    geoTrans.mult(pt2, pt2);
                    geoTrans.mult(pt3, pt3);
                    
                    ret.addTriangle(pt1.getArr(), pt2.getArr(), pt3.getArr());
                }
                
                positionStream->unlockReadBuffer();
                indexBufferPtr->unlockReadBuffer();
            }
        }        
    }
	return true;
}

btCollisionShape* GHBulletObjectXMLLoader::createConvexHull(const GHXMLNode& node, const GHPropertyContainer& extraData) const
{
    GHModel* model = extraData.getProperty(GHEntityHashes::MODEL);
	if (!model || !model->getRenderable()) {
        return 0;
    }

    btConvexHullShape rawShape;
    class HullFiller : public TriangleFiller
    {
    public:
        HullFiller(btConvexHullShape* hull) : mHull(hull) {}
        virtual void preallocate(int numVerts, int numIndicies) {}
        virtual void addTriangle(const float* vPtr1, const float* vPtr2, const float* vPtr3)
        {  
            mHull->addPoint(btVector3(*vPtr1, *(vPtr1+1), *(vPtr1+2)));
            mHull->addPoint(btVector3(*vPtr2, *(vPtr2+1), *(vPtr2+2)));
            mHull->addPoint(btVector3(*vPtr3, *(vPtr3+1), *(vPtr3+2)));
        }
    private:
        btConvexHullShape* mHull;
    };
    
    HullFiller hullFiller(&rawShape);
    fillTriangles(hullFiller, model, 0);
    
    btShapeHull hull(&rawShape);
    btScalar margin = rawShape.getMargin();
    hull.buildHull(margin);

    const btScalar* points = (const btScalar*)hull.getVertexPointer();
    btConvexHullShape* ret = new btConvexHullShape(points, hull.numVertices());
    
    return ret;
}

btCollisionShape* GHBulletObjectXMLLoader::createMesh(const GHXMLNode& node, const GHPropertyContainer& extraData, bool useStaticMeshType) const
{
    GHPROFILESCOPE("GHBulletObjectXMLLoader::createMesh", GHString::CHT_REFERENCE)
    GHModel* model = extraData.getProperty(GHEntityHashes::MODEL);
	if (!model || !model->getRenderable()) {
        return 0;
    }
	GHIdentifier geoId = 0;
	node.readAttrIdentifier("geoName", geoId, mStringHashTable);

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    
    class MeshFiller : public TriangleFiller
    {
    public:
        MeshFiller(btTriangleMesh* mesh) : mMesh(mesh) {}
        virtual void preallocate(int numVerts, int numIndicies)
        {
            mMesh->preallocateIndices(numIndicies);
            mMesh->preallocateVertices(numVerts);
        }
        virtual void addTriangle(const float* vPtr1, const float* vPtr2, const float* vPtr3)
        {
            mMesh->addTriangle(btVector3(*vPtr1, *(vPtr1+1), *(vPtr1+2)),
                             btVector3(*vPtr2, *(vPtr2+1), *(vPtr2+2)),
                             btVector3(*vPtr3, *(vPtr3+1), *(vPtr3+2)));
        }
    private:
        btTriangleMesh* mMesh;
    };
    
    MeshFiller meshFiller(triangleMesh);
	if (!fillTriangles(meshFiller, model, geoId)) {
		return 0;
	}
    
    btCollisionShape* ret = 0;
    if (useStaticMeshType) {
        btBvhTriangleMeshShape* meshShape = new btBvhTriangleMeshShape(triangleMesh, true);
        meshShape->setLocalScaling(btVector3(1.f,1.f,1.f));
        ret = meshShape;
    }
    else {
        btGImpactMeshShape* meshShape = new btGImpactMeshShape(triangleMesh);
		meshShape->setLocalScaling(btVector3(1.f,1.f,1.f));
        meshShape->updateBound();
        ret = meshShape;
    }
	float margin = ret->getMargin();
	node.readAttrFloat("margin", margin);
	ret->setMargin(margin);

    return ret;
}

btCollisionShape* GHBulletObjectXMLLoader::createHeightfield(const GHXMLNode& node, const GHPropertyContainer& extraData, GHPoint3& outOffset) const
{
    GHHeightField* hf = extraData.getProperty(GHHeightFieldProperties::HEIGHTFIELD);
    if (!hf)
    {
        GHDebugMessage::outputString("Bullet Object Loader Error: Trying to load a PS_HEIGHTFIELD with no heightfield data.");
        return 0;
    }
    
    /*
    btHeightfieldTerrainShape(int heightStickWidth,int heightStickLength,
	                          void* heightfieldData, btScalar heightScale,
	                          btScalar minHeight, btScalar maxHeight,
	                          int upAxis, PHY_ScalarType heightDataType,
	                          bool flipQuadEdges);
     */
    
    unsigned int nodeWidth, nodeHeight;
    hf->getDimensions(nodeWidth, nodeHeight);
    btHeightfieldTerrainShape* ret;
    ret = new btHeightfieldTerrainShape(nodeWidth, nodeHeight,
                                        hf->getHeights(), 1.f,
                                        -2000.f, 2000.f,
                                        1, PHY_FLOAT,
                                        false);
     
    ret->setLocalScaling(btVector3(hf->getDistBetweenNodes(), 1.f, hf->getDistBetweenNodes()));
    
    outOffset[0] = ((float)(nodeWidth-1))*hf->getDistBetweenNodes()*.5f;
    outOffset[1] = 0;
    outOffset[2] = ((float)(nodeHeight-1))*hf->getDistBetweenNodes()*.5f;
    
    return ret;
}
