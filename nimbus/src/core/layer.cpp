#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/layer.hpp"

namespace nimbus
{

Layer::Layer(Type type, const std::string& name) : m_type(type), m_name(name)
{
}

}  // namespace nimbus
