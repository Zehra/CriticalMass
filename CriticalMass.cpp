// CriticalMass.cpp

#include "bzfsAPI.h"

class CriticalMass : public bz_Plugin
{
   public:
   const char* Name(){return "CriticalMass[0.0.1]";}
   void Init ( const char* /*config*/ );
   void Event(bz_EventData *eventData );
   void Cleanup ( void );
   bz_eGameType gameType;
};

void CriticalMass::Init (const char* commandLine) {
   Register(bz_ePlayerDieEvent);
   bz_RegisterCustomFlag("CM", "CriticalMass", "Death results in a shockwave, but all kills are yours.", 0, eGoodFlag);
   gameType = bz_getGameType();
}

void CriticalMass::Cleanup (void) {
   Flush();
}

void CriticalMass::Event(bz_EventData *eventData ) {
   switch (eventData->eventType) {
      case bz_ePlayerDieEvent: {
      bz_PlayerDieEventData_V2* deathData = (bz_PlayerDieEventData_V2*)eventData;
      int player = deathData->playerID;
      if((deathData->killerID == 253) && ((deathData->shotID != -1))) {
          uint32_t shotGUID = bz_getShotGUID(deathData->killerID, deathData->shotID);
          if(bz_shotHasMetaData(shotGUID, "killer")) {
            int killerID = bz_getShotMetaDataI(shotGUID, "killer");
            bz_eTeamType killTeam = bz_getPlayerTeam(killerID);
            if ((gameType != eOpenFFAGame) && (killTeam != eRogueTeam && deathData->team != eRogueTeam)) {
                if (deathData->team == killTeam) {
                    //printf("%d has %d in loss\n", player, bz_getPlayerLosses(player));
                    if (bz_getPlayerLosses(player) == 0) {
                        bz_setPlayerLosses(player, 0);
                    } else {
                        bz_incrementPlayerLosses(player, -1);
                    }
                    //^Prevent weird scoring errors...
                    //bz_incrementPlayerWins(player, 1);
                    // ^Probably should be commented out in servers where -noTeamKills is enabled.
                    //printf("%d has %d in losses now\n", player, bz_getPlayerLosses(player));
                } else {
                    deathData->killerID = killerID;
                }
            } else {
              deathData->killerID = killerID;
            }
          }
      }
      if (deathData->flagHeldWhenKilled != -1) {
        if (strcmp(bz_getFlagName(deathData->flagHeldWhenKilled).c_str(), "CM") == 0) {
            float vector[3] = {0, 0, 0};
            uint32_t shotGUID = bz_fireServerShot("SW", deathData->state.pos, vector, deathData->team);
            bz_setShotMetaData(shotGUID, "killer", deathData->playerID);
        }
      }
    }break;

    default:{ 
    }break;
  }
}

BZ_PLUGIN(CriticalMass)



