#pragma once
#include "system/Object.hpp"
#include "components/Renderable.hpp"

namespace rt
{
	namespace object
	{
		class Teapot : public Object
		{
		public:
			Teapot();
			virtual ~Teapot();
			virtual void update(float delta) override;
		private:
			Renderable m_renderable;
		};
	}
}