// Copyright Golden Hammer Software

#include "GHMath/GHRect.h"
#include "GHMath/GHPoint.h"
#include <stdint.h>
#include "GHVBBlitter.h"
#include <vector>

class GHVertexBuffer;
class GHVBFactory;
class GHSphereBounds;

class GHGeoCreationUtil
{
public:
    // create a buffer capable of holding a number of consecutive boxes.
    // this should create a buffer useable by createBox.
    // blitter is an optional shared blitter between box buffers of identical size.
    static GHVertexBuffer* createBoxBuffer(const GHVBFactory& vbFactory, unsigned int numBoxes,
                                           GHVBBlitterPtr* blitter,
                                           unsigned int& vertsPerBox, unsigned int& indexPerBox);
    
    static GHVertexBuffer* createBox(const GHVBFactory& vbFactory, const GHRect<float, 3>& lhsFace, const GHRect<float, 3>& rhsFace, const GHPoint3* color, GHVBBlitterPtr* blitter);
    static void setBoxVerts(GHVertexBuffer& vertexBuffer, const GHRect<float, 3>& lhsFace, const GHRect<float, 3>& rhsFace, const GHPoint3* color, unsigned int vertStart);
    // a version that takes 4 points per side.
    static void setBoxVerts(GHVertexBuffer& vertexBuffer, std::vector<GHPoint3>& lhsFace, std::vector<GHPoint3>& rhsFace, const GHPoint3* color, unsigned int vertStart);

    static GHVertexBuffer* createLine(const GHVBFactory& vbFactory, const GHPoint3& lhsPoint, const GHPoint3& rhsPoint, const GHPoint3* color, GHVBBlitterPtr* blitter);
    static void setLineVerts(GHVertexBuffer& vertexBuffer, const GHPoint3& lhsPoint, const GHPoint3& rhsPoint, const GHPoint3* color, unsigned int vertStart, float lineThickness);
    
    // take an index buffer of appropriate size and fill in indices for a box.
    static void fillBoxBlitter(GHVBBlitterIndex& indexBlitter, unsigned int vertStart, unsigned int indexStart);

	static GHVertexBuffer* createQuad(const GHVBFactory& vbFactory, const GHPoint<float, 3>& ul, const GHPoint<float, 3>& br, 
		GHVBBlitterPtr* blitter);

	static bool calcSphereBounds(const GHVertexBuffer& vertexBuffer, GHSphereBounds& bounds);

private:
    static float* fillVertexBufferPoint(float* buffer, const GHPoint3& point, unsigned short stride);
    static float* fillVertexBufferColor(float* buffer, const int32_t color, unsigned short stride);
    
    static void getBoxFacesForLine(const GHPoint3& lhsPoint, const GHPoint3& rhsPoint, GHRect<float, 3>& lhsFace, GHRect<float, 3>& rhsFace, float thickness = 1.0f);
};
