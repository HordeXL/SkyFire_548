/*
* This file is part of Project SkyFire https://www.projectskyfire.org.
* See LICENSE.md file for Copyright information
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"

enum CaveOfMeditation
{
    //SPELL_SUMMON_LI_FEI = 102445, //
    //SPELL_LI_FEI_VISUAL_GHOST_AURA = 22650,
    //SPELL_Aysa_Cave_of_Scrolls_Comp_Timer_Aura = 128598,
    SPELL_SEE_QUEST_INVIS_7 = 102396,
    //SPELL_GENERIC_QUEST_INVISIBILITY_7 = 85096,
    //SPELL_AYSA_BAR = 116421,
};

const Position AddSpawnPos[2] =
{
    { 1184.7f, 3448.3f, 102.5f, 0.0f },
    { 1186.7f, 3439.8f, 102.5f, 0.0f },
};

const Position AddPointPos[2] =
{
    { 1145.13f, 3432.88f, 105.268f, 0.0f },
    { 1143.36f, 3437.39f, 104.973f, 0.0f },
};
/*####
# npc_aysa_meditation
####*/
class npc_aysa_meditation : public CreatureScript
{
    enum EventsAysaMeditation
    {
        EVENT_POWER = 1,
        EVENT_ADDS = 2,
    };

public:
    npc_aysa_meditation() : CreatureScript("npc_aysa_meditation") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new npc_aysa_meditationAI(creature);
    }
    struct npc_aysa_meditationAI : public ScriptedAI
    {
        npc_aysa_meditationAI(Creature* creature) : ScriptedAI(creature) {
            if (me->GetAreaId() == 5848) // Cave of Meditation Area
            {
                events.ScheduleEvent(EVENT_POWER, 1000);
                events.ScheduleEvent(EVENT_ADDS, 1000);
            }
        }
        EventMap events;
        uint32 Power = 0;
        std::vector<Player*> playersParticipate;

        void UpdatePlayerList()
        {
            playersParticipate.clear();

            std::list<Player*> PlayerList;
            me->GetPlayerListInGrid(PlayerList, 20.0f);

            for (auto&& player : PlayerList)
                if (!player->IsGameMaster() && player->GetQuestStatus(29414) == QUEST_STATUS_INCOMPLETE)
                    playersParticipate.push_back(player);
        }

        void UpdateAI(uint32 diff) OVERRIDE
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId) {
                case EVENT_POWER:
                {
                    UpdatePlayerList();
                    if (!playersParticipate.empty())
                    {
                        Power++;

                        for (auto&& player : playersParticipate)
                        {
                            player->SetPower(POWER_ALTERNATE_POWER, Power);
                            player->SetMaxPower(POWER_ALTERNATE_POWER, 90);

                            if (Power >= 90)
                            {
                                player->CastSpell(player, SPELL_SEE_QUEST_INVIS_7);
                                player->KilledMonsterCredit(54856, 0);
                                player->RemoveAura(116421);
                            }
                        }
                    }
                    events.ScheduleEvent(EVENT_POWER, 1000);
                    break;
                }
                case EVENT_ADDS:
                {
                    for (uint8 i = 0; i < 2; ++i)
                    {
                        if (Creature* troublemaker = me->SummonCreature(61801, AddSpawnPos[i], TempSummonType::TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
                        {
                            troublemaker->GetMotionMaster()->MovePoint(0, AddPointPos[i]);
                        }
                    }
                    events.ScheduleEvent(EVENT_ADDS, 30000);
                    break;
                }
                default:
                    break;
                }
            }
        }
    };
};

/*####
# npc_aysa_cloudsinger
####*/
Position const AysaSpawnPos = { 1206.31f, 3507.45f, 85.99f };
Position const AysaJumpPos1 = { 1196.72f, 3492.85f, 90.9836f, 0 };
Position const AysaJumpPos2 = { 1192.29f, 3478.69f, 108.788f, 0 };
Position const AysaJumpPos3 = { 1197.99f, 3460.63f, 103.04f, 0 };
Position const AysaMovePos4 = { 1176.1909f, 3444.8743f, 103.35291f, 0 };
Position const AysaMovePos5 = { 1149.9497f, 3437.1702f, 104.967064f, 0 };

class npc_aysa_cloudsinger : public CreatureScript
{
    enum EventsAysaCloudsinger
    {
        EVENT_AYSA_JUMP_POS_1 = 1,
        EVENT_AYSA_JUMP_POS_2,
        EVENT_AYSA_JUMP_POS_3,
        EVENT_AYSA_MOVE_POS_4,
        EVENT_AYSA_MOVE_POS_5,
        EVENT_AYSA_DESPAWN,
    };

public:
    npc_aysa_cloudsinger() : CreatureScript("npc_aysa_cloudsinger") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) OVERRIDE
    {
        if (quest->GetQuestId() == 29414)
        {
            creature->MonsterSay("Meet me up in the cave if you would. friend.", Language::LANG_UNIVERSAL, player);
            creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
            creature->GetMotionMaster()->MovePoint(0, AysaSpawnPos);
            return true;
        }
        return false;
    }

    struct npc_aysa_cloudsingerAI : public ScriptedAI
    {
        npc_aysa_cloudsingerAI(Creature* creature) : ScriptedAI(creature) { }

        EventMap events;

        void MovementInform(uint32 type, uint32 id) OVERRIDE
        {
            if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                return;

            switch (id)
            {
            case 0:
                events.ScheduleEvent(EVENT_AYSA_JUMP_POS_1, 5000);
                return;
            default:
                break;
            }
        }

        void UpdateAI(uint32 diff) OVERRIDE
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_AYSA_JUMP_POS_1:
                    me->GetMotionMaster()->MoveJump(AysaJumpPos1, 15.0f, 15.0f, 1);
                    events.ScheduleEvent(EVENT_AYSA_JUMP_POS_2, 2000);
                    break;
                case EVENT_AYSA_JUMP_POS_2:
                    me->GetMotionMaster()->MoveJump(AysaJumpPos2, 15.0f, 25.0f, 2);
                    events.ScheduleEvent(EVENT_AYSA_JUMP_POS_3, 2000);
                    break;
                case EVENT_AYSA_JUMP_POS_3:
                    me->GetMotionMaster()->MoveJump(AysaJumpPos3, 15.0f, 15.0f, 3);
                    events.ScheduleEvent(EVENT_AYSA_MOVE_POS_4, 2000);
                    break;
                case EVENT_AYSA_MOVE_POS_4:
                    me->GetMotionMaster()->MovePoint(4, AysaMovePos4);
                    events.ScheduleEvent(EVENT_AYSA_MOVE_POS_5, 2000);
                    break;
                case EVENT_AYSA_MOVE_POS_5:
                    me->GetMotionMaster()->MovePoint(5, AysaMovePos5);
                    events.ScheduleEvent(EVENT_AYSA_DESPAWN, 4000);
                    break;
                case EVENT_AYSA_DESPAWN:
                    me->DespawnOrUnsummon(1000);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new npc_aysa_cloudsingerAI(creature);
    }
};

void AddSC_wandering_island()
{
    new npc_aysa_meditation();
    new npc_aysa_cloudsinger();
}
