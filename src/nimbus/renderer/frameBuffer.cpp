#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/renderer/frameBuffer.hpp"

#include "platform/gl/glFrameBuffer.hpp"


namespace nimbus
{

ref<FrameBuffer> FrameBuffer::s_create(FrameBuffer::Spec& spec)
{
    return makeRef<GlFrameBuffer>(spec);
}

}  // namespace nimbus