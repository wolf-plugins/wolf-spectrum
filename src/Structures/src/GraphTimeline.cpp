#include "GraphTimeline.hpp"
#include "Mathf.hpp"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cstring>

START_NAMESPACE_DISTRHO

//TODO: make this work

namespace wolf
{
GraphTimeline::GraphTimeline() : fTimePos(0.0f),
                                 fKeyframeCount(0)
{

}

void GraphTimeline::seek(float time)
{
    fTimePos = time;
}

float GraphTimeline::getValueAt(float x)
{
    return wolf::lerp(fKeyframes[0].getValueAt(x), fKeyframes[1].getValueAt(x), fTimePos);
}

int GraphTimeline::getKeyframeCount()
{
    return fKeyframeCount;
}

const char *GraphTimeline::serialize()
{
    for (int i = 0; i < fKeyframeCount; ++i)
    {
        //TODO
    }
}

void GraphTimeline::rebuildFromString(const char *serializedTimeline)
{
    char *rest = (char *const)serializedTimeline;
}

} // namespace wolf

END_NAMESPACE_DISTRHO
