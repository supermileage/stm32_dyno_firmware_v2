#include "pid/pid.h"

class PID
{
	public:
		PID()
		{

		}

		virtual ~PID() = default;

		bool Init();
		void Run();
	private:
		bool _enabled;

};



