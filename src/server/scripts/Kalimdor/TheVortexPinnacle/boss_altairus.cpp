/* 
 * Copyright (C) 2010-2011 Project SkyFire <http://www.projectskyfire.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include"ScriptPCH.h"
#include"WorldPacket.h"
#include"ScriptMgr.h"
#include"ScriptedCreature.h"
#include"SpellScript.h"
#include"SpellAuraEffects.h"
#include"SpellAuras.h"
#include"the_vortex_pinnacle.h"

enum Spells
{
    SPELL_CIILLING_BREATH_N       = 88308,
    SPELL_CIILLING_BREATH_H       = 93989,
	SPELL_CALL_THE_WIND           = 88244,
	SPELL_LIGHTING_BLAST_N        = 88357,
	SPELL_LIGHTING_BLAST_H        = 93988,
	SPELL_TWISTING_WINDS          = 88314,
	SPELL_UPWIND_OF_ALTAIRUS      = 88282,
	SPELL_DOWNWIND_OF_ALTAIRUS    = 88286,
};

Position const TwisterSummonPos[24] =
{
	{-1197.6f, 56.2342f, 734.174f, 0},
	{-594.312f, 1177.62f, 95.9784f, 0},
	{-1207.69f, 70.5042f, 734.174f, 0},
	{-1216.64f, 67.0546f, 734.174f, 0},
	{-1214.29f, 57.7803f, 734.174f, 0},
	{-1207.47f, 43.7191f, 734.174f, 0},
	{-1223.87f, 53.1735f, 734.174f, 0},
	{-1227.2f, 43.4324f, 734.174f, 0},
	{-1217.06f, 26.779f, 734.174f, 0},
	{-1233.09f, 31.1634f, 734.174f, 0},
	{-1242.14f, 58.9937f, 733.353f, 0},
	{-1237.32f, 66.7903f, 734.174f, 0},
	{-1232.82f, 76.1493f, 734.174f, 0},
	{-1224.77f, 83.2728f, 734.174f, 0},
	{-1216.1f, 91.4706f, 736.83f, 0},
	{-1206.04f, 87.7716f, 736.044f, 0},
	{-1191.39f, 87.677f, 739.588f, 0},
	{-1190.3f, 98.6693f, 740.733f, 0},
	{-1198.11f, 108.306f, 740.707f, 0},
	{-1209.05f, 108.034f, 740.727f, 0},
	{-1230.07f, 67.8356f, 734.174f, 0},
	{-1245.9f, 74.7447f, 731.654f, 0},
	{-1238.86f, 86.3723f, 731.654f, 0},
	{-1238.86f, 55.4677f, 734.183f, 0}
};


class boss_altairus : public CreatureScript
{
public:
    boss_altairus() : CreatureScript("boss_altairus") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_altairusAI (creature);
    }

        void EnterCombat(Unit* /*who*/)
        {
        }
        //void JustDied(Unit* /*Killer*/)
        /*{
            instance->SetData(DATA_ALTAIRUS, DONE);

            Creature* Slipstream = me->SummonCreature(NPC_SLIPSTREAM, -1190.88f, 125.20f, 737.62f, 1.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            Slipstream->SetUInt32Value(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_GOSSIP);

            Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

            if (PlList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                if (Player* player = i->getSource())
                    player->ModifyCurrency(395, DUNGEON_MODE(3000, 7000));
        }*/
        struct boss_altairusAI : public ScriptedAI
		{
			boss_altairusAI(Creature* pCreature) : ScriptedAI(pCreature), Summons(me)
			{
				pInstance = pCreature->GetInstanceScript();
			}

			InstanceScript *pInstance;			
			uint32 ChillingBreathTimer;
            EventMap events;
            SummonList Summons;
            bool check_in;

			uint32 m_uiSayTimer;                                    // Timer for random chat
            uint32 m_uiRebuffTimer;                                 // Timer for rebuffing
            uint32 m_uiSpellTimerCTW;                                 // Timer for spell 1 when in combat
            uint32 m_uiSpellTimerCTW_stop;                                 // Timer for spell 1 when in combat

            uint32 m_uiSpellTimerLB;                                 // Timer for spell 1 when in combat
            uint32 m_uiSpellTimerCB;                                 // Timer until we go into Beserk (enraged) mode

			uint32 m_uiSpellTimerSummTwister;
			uint32 m_uiSpellTimerSummTwister1;
			uint32 m_uiSpellTimerSummTwister2;
			uint32 m_uiSpellTimerSummTwister3;
			uint32 m_CombatDistance;

            uint32 m_uiPhase;                                       // The current battle phase we are in
            uint32 m_uiPhaseTimer;                                  // Timer until phase transition
			bool isCTW;
			float windOrientation;

            void Reset ()
			{	
			    DespawnCreatures(NPC_TWISTER);
                events.Reset();
				
				m_uiPhase = 1;                                      // Start in phase 1
                m_uiPhaseTimer = 60000;                             // 60 seconds

                m_uiSpellTimerCTW = urand(10000,15000);            // between 10 and 15 seconds
				m_uiSpellTimerCTW_stop = urand(2000,5000);         // between 2 and 5 seconds
				
                m_uiSpellTimerLB = urand(7000,15000);               // between 10 and 20 seconds
                m_uiSpellTimerCB = urand(7000,15000);               // between 10 and 20 seconds

                m_uiSpellTimerSummTwister = 5000;                           //  2 minutes
				m_uiSpellTimerSummTwister1 = 10000;
				m_uiSpellTimerSummTwister2 = 15000;
				m_uiSpellTimerSummTwister3 = 20000;
				m_CombatDistance = 2;
                                
                if (pInstance && (pInstance->GetData(DATA_ALTAIRUS_EVENT) != DONE && !check_in))
                    pInstance->SetData(DATA_ALTAIRUS_EVENT, NOT_STARTED);
                
                check_in = false;
				isCTW = false;
			}
			
            void JustDied(Unit* /*Kill*/)
            {
		            DespawnCreatures(NPC_TWISTER);
					
                if (pInstance)
                    pInstance->SetData(DATA_ALTAIRUS_EVENT, DONE);
            }

            void EnterCombat(Unit* /*Ent*/)
            {
				if (pInstance)
                    pInstance->SetData(DATA_ALTAIRUS_EVENT, IN_PROGRESS);
	
                DoZoneInCombat();
			}

			void UpdateAI(const uint32 uiDiff)
            {
                if (!UpdateVictim())  /* No target to kill */
                    return;
								
				events.Update(uiDiff);

				//Spell STOP CTW timer
	            if (m_uiSpellTimerCTW <= uiDiff && isCTW)
	            {
					if (me->HasAura(SPELL_CALL_THE_WIND))
						me-> RemoveAurasDueToSpell(SPELL_CALL_THE_WIND);
	                //DoCast(me, SPELL_CALL_THE_WIND);
					m_uiSpellTimerCTW_stop = urand(2000,5000);
					isCTW = false;
	            }
	            else if ( isCTW ) 
	                m_uiSpellTimerCTW -= uiDiff;

				//Spell CTW timer
				if (m_uiSpellTimerCTW_stop <= uiDiff && !isCTW)
	            {
	                    DoCast(me, SPELL_CALL_THE_WIND);
						windOrientation = me->GetOrientation();
						m_uiSpellTimerCTW = urand(10000,20000);
						isCTW = true;
	            }
	            else if ( !isCTW ) 
	                m_uiSpellTimerCTW_stop -= uiDiff;

                //if (me->HasUnitState(UNIT_STAT_CASTING))
                //    return;

	            //Spell CB timer
	            if (m_uiSpellTimerCB <= uiDiff)
	            {
	                //Cast spell two on our current target.
	                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    me->CastSpell(target, DUNGEON_MODE(SPELL_CIILLING_BREATH_N, SPELL_CIILLING_BREATH_H), true);
	                m_uiSpellTimerCB = urand(10000,200000);
	            }
	            else
	                m_uiSpellTimerCB -= uiDiff;
	
				//Spell LB timer
				if (m_uiSpellTimerLB <= uiDiff)
				{
					//Cast spell one on our current target.
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
					me->CastSpell(target, DUNGEON_MODE(SPELL_LIGHTING_BLAST_N, SPELL_LIGHTING_BLAST_H), true);

					m_uiSpellTimerLB = urand(10000, 20000);
				}
				else
					m_uiSpellTimerLB -= uiDiff;

				//Spell m_uiSpellTimerSummTwister timer
				if (m_uiSpellTimerSummTwister <= uiDiff)
				{
			if(me->GetMap()->IsHeroic())
			{
					//Cast spell one on our current target.
					me->SummonCreature(NPC_TWISTER, -1197.6f, 56.2342f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 8000);//1
					me->SummonCreature(NPC_TWISTER, -594.312f, 1177.62f, 95.9784f, 0, TEMPSUMMON_TIMED_DESPAWN, 8000);//2
					me->SummonCreature(NPC_TWISTER, -1217.06f, 26.779f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 8000);//9
					me->SummonCreature(NPC_TWISTER, -1233.09f, 31.1634f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 8000);//10
					me->SummonCreature(NPC_TWISTER, -1191.39f, 87.677f, 739.588f, 0, TEMPSUMMON_TIMED_DESPAWN, 8000);//17
					me->SummonCreature(NPC_TWISTER, -1190.3f, 98.6693f, 740.733f, 0, TEMPSUMMON_TIMED_DESPAWN, 8000);//18	
                    //DoCast(me->getVictim(), SPELL_TWISTING_WINDS);

					m_uiSpellTimerSummTwister = 10000;
				}
				}
				else
					m_uiSpellTimerSummTwister -= uiDiff;
										
				if (m_uiSpellTimerSummTwister1 <= uiDiff)
				{
			if(me->GetMap()->IsHeroic())
			{
					//Cast spell one on our current target.
					me->SummonCreature(NPC_TWISTER, -1207.69f, 70.5042f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000);//3
					me->SummonCreature(NPC_TWISTER, -1216.64f, 67.0546f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000);//4
					me->SummonCreature(NPC_TWISTER, -1242.14f, 58.9937f, 733.353f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000);//11
					me->SummonCreature(NPC_TWISTER, -1237.32f, 66.7903f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000);//12
					me->SummonCreature(NPC_TWISTER, -1198.11f, 108.306f, 740.707f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000);//19
					me->SummonCreature(NPC_TWISTER, -1209.05f, 108.034f, 740.727f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000);//20
                    //DoCast(me->getVictim(), SPELL_TWISTING_WINDS);

					m_uiSpellTimerSummTwister1 = 15000;
				}
				}
				else
					m_uiSpellTimerSummTwister1 -= uiDiff;
					
				if (m_uiSpellTimerSummTwister2 <= uiDiff)
				{
			if(me->GetMap()->IsHeroic())
			{				
					//Cast spell one on our current target.
					me->SummonCreature(NPC_TWISTER, -1214.29f, 57.7803f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//5
					me->SummonCreature(NPC_TWISTER, -1207.47f, 43.7191f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//6
					me->SummonCreature(NPC_TWISTER, -1232.82f, 76.1493f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//13
					me->SummonCreature(NPC_TWISTER, -1224.77f, 83.2728f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//14
					me->SummonCreature(NPC_TWISTER, -1230.07f, 67.8356f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//21
					me->SummonCreature(NPC_TWISTER, -1245.9f, 74.7447f, 731.654f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//22
                    //DoCast(me->getVictim(), SPELL_TWISTING_WINDS);

					m_uiSpellTimerSummTwister2 = 15000;
				}
				}
				else
					m_uiSpellTimerSummTwister2 -= uiDiff;

				if (m_uiSpellTimerSummTwister3 <= uiDiff)
				{
			if(me->GetMap()->IsHeroic())
			{
					//Cast spell one on our current target.
					me->SummonCreature(NPC_TWISTER, -1223.87f, 53.1735f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//7
					me->SummonCreature(NPC_TWISTER, -1227.2f, 43.4324f, 734.174f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//8
					me->SummonCreature(NPC_TWISTER, -1216.1f, 91.4706f, 736.83f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//15
					me->SummonCreature(NPC_TWISTER, -1206.04f, 87.7716f, 736.044f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//16
					me->SummonCreature(NPC_TWISTER, -1238.86f, 86.3723f, 731.654f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//23
					me->SummonCreature(NPC_TWISTER, -1238.86f, 55.4677f, 734.183f, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);//24	
                    //DoCast(me->getVictim(), SPELL_TWISTING_WINDS);

					m_uiSpellTimerSummTwister3 = 20000;
				}
				}
				else
					m_uiSpellTimerSummTwister3 -= uiDiff;					
					
				Map* pMap = me->GetMap();
                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (Player* i_pl = i->getSource())
                        if (i_pl->isAlive())
							if ( i_pl->isInFrontInMap(NULL , 4 , windOrientation ) )
								DoCast(i_pl,  SPELL_UPWIND_OF_ALTAIRUS);
							//else 
								//DoCast(i_pl,  SPELL_DOWNWIND_OF_ALTAIRUS);
	
                					
            //DoMeleeAttackIfReady();
			
        }
		
		void DespawnCreatures(uint32 entry)
		{
			std::list<Creature*> creatures;
			GetCreatureListWithEntryInGrid(creatures, me, entry, 10000000);

			if (creatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
				(*iter)->ForcedDespawn();
		}
    };
            
};

class npc_twister : public CreatureScript
{
    public:
        npc_twister() : CreatureScript("npc_twister") { }

        struct npc_twisterAI : public ScriptedAI
        {
            npc_twisterAI(Creature* creature) : ScriptedAI(creature)
            {
            }
			InstanceScript* pInstance;
			
			uint32 m_TwistingWindsTimer;
			
		    void Reset()
		    {
			  m_TwistingWindsTimer = 2000;
		    }   
			
            void IsSummonedBy(Unit* summoner)
            {
            }
			
            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if(m_TwistingWindsTimer <= diff)
                {
                  DoCast(me->getVictim(), SPELL_TWISTING_WINDS);
                
				m_TwistingWindsTimer = 2000;
				}
            else
                m_TwistingWindsTimer -= diff;

                }

            };
			
	    };

void AddSC_boss_altairus()
{
    new boss_altairus();
	new npc_twister();
}