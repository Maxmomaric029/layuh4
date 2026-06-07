#pragma once

#include "entry.h"

class CMisc {
public:
	void teleport_to_nearest(matrix viewmatrix);
	void noclip();
	void fly();
	void spinbot();
	void speed_hack();
	void antistomp_realud();
	//void instant_proximity_prompt();
	void jump_power();
	void rapid_fire();
	void headless();
	void NoJumpCoolDown();
};

namespace fs { inline CMisc misc; }
