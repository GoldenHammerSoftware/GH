#include "GHOculusGameScale.h"

namespace GHOculusUtil
{
	//test hack scaling
	//I am declaring all our bowling balls to be 21.95cm in diameter (middle of range stated on wikipedia)
	// That is 0.2195 meters, or 4.55 in meters/bowlingball
	//const float kGameScaleToMeters = 0.2195f;
	const float kGameScaleToMeters = 1.0f;
	const float kMetersToGameScale = 1.0f/kGameScaleToMeters;

	float getOculusToGameScale(void)
	{
		return kMetersToGameScale;
	}

	float getGameToOculusScale(void)
	{
		return kGameScaleToMeters;
	}
}
