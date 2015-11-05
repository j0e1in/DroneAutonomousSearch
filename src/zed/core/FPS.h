
#include <chrono>

class Interval
{
private:
	std::chrono::high_resolution_clock::time_point t_init;

public:
	inline Interval() : t_init(std::chrono::high_resolution_clock::now()){}

	virtual ~Interval(){}

	inline double value()
	{
		return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t_init).count() * 1000.;
	}
};

class Fps
{
protected:
	unsigned int m_fps;
	unsigned int m_fpscount;
	Interval m_fpsinterval;

public:
	Fps() : m_fps(0), m_fpscount(0){}

	void update()
	{
		m_fpscount++;

		if (m_fpsinterval.value() > 1000)
		{
			m_fps = m_fpscount;

			m_fpscount = 0;
			m_fpsinterval = Interval();
		}
	}

	unsigned int get() const
	{
		return m_fps;
	}
};