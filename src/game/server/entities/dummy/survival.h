#ifndef GAME_SERVER_ENTITIES_DUMMY_SURVIVAL_H
#define GAME_SERVER_ENTITIES_DUMMY_SURVIVAL_H

#include "dummybase.h"

class CDummySurvival : public CDummyBase
{
public:
	CDummySurvival(class CCharacter *pChr, class CPlayer *pPlayer);
	virtual void OnTick() override;
	virtual ~CDummySurvival(){};
	const char *ModeStr() override { return "Survival"; }

private:
	int m_DummyDir;
};

#endif
