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
#include"the_vortex_pinnacle.h"

enum eEvents
{
    EVENT_NONE,
    EVENT_BLAZE,
    EVENT_SOUL_SEVER,
    EVENT_WAIL_DARKNESS,
    EVENT_CHAIN_LIGHTNING,
    EVENT_HEX,
    EVENT_METEOR,
    EVENT_SHADOW_BOLT,
    EVENT_DISEASE_BREATH,
    EVENT_INFECTIOUS_PLAGUE,
    EVENT_MIXTURE,
    EVENT_RITUAL_BLOODLETTING,
    EVENT_VICIOUS_LEECHES,
    EVENT_BRANDED_TONGUE,
    EVENT_SERUM_TORMENT,
    EVENT_SKULL_CRUSH,
    EVENT_SLAM,
    EVENT_SKULL_CRACK,
    EVENT_BLADED_SHIELD,
    EVENT_RESISTANCE,
    EVENT_CALL_FALCON,
    EVENT_SHOOT,
    EVENT_WING_CLIP,
    EVENT_BLINK,
    EVENT_CALL_SCORPID,
    EVENT_EVASION,
    EVENT_FAN_KNIVES,
    EVENT_SPINAL_PIERCE,
    EVENT_FERAL_LINK,
    EVENT_CALL_CROCO,
    EVENT_FORKED_SHOT,
    EVENT_IMPALE,
    EVENT_SHOCKWAVE,
    EVENT_DRAGON_BREATH,
    EVENT_FIRE_BLAST,
    EVENT_TOXIC_DART,
    EVENT_LIGHTNING_NOVA,
    EVENT_THUNDER_CRASH,
	EVENT_STORM,
	EVENT_CLING,
	EVENT_FIELD,
	EVENT_CHAIN,
	EVENT_SUMMON,
	EVENT_SAY,
	EVENT_BARRAGE,
};

enum Creatures
{
    NPC_SKYFALL_STAR = 45932,
};

const Position aSpawnLocations[5] =
{
    {2728.12f, -3544.43f, 261.91f, 6.04f},
    {2729.05f, -3544.47f, 261.91f, 5.58f},
    {2728.24f, -3465.08f, 264.20f, 3.56f},
    {2704.11f, -3456.81f, 265.53f, 4.51f},
    {2663.56f, -3464.43f, 262.66f, 5.20f},
};

class npc_slipstream : public CreatureScript
{
public:
    npc_slipstream() : CreatureScript("npc_slipstream") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 Sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (Sender != GOSSIP_SENDER_MAIN)
            return true;
        if (!player->getAttackers().empty())
            return true;

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->TeleportTo(657, -914.87f, -190.27f, 664.50f, 2.43f);
            break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->TeleportTo(657, -1189.04f, 475.85f, 634.78f, 0.46f);
            break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        InstanceScript* instance = creature->GetInstanceScript();

        //if (instance && instance->GetData(DATA_GRAND_VIZIER_ERTAN)==DONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport me to platform 2", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        //if (instance && instance->GetData(DATA_ALTAIRUS)==DONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport me to platform 3", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

        player->SEND_GOSSIP_MENU(2475, creature->GetGUID());
        return true;
    }
};

void AddSC_vortex_pinnacle()
{
    new npc_slipstream();
}