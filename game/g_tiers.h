typedef struct{
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
	float powerLevelDrain;
	float powerLevelTotalDrain;
	float powerLevelMaximumDrain;
	float powerLevelChargeRate;
	float powerLevelBreakLimitRate;
}tierConfig_g;
typedef struct{
	tierConfig_g tiers[8];
}tierList;
