#pragma once

// Central place for including bullet headers.
// Bullet physics has a lot of warnings and does not seem to want to fix them.
// This header allows us to put header guards around the includes to supress those warnings.
// GHBulletBtInclude does lead to pulling in more of bullet than needed in most places,
//  but this file should be internal to the GHBullet project (ie not included in public headers),
//  and it's a fair tradeoff for having the header guards centralized.

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4127)
	#pragma warning(disable:4100)
	#pragma warning(disable:4324)
	#pragma warning(disable:4305)
	#pragma warning(disable:6001)
	#pragma warning(disable:26495)
	#pragma warning(disable:26451)
#elif defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Weffc++"
	#pragma GCC diagnostic ignored "-Wsign-conversion"
	#pragma GCC diagnostic ignored "-Wswitch-default"
	#pragma GCC diagnostic ignored "-Wunused-parameter"
	#pragma GCC diagnostic ignored "-Winline"
	#pragma GCC diagnostic ignored "-Wuninitialized"
	#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7))
		#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	#endif
#endif

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btIDebugDraw.h"

#if defined(_MSC_VER)
	#pragma warning(pop)
#elif defined(__GNUC__)
	#pragma GCC diagnostic pop
#endif
