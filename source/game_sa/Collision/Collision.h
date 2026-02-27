/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ColModel.h"
#include "ColPoint.h"
#include "StoredCollPoly.h"
#include "CompressedVector.h"

class  CEntity;
class  CVehicle;
struct CColCacheEntry;
class  CColSphere;
class  CColBox;

class CCollision {
public:
    static inline CLinkList<CCollisionData*> &ms_colModelCache = *(CLinkList<CCollisionData*>*)0x96592C;
    static inline uint32& ms_iProcessLineNumCrossings = *(uint32*)0x9655D0;
    static inline uint32& ms_collisionInMemory = *(uint32*)0x9655D4;

    static inline bool& bCamCollideWithVehicles = *(bool*)0x8A5B14;         // = true
    static inline bool& bCamCollideWithObjects = *(bool*)0x8A5B15;          // = true
    static inline bool& bCamCollideWithPeds = *(bool*)0x8A5B17;             // = true
    static inline bool& bCamCollideWithBuildings = *(bool*)0x8A5B16;        // = true
    static inline float& relVelCamCollisionVehiclesSqr = *(float*)0x8A5B18; // = 0.01f

    static inline struct DebugSettings {
        struct ShapeShapeCollision {
            enum Shape { // I don't like casting, sorry
                SBOX,
                SSPHERE,
                STRI,
                SLINE,
                SNUM
            };

            enum State : uint8 {
                DISABLED,
                ENABLED,
                NOT_SUPPORTED
            };

            // Entry permutations are linked in state (eg.: BOX-TRI and TRI-BOX always are in the same state)
            // So it's enough to check States[SBOX][STRI], no need to check States[STRI][SBOX]
            State States[SNUM][SNUM]{
                // Box           Sphere   Triangle       Line
                { NOT_SUPPORTED, ENABLED, NOT_SUPPORTED, ENABLED       }, // Box
                { ENABLED,       ENABLED, ENABLED,       ENABLED       }, // Sphere
                { NOT_SUPPORTED, ENABLED, NOT_SUPPORTED, ENABLED       }, // Triangle
                { ENABLED,       ENABLED, ENABLED,       NOT_SUPPORTED }, // Line
            };

            bool IsEnabled(Shape a, Shape b) { return States[a][b] == ENABLED; }
        } ShapeShapeCollision{};

        // OG code considered lines that begin and end within the sphere as not intersecting it
        bool AllowLineOriginInsideSphere{false};
    } s_DebugSettings{};

public:
    static void InjectHooks();
    static void Tests(int32 i);

    // dummy function
    static void Init();
    static void Shutdown();
    static void Update();
    static void SortOutCollisionAfterLoad();
    static bool TestSphereSphere(CColSphere const& sphere1, CColSphere const& sphere2);
    static bool TestSphereBox(CSphere const& sphere, CBox const& box);
    static bool ProcessSphereBox(CColSphere const & sph, CColBox const& box, CColPoint & point, float & mindistsq);
    static bool __stdcall PointInTriangle(CVector const& point, CVector const* triPoints);
    static float DistToLineSqr(CVector const& lineStart, CVector const& lineEnd, CVector const& point);
    static float DistToMathematicalLine(CVector const* lineStart, CVector const* lineEnd, CVector const* point);
    static float DistToMathematicalLine2D(float lineStartX, float lineStartY, float lineEndX, float lineEndY, float pointX, float pointY);
    static float DistAlongLine2D(float lineX, float lineY, float lineDirX, float lineDirY, float pointX, float pointY);
    static bool ProcessLineSphere(const CColLine& line, const CColSphere& sphere, CColPoint& colPoint, float& depth);
    static bool TestLineBox_DW(const CColLine& line, const CBox& box);
    static bool TestLineBox(const CColLine& line, const CBox& box);
    static bool TestVerticalLineBox(const CColLine& line, const CBox& box);
    static bool ProcessLineBox(const CColLine& line, const CColBox& box, CColPoint& colPoint, float& maxTouchDistance);
    static bool Test2DLineAgainst2DLine(float line1StartX, float line1StartY, float line1EndX, float line1EndY, float line2StartX, float line2StartY, float line2EndX, float line2EndY);
    static bool ProcessDiscCollision(CColPoint& tempTriCol, const CMatrix& matBA, const CColDisk& disk, CColPoint& diskColPoint, bool& lineCollision, float& lineRatio, CColPoint& lineColPoint);
    static bool TestLineTriangle(const CColLine& line, const CompressedVector* verts, const CColTriangle& tri, const CColTrianglePlane& triPlane);
    static bool ProcessLineTriangle(const CColLine& line, const CompressedVector* verts, const CColTriangle& tri, const CColTrianglePlane& triPlane, CColPoint& colPoint, float& maxTouchDistance, CStoredCollPoly* collPoly);
    static bool ProcessVerticalLineTriangle(const CColLine& line, const CompressedVector* verts, const CColTriangle& tri, const CColTrianglePlane& triPlane, CColPoint& colPoint, float& maxTouchDistance, CStoredCollPoly* collPoly);
    static bool IsStoredPolyStillValidVerticalLine(const CVector& lineOrigin, float lineDist, CColPoint& colPoint, CStoredCollPoly* collPoly);
    static CColBox GetBoundingBoxFromTwoSpheres(const CColSphere& spA, const CColSphere& spB);
    static bool IsThisVehicleSittingOnMe(CVehicle* vehicle, CVehicle* vehicleOnMe);
    static bool CheckCameraCollisionPeds(int32 sectorX, int32 sectorY, const CVector& pos, const CVector& dir, float& /*unused*/);
    static bool CheckPeds(const CVector& src, const CVector& normal, float& nearest);
    static bool SphereCastVsBBox(const CColSphere& sphere1, const CColSphere& sphere2, const CColBox& box);
    static bool RayPolyPOP(CVector* arg0, CVector* arg1, CColTriangle* arg2, CVector* arg3, CVector* arg4);
    static int32 GetPrincipleAxis(const CVector& normal);
    static bool PointInPoly(
        const CVector& testPt,
        const CColTriangle& /*unused*/,
        const CVector& normal,
        const CVector* verts
    );
    static void Closest3(CVector* arg0, CVector* arg1);
    static bool  SphereCastVersusVsPoly(const CColSphere& sphere1, const CColSphere& sphere2, const CColTriangle& tri, const CColTrianglePlane& triPlane, CompressedVector* verts);
    static void CalculateTrianglePlanes(CCollisionData* colData);
    static void RemoveTrianglePlanes(CCollisionData* colData);
    static bool ProcessSphereSphere(const CColSphere& sphere1, const CColSphere& sphere2, CColPoint& colPoint, float& maxTouchDistance);
    static bool TestSphereTriangle(const CColSphere& sphere, const CompressedVector* verts, const CColTriangle& tri, const CColTrianglePlane& triPlane);
    static bool ProcessSphereTriangle(const CColSphere& sphere, const CompressedVector* verts, const CColTriangle& tri, const CColTrianglePlane& triPlane, CColPoint& colPoint, float& maxTouchDistance);
    static bool TestLineSphere(const CColLine& line, const CColSphere& sphere);
    static float DistToLine(const CVector& lineStart, const CVector& lineEnd, const CVector& point);
    static bool TestLineOfSight(const CColLine& line, const CMatrix& transform, CColModel& colModel, bool doSeeThroughCheck, bool doShootThroughCheck);
    static bool ProcessLineOfSight(const CColLine& line, const CMatrix& transform, CColModel& colModel, CColPoint& colPoint, float& maxTouchDistance, bool doSeeThroughCheck, bool doShootThroughCheck);
    static bool ProcessVerticalLine(const CColLine& line, const CMatrix& transform, CColModel& colModel, CColPoint& colPoint, float& maxTouchDistance, bool doSeeThroughCheck, bool doShootThroughCheck, CStoredCollPoly* collPoly);
    static bool SphereCastVsSphere(const CColSphere& spA, const CColSphere& spB, const CColSphere& spS);
    static void ClosestPointOnLine(const CVector& l0, const CVector& l1, const CVector& point, CVector& closest);
    static void ClosestPointsOnPoly(CColTriangle* arg0, CVector* arg1, CVector* arg2, CVector* arg3);
    static void ClosestPointOnPoly(CColTriangle* arg0, CVector* arg1, CVector* arg2);
    static bool SphereCastVsCaches(
        const CColSphere& spA,
        const CVector& velocity,

        int32 numTest,
        CColCacheEntry* pTest,

        int32& pNumWrite,
        CColCacheEntry* pWrite
    );
    static void CalculateTrianglePlanes(CColModel* colModel);
    static void RemoveTrianglePlanes(CColModel* colModel);
    // returns number of resulting collision points
    static int32 ProcessColModels(
        const CMatrix& transformA, CColModel& cmA,
        const CMatrix& transformB, CColModel& cmB,
        std::array<CColPoint, 32>& sphereCPs,
        CColPoint* lineCPs,
        float* maxTouchDistances,
        bool arg7
    );
    static bool SphereCastVsEntity(
        const CColSphere& spAws,
        const CColSphere& spBws,
        CEntity* entity
    );
    static bool SphereVsEntity(CColSphere* sphere, CEntity* entity);

    /*!
    * @param X      Sector X
    * @param Y      Sector Y
    * @param bbSpAB Bounding box of spA and spB
    * @param spS    Fuck knows
    * @param spA    Camera sphere 
    * @param spB    Camera sphere next frame (Offsetted by player velocity)
    * @addr 0x41A820
    */
    static bool CheckCameraCollisionBuildings(
        int32 X,
        int32 Y,
        const CColBox& bbSpAB,
        const CColSphere& spS,
        const CColSphere& spA,
        const CColSphere& spB
    );

    /*!
    * @param X          Repeat Sector X
    * @param Y          Repeat Sector Y
    * @param bbSpAB     Bounding box of spA and spB
    * @param spS        Fuck knows
    * @param spA        Camera sphere 
    * @param spB        Camera sphere next frame (Offsetted by player velocity)
    * @param plyrVehVel Player vehicle's velocity (Or null if player is not in a vehicle)
    * @addr 0x41A990
    */
    static bool CheckCameraCollisionVehicles(
        int32 X,
        int32 Y,
        const CColBox& bbSpAB,
        const CColSphere& spS,
        const CColSphere& spA,
        const CColSphere& spB,
        const CVector* plyrVehVel
    );

    /*!
    * @param X      Repeat Sector X
    * @param Y      Repeat Sector Y
    * @param bbSpAB Bounding box of spA and spB
    * @param spS    Fuck knows
    * @param spA    Camera sphere
    * @param spB    Camera sphere next frame (Offsetted by player velocity)
    * @addr 0x41AB20
    */
    static bool CheckCameraCollisionObjects(
        int32 X,
        int32 Y,
        const CColBox& pBox,
        const CColSphere& spS,
        const CColSphere& spA,
        const CColSphere& spB
    );
    static bool BuildCacheOfCameraCollision(
        const CColSphere& spA,
        const CColSphere& spB
    );
    static bool CameraConeCastVsWorldCollision(
        const CColSphere& spA,
        const CColSphere& spB,
        float& dist,
        float minDist
    );

    // NOTSA section
    static CVector GetClosestPtOnLine(const CVector& l0, const CVector& l1, const CVector& point);
    static CVector GetBaryCoordsOnTriangle(CVector a, CVector b, CVector c, CVector p);
    static CVector GetClampedBaryCoordsIntoTriangle(CVector a, CVector b, CVector c, CVector p);
    static CVector GetCoordsClampedIntoTriangle(CVector a, CVector b, CVector c, CVector p);
    static float   ClosestPtSegmentSegment(CVector p1, CVector d1, float a, CVector p2, CVector d2, float e, float& s, float& t, CVector& c1, CVector& c2);
};

void CalculateColPointInsideBox(const CBox& box, const CVector& point, CColPoint& colPoint);
bool ProcessDiscCollision(CColPoint& colPoint1, const CMatrix& mat, const CColDisk& disk, CColPoint& colPoint2, bool& arg4, float& arg5, CColPoint& colPoint3);
void ResetMadeInvisibleObjects();
float ClosestSquaredDistanceBetweenFiniteLines(const CVector& line1Start, const CVector& line1End, const CVector& line2Start, const CVector& line2End, float arg4);
