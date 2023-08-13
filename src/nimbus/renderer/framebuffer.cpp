#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/framebuffer.hpp"

#include "platform/gl/glFramebuffer.hpp"

namespace nimbus
{

ref<Framebuffer> Framebuffer::s_create(Framebuffer::Spec& spec) noexcept
{
    return ref<GlFramebuffer>::gen(spec);
}

}  // namespace nimbus