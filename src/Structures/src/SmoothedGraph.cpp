#include "SmoothedGraph.hpp"
#include "Mathf.hpp"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>

START_NAMESPACE_DISTRHO

namespace wolf
{

const int framesToSmooth = 1024;

SmoothedGraph::SmoothedGraph() : fFrame(0),
                                 fOldGraph(),
                                 fTargetGraph()
{

}

float SmoothedGraph::getValueAt(float x)
{
    if (frame == framesToSmooth)
    {
        return fTargetGraph.getValueAt(x);
    }

    return wolf::lerp(fOldGraph.getValueAt(x), fTargetGraph.getValueAt(x), (float)frame / (float)framesToSmooth);
}

void SmoothedGraph::setWarpAmount(float warp)
{
    fTargetGraph.warpAmount = warp;
    frame = 0;
}

void SmoothedGraph::setWarpType(WarpType warpType)
{
    fTargetGraph.warpType = warpType;
    frame = 0;
}

Vertex *SmoothedGraph::getVertexAtIndex(int index)
{
    assert(index < vertexCount);

    //TODO: Implement this
    return nullptr;
}

int SmoothedGraph::getVertexCount()
{
    return fTargetGraph.getVertexCount();
}

void SmoothedGraph::setBipolarMode(bool bipolarMode)
{
    fTargetGraph.setBipolarMode(bipolarMode);
}

void SmoothedGraph::rebuildFromString(const char *serializedGraph)
{
    if (frame == framesToSmooth) //this is bad. need to be some way to set the old graph to the lerped version of it
    {
        frame = 0;
        fOldGraph = fTargetGraph;
    }

    fTargetGraph.rebuildFromString(serializedGraph);
}

} // namespace wolf

END_NAMESPACE_DISTRHO
