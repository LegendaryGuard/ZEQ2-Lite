#include "cg_local.h"
#define MUSIC_MAXTYPES 10
#define MUSIC_MAXTRACKS 32
#define MUSIC_MAXDURATION 300000
struct{
	char playlist[MUSIC_MAXTYPES][MUSIC_MAXTRACKS][MAX_QPATH];
	int trackLength[MUSIC_MAXTYPES][MUSIC_MAXTRACKS];
	int typeSize[MUSIC_MAXTYPES];
	int lastTrack[MUSIC_MAXTYPES];
	int currentType;
	int fadeAmount;
	int endTime;
	int volume;
	qboolean isRandom;
	qboolean isFading;
	qboolean playToEnd;
}Music;
typedef enum{
	Fighting,
	Idling,
	Underwater,
	IdleDanger,
	Struggling,
	Standoff,
	StandoffDanger,
	VictoryGood,
	VictoryEvil,
	Transforming
}TrackTypes;
void CG_Music_ParsePlaylist(void);
void CG_Music_CheckType(int type);
