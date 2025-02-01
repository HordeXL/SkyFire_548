/*
* This file is part of Project SkyFire https://www.projectskyfire.org. 
* See LICENSE.md file for Copyright information
*/

/*
Name: Boss_Chrono_Lord_Deja
%Complete: 65
Comment: All abilities not implemented
Category: Caverns of Time, The Black Morass
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "the_black_morass.h"

enum Enums
{
    SAY_ENTER                   = 0,
    SAY_AGGRO                   = 1,
    SAY_BANISH                  = 2,
    SAY_SLAY                    = 3,
    SAY_DEATH                   = 4,

    SPELL_ARCANE_BLAST          = 31457,
    H_SPELL_ARCANE_BLAST        = 38538,
    SPELL_ARCANE_DISCHARGE      = 31472,
    H_SPELL_ARCANE_DISCHARGE    = 38539,
    SPELL_TIME_LAPSE            = 31467,
    SPELL_ATTRACTION            = 38540                       //Not Implemented (Heroic mode)
};

enum Events
{
    EVENT_ARCANE_BLAST          = 1,
    EVENT_TIME_LAPSE            = 2,
    EVENT_ARCANE_DISCHARGE      = 3,
    EVENT_ATTRACTION            = 4
};

class boss_chrono_lord_deja : public CreatureScript
{
public:
    boss_chrono_lord_deja() : CreatureScript("boss_chrono_lord_deja") { }

    struct boss_chrono_lord_dejaAI : public BossAI
    {
        boss_chrono_lord_dejaAI(Creature* creature) : BossAI(creature, TYPE_CRONO_LORD_DEJA) { }

        void Reset() OVERRIDE { }

        void EnterCombat(Unit* /*who*/) OVERRIDE
        {
            events.ScheduleEvent(EVENT_ARCANE_BLAST, std::rand() % 23000 + 18000);
            events.ScheduleEvent(EVENT_TIME_LAPSE, std::rand() % 15000 + 10000);
            events.ScheduleEvent(EVENT_ARCANE_DISCHARGE, std::rand() % 30000 + 20000);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_ATTRACTION, std::rand() % 35000 + 25000);

            Talk(SAY_AGGRO);
        }

        void MoveInLineOfSight(Unit* who) OVERRIDE

        {
            //Despawn Time Keeper
            if (who->GetTypeId() == TypeID::TYPEID_UNIT && who->GetEntry() == NPC_TIME_KEEPER)
            {
                if (me->IsWithinDistInMap(who, 20.0f))
                {
                    Talk(SAY_BANISH);
                    me->DealDamage(who, who->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }

            ScriptedAI::MoveInLineOfSight(who);
        }

        void KilledUnit(Unit* /*victim*/) OVERRIDE
        {
            Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*killer*/) OVERRIDE
        {
            Talk(SAY_DEATH);

            if (instance)
                instance->SetData(TYPE_RIFT, SPECIAL);
        }

        void UpdateAI(uint32 diff) OVERRIDE
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_ARCANE_BLAST:
                        DoCastVictim(SPELL_ARCANE_BLAST);
                        events.ScheduleEvent(EVENT_ARCANE_BLAST, std::rand() % 25000 + 15000);
                        break;
                    case EVENT_TIME_LAPSE:
                        Talk(SAY_BANISH);
                        DoCast(me, SPELL_TIME_LAPSE);
                        events.ScheduleEvent(EVENT_TIME_LAPSE, std::rand() % 25000 + 15000);
                        break;
                    case EVENT_ARCANE_DISCHARGE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_ARCANE_DISCHARGE);
                        events.ScheduleEvent(EVENT_ARCANE_DISCHARGE, std::rand() % 30000 + 20000);
                        break;
                    case EVENT_ATTRACTION: // Only in Heroic
                        DoCast(me, SPELL_ATTRACTION);
                        events.ScheduleEvent(EVENT_ATTRACTION, std::rand() % 35000 + 25000);
                        break;
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_chrono_lord_dejaAI(creature);
    }
};

void AddSC_boss_chrono_lord_deja()
{
    new boss_chrono_lord_deja();
}
