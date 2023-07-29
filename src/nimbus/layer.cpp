#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/layer.hpp"

namespace nimbus
{

Layer::Layer(Type type, const std::string& name) : m_type(type), m_name(name)
{
}

}  // namespace nimbus
