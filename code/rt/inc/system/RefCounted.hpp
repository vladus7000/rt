#pragma once

namespace rt
{
#ifdef RT_USE_REF_CHECKER
	class RefCounderChecker
	{
	public:
		RefCounderChecker()
			: m_refObjects(0)
		{
		}

		~RefCounderChecker()
		{
			if (m_refObjects != 0)
			{
				__debugbreak();
			}
		}

		void inc()
		{
			//TODO: atomic
			m_refObjects++;
		}

		void dec()
		{
			//TODO: atomic
			m_refObjects--;
		}
	private:
		int64 m_refObjects;
	};

	static RefCounderChecker refChecker;
#endif
	class RefCounted
	{
	public:
		RefCounted()
			: m_counter(1)
		{
#ifdef RT_USE_REF_CHECKER
			refChecker.inc();
#endif
		}

		void acquire()
		{
			//TODO: atomic
			m_counter++;
#ifdef RT_USE_REF_CHECKER
			refChecker.inc();
#endif
		}

		void release()
		{
			//TODO: atomic
			m_counter--;
#ifdef RT_USE_REF_CHECKER
			refChecker.dec();
#endif
			if (m_counter < 0)
			{
				__debugbreak();
			}
			if (m_counter == 0)
			{
				delete this;
			}
		}
	protected:
		virtual ~RefCounted()
		{}
	private:
		int64 m_counter;
	};
}