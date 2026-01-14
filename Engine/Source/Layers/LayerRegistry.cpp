#include "hepch.h"
#include "LayerRegistry.h"

void LayerRegistry::PushLayer(Layer* layer) {
	_Layers.emplace(_Layers.begin() + _LayerInsertIndex, layer);
	_LayerInsertIndex++;
	layer->OnAttach();
}

void LayerRegistry::PushOverlay(Layer* layer)
{
	_Layers.emplace_back(layer);
	layer->OnAttach();
}

void LayerRegistry::PopLayer(Layer* layer){ }

void LayerRegistry::Clear() {
	for (Layer* layer : _Layers) {
		delete layer;
		layer = nullptr;
	}
	_Layers.clear();
	_LayerInsertIndex = 0;
	LOG_CORE_INFO("LayerStack cleared");
}