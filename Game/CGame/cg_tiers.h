#define TIERNAMELENGTH 15
#define MAX_TIERS 10 // Must be the same as g_tiers.h
#define MAX_DAMAGED_STATES 10
typedef struct{
	char name[TIERNAMELENGTH];
	char transformMusic[MAX_QPATH];
	qhandle_t headModel;
	qhandle_t headModelDamaged[MAX_DAMAGED_STATES];
	qhandle_t headSkin;
	qhandle_t headSkinDamaged[MAX_DAMAGED_STATES];
	qhandle_t torsoModel;
	qhandle_t torsoModelDamaged[MAX_DAMAGED_STATES];
	qhandle_t torsoSkin;
	qhandle_t torsoSkinDamaged[MAX_DAMAGED_STATES];
	qhandle_t legsModel;
	qhandle_t legsModelDamaged[MAX_DAMAGED_STATES];
	qhandle_t legsSkin;
	qhandle_t legsSkinDamaged[MAX_DAMAGED_STATES];
	qhandle_t cameraModel;
	qhandle_t crosshair;
	qhandle_t crosshairPowering;
	qhandle_t screenEffect[MAX_DAMAGED_STATES];
	qhandle_t screenEffectPowering;
	qhandle_t screenEffectTransforming;
	qhandle_t icon2D[MAX_DAMAGED_STATES];
	qhandle_t icon2DPowering;
	qhandle_t icon2DTransforming;
	qboolean damageFeatures;
	qboolean damageModelsRevertHealed;
	qboolean damageTexturesRevertHealed;
	qboolean transformScriptExists;
	float meshScale;
	float icon3DZoom;
	float hudMultiplier;
	int icon3DOffset[2];
	int icon3DRotation[3];
	int icon3DSize[2];
	int cameraOffset[3];
	int meshOffset;
	int sustainCurrent;
	int sustainCurrentPercent;
	int sustainFatigue;
	int sustainHealth;
	int sustainMaximum;
	int requirementCurrent;
	int requirementCurrentPercent;
	int requirementFatigue;
	int requirementHealth;
	int requirementHealthMaximum;
	int requirementMaximum;
	int transformMusicLength;
	sfxHandle_t soundTransformFirst;
	sfxHandle_t soundTransformUp;
	sfxHandle_t soundTransformDown;
	sfxHandle_t soundPoweringUp;
}tierConfig_cg;
void parseTier(char *path,tierConfig_cg *tier);
