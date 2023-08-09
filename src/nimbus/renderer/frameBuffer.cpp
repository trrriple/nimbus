#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/frameBuffer.hpp"

#include "platform/gl/glFrameBuffer.hpp"


namespace nimbus
{

ref<FrameBuffer> FrameBuffer::s_create(FrameBuffer::Spec& spec)
{
    return ref<GlFrameBuffer>::gen(spec);
}

}  // namespace nimbus