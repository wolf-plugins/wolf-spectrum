#ifndef WOLF_GRAPH_TIMELINE_DEFINED_H
#define WOLF_GRAPH_TIMELINE_DEFINED_H

#include "src/DistrhoDefines.h"
#include "Graph.hpp"

START_NAMESPACE_DISTRHO

namespace wolf
{
/**
  * The max number of graphs that can be in the timeline at the same time.
  */
const int maxKeyframes = 99;

class GraphTimeline
{
  public:
    GraphTimeline();

    void seek(float time);
    float getValueAt(float x);

    int getKeyframeCount();

    const char *serialize();
    void rebuildFromString(const char *serializedTimeline);

  private:
    float fTimePos;

    Graph fKeyframes[maxKeyframes];
    int fKeyframeCount;
};

} // namespace wolf

END_NAMESPACE_DISTRHO

#endif