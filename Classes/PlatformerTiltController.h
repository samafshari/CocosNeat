#ifndef _Platformer_Tiltcontroller_h
#define _Platformer_Tiltcontroller_h

#include "helpers.h"
#include "GameSettings.h"
#include "InputController.h"

namespace Platformer
{
	class TiltController : public InputController
	{
	public:
		FLOAT3 lastV;

		virtual bool init();

		CREATE_FUNC(TiltController);
		void onAcceleration(Acceleration* accel, Event*) override;
		void update(float);
	};
}

#endif
