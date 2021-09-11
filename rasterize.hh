#include <type_traits>
#include <utility>
#include <tuple>
#include <algorithm>

template<typename P>
void RasterizeTriangle
    (const P* p0, const P* p1, const P* p2, // The 3 corner vertices of type P
     auto&& GetXY, // A functor that retrieves integer x and y coordinates from a ref to P.
     auto&& MakeSlope,    // A functor that creates a slope between two points, with a number of iterations.
     auto&& DrawScanline) // A functor that plots N pixels using the slopes on left and right side.
{
  // Do the rasterization from top to bottom.
  auto [x0,y0, x1,y1, x2,y2] = std::tuple_cat(GetXY(*p0), GetXY(*p1), GetXY(*p2));

  // Sort the points in order of Y coordinate, so first point is the top one.
  // In case of equal Y coordinates, sort according to X coordinates.
  // Using a 3-input sorting network.
  if(std::tie(y1,x1) < std::tie(y0,x0)) { std::swap(x0,x1); std::swap(y0,y1); std::swap(p0,p1); }
  if(std::tie(y2,x2) < std::tie(y0,x0)) { std::swap(x0,x2); std::swap(y0,y2); std::swap(p0,p2); }
  if(std::tie(y2,x2) < std::tie(y1,x1)) { std::swap(x1,x2); std::swap(y1,y2); std::swap(p1,p2); }

  // Refuse to draw area-less triangles.
  if(int(y0) == int(y2)) return;

  // Determine if the short side is on the right or the left.
  bool shortside = (y1 - y0) * (x2 - x0) < (x1 - x0) * (y2 - y0); // false=left side, true=right side

  // Create 2 slopes: p0 - p1 (short) and p0 - p2 (long).
  // One of these is on the left side, the other is on the right.
  std::invoke_result_t<decltype(MakeSlope), P*,P*,int> sides[2];
  sides[!shortside] = MakeSlope(p0, p2, y2 - y0); //Slope for long side

  // The main rasterization loop.
  if(int(y0) < int(y1))
  {
      // Calculate the first slope for short side. The number of lines cannot be zero.
      sides[shortside] = MakeSlope(p0,p1, y1-y0);
      for(int y = y0; y < int(y1); ++y)
      {
          // On a single scanline, we go from the left X coordinate to the right X coordinate.
          DrawScanline(y, sides[0], sides[1]);
      }
  }
  if(int(y1) < int(y2))
  {
      // Calculate the second slope for short side. The number of lines cannot be zero.
      sides[shortside] = MakeSlope(p1,p2, y2-y1);
      for(int y = y1; y < int(y2); ++y)
      {
          // On a single scanline, we go from the left X coordinate to the right X coordinate.
          DrawScanline(y, sides[0], sides[1]);
      }
  }
}