# loose_quadtree
Loose Quadtree (Region Tree) simple C++11 implementation

---------------------

Loose quadtree (unlike normal quadtrees which are for points only) is a region tree designed to store bounding boxes.
See boost::geometry::index::rtree for a more advanced, general solution!

This implementation features:
 * Fully adaptive behavior, adjusts its bounds, height and memory on demand
 * Every object will be stored on the level to which its size corresponds to
 * Gives theoretically optimal search results (see previous)
 * Uses tree structure instead of hashed (smaller memory footprint, cache friendly)
 * Uses as much data in-place as it can (by using its own allocator)
 * Allocates memory in big chunks
 * Uses axis-aligned bounding boxes for calculations
 * Uses left-top-width-height bounds for better precision (no right-bottom)
 * Uses left-top closed right-bottom open interval logic (for integral types)
 * Uses X-towards-right Y-towards-bottom screen-like coordinate system
 * It is suitable for both floating- and fixed-point logic
 * Generic parameters are:
   * NumberT generic number type allows its floating- and fixed-point usage
   * ObjectT* pointer is stored, no object copying, not an inclusive container
   * BoundingBoxExtractorT allows using your own bounding box type/source

LooseQuadtree was written by Zozó, use freely under MIT license
