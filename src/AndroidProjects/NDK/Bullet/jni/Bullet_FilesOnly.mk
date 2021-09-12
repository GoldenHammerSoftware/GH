# This is only the files part of bullet Android.mk.
# It is used for including this project in another mk for gradle without building a library.

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletCollision/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletDynamics/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletDynamics/Character/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletDynamics/Dynamics/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletDynamics/Vehicle/

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/BulletSoftBody/

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/LinearMath/

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/vectormath/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/vectormath/scalar/

LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btDbvt.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp


LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btGhostObject.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/btUnionFind.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp


LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btBox2dShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btBoxShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btCollisionShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btCompoundShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConcaveShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConeShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConvexShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btCylinderShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btEmptyShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btMiniSDF.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btSdfCollisionShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btShapeHull.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btSphereShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp


LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/btContactProcessing.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/btGImpactBvh.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/btGImpactShape.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/gim_box_set.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/gim_contact.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/gim_memory.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/Gimpact/gim_tri_collision.cpp


LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp


LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Character/btKinematicCharacterController.cpp

LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp


LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Dynamics/btRigidBody.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp

LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletDynamics/Vehicle/btWheelInfo.cpp

LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btDefaultSoftBodySolver.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btSoftBody.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btSoftBodyHelpers.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp


LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/LinearMath/btAlignedAllocator.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/LinearMath/btConvexHull.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/LinearMath/btConvexHullComputer.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/LinearMath/btGeometryUtil.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/LinearMath/btQuickprof.cpp
LOCAL_SRC_FILES += ../../../../../../Contrib/bullet3-master/src/LinearMath/btSerializer.cpp
