#pragma once

#include <deque>
#include <vector>

#include "nimbus/core/common.hpp"
#include "nimbus/core/layer.hpp"

namespace nimbus
{

static const i32_t k_insertLocationHead = -1;

class LayerDeck
{
   public:
    LayerDeck() = default;

    ~LayerDeck();

    void insertLayer(ref<Layer> p_layer, i32_t location = k_insertLocationHead);

    void removeLayer(ref<Layer> p_layer);

    void clear();

    const std::vector<std::string> getLayerNames() const;

    ////////////////////////////////////////////////////////////////////
    // Foward Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<ref<Layer>>::iterator begin()
    {
        return m_deck.begin();
    }

    std::deque<ref<Layer>>::iterator end()
    {
        return m_deck.end();
    }

    ////////////////////////////////////////////////////////////////////
    // Reverse Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<ref<Layer>>::reverse_iterator rbegin()
    {
        return m_deck.rbegin();
    }

    std::deque<ref<Layer>>::reverse_iterator rend()
    {
        return m_deck.rend();
    }

    ////////////////////////////////////////////////////////////////////
    // Constant Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<ref<Layer>>::const_iterator begin() const
    {
        return m_deck.begin();
    }

    std::deque<ref<Layer>>::const_iterator end() const
    {
        return m_deck.end();
    }

    ////////////////////////////////////////////////////////////////////
    // Constant Reverse Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<ref<Layer>>::const_reverse_iterator rbegin() const
    {
        return m_deck.rbegin();
    }

    std::deque<ref<Layer>>::const_reverse_iterator rend() const
    {
        return m_deck.rend();
    }

   private:
    std::deque<ref<Layer>> m_deck;
    i32_t                  m_lastRegularLayerIdx = 0;
};
}  // namespace nimbus