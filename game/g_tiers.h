typedef struct{
	qboolean exists;
	qboolean customWeapons;
	qboolean permanent;
	qboolean requirementPowerLevelButton;
	int requirementPowerLevelCurrent;
	int requirementPowerLevelTotal;
	int requirementPowerLevelMaximum;
	int zanzokenCost;
	int zanzokenDistance;
	int speed;
	float meleeAttack;
	float meleeDefense;
	float energyDefense;
	float energyAttackDamage;
	float energyAttackCost;
	float transformTime;
	float powerLevelEffect;
	float powerLevelTotalEffect;
	float powerLevelMaximumEffect;
	float powerLevelBreakLimitRate;
}tierConfig_g;
