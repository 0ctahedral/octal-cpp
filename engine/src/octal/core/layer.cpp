#include "octal/core/layer.h"
#include <algorithm>

namespace octal {

  Layer::Layer(const std::string& name) : m_DebugName(name) { }

  LayerStack::~LayerStack() {
    for (Layer* layer : m_Layers) {
      layer->OnPop();
      delete layer;
    }
  }

		void LayerStack::PushLayer(Layer* layer) {
      layer->OnPush();
      m_Layers.emplace(m_Layers.begin() + m_LayerIdx, layer);
      m_LayerIdx++;
    }

		void LayerStack::PushOverlay(Layer* overlay) {
      overlay->OnPush();
      m_Layers.emplace_back(overlay);
    }

		void LayerStack::PopLayer(Layer* layer) {
      auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerIdx, layer);
      if (it != m_Layers.begin() + m_LayerIdx)
      {
        layer->OnPop();
        m_Layers.erase(it);
        m_LayerIdx--;
      }
    }

		void LayerStack::PopOverlay(Layer* overlay) {
      auto it = std::find(m_Layers.begin() + m_LayerIdx, m_Layers.end(), overlay);
      if (it != m_Layers.end())
      {
        overlay->OnPop();
        m_Layers.erase(it);
      }
    }

}
