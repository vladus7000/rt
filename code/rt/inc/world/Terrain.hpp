#pragma once
#include "components/Renderable.hpp"

namespace rt
{
	class Terrain : public Renderable
	{
	public:
		Terrain();
		virtual ~Terrain();

		virtual void update(float delta) override;
		virtual void draw(RenderableContext* context) override;
	};
}