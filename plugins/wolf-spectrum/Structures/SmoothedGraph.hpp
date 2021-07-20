#ifndef WOLF_SMOOTHED_GRAPH_DEFINED_H
#define WOLF_SMOOTHED_GRAPH_DEFINED_H

#include "src/DistrhoDefines.h"
#include "Graph.hpp"

START_NAMESPACE_DISTRHO

namespace wolf
{
class SmoothedGraph
{
  public:
    SmoothedGraph();

    Vertex *getVertexAtIndex(int index);

    /**
   * Return the number of vertices contained in the graph.
   */
    int getVertexCount();

    /**
   * Get the y value at x in the graph. 
   */
    float getValueAt(float x);

    void setBipolarMode(bool bipolarMode);
    void setWarpAmount(float warp);
    void setWarpType(WarpType warpType);

  /**
   * Rebuild the graph from a string.
   */
    void rebuildFromString(const char *serializedGraph);

  private:
    int fFrame;

    Graph fOldGraph;
    Graph fTargetGraph;
};

}; // namespace wolf

END_NAMESPACE_DISTRHO

#endif