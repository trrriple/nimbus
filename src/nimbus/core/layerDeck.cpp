#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/layerDeck.hpp"

namespace nimbus
{

LayerDeck::~LayerDeck()
{
    for (auto& layer : m_deck)
    {
        layer->onRemove();
    }
}

void LayerDeck::insertLayer(ref<Layer> p_layer, int32_t location)
{
    NM_PROFILE();

    if (p_layer->m_type == Layer::Type::REGULAR)
    {
        if (location == k_insertLocationHead || location > m_lastRegularLayerIdx)
        {
            // location is outside of the current range, so load it at the
            // end of the Regular layer portion of the deck
            location = m_lastRegularLayerIdx;
        }

        m_deck.insert(m_deck.begin() + location, p_layer);
        m_lastRegularLayerIdx++;
    }
    else if (p_layer->m_type == Layer::Type::OVERLAY)
    {
        // location is ignored for overlays and they are always pushed to
        // the absolute end of the deck
        m_deck.push_back(p_layer);
    }

    p_layer->onInsert();
}

void LayerDeck::removeLayer(ref<Layer> p_layer)
{
    NM_PROFILE();

    auto it = std::find(m_deck.begin(), m_deck.end(), p_layer);
    if (it != m_deck.end())
    {
        m_deck.erase(it);
        p_layer->onRemove();
    }
}

void LayerDeck::clear()
{
    NM_PROFILE();

    for (auto p_layer : m_deck)
    {
        p_layer->onRemove();
    }

    m_deck.clear();
}

const std::vector<std::string> LayerDeck::getLayerNames() const
{
    NM_PROFILE_TRACE();

    std::vector<std::string> layerNames;

    for (auto p_layer : m_deck)
    {
        layerNames.push_back(p_layer->m_name);
    }

    return layerNames;
}

}  // namespace nimbus