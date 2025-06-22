/*
* This file is part of Project SkyFire https://www.projectskyfire.org. 
* See LICENSE.md file for Copyright information
*/

/* ScriptData
SDName: Boss_Sartura
SD%Complete: 95
SDComment:
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Sartura
{
    SAY_AGGRO           = 0,
    SAY_SLAY            = 1,
    SAY_DEATH           = 2,

    SPELL_WHIRLWIND     = 26083,
    SPELL_ENRAGE        = 28747,            //Not sure if right ID.
    SPELL_ENRAGEHARD    = 28798,

//Guard Spell
    SPELL_WHIRLWINDADD  = 26038,
    SPELL_KNOCKBACK     = 26027
};

class boss_sartura : public CreatureScript
{
public:
    boss_sartura() : CreatureScript("boss_sartura") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_sarturaAI(creature);
    }

    struct boss_sarturaAI : public ScriptedAI
    {
        boss_sarturaAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 WhirlWind_Timer;
        uint32 WhirlWindRandom_Timer;
        uint32 WhirlWindEnd_Timer;
        uint32 AggroReset_Timer;
        uint32 AggroResetEnd_Timer;
        uint32 EnrageHard_Timer;

        bool Enraged;
        bool EnragedHard;
        bool WhirlWind;
        bool AggroReset;

        void Reset() OVERRIDE
        {
            WhirlWind_Timer = 30000;
            WhirlWindRandom_Timer = std::rand() % 7000 + 3000;
            WhirlWindEnd_Timer = 15000;
            AggroReset_Timer = std::rand() % 55000 + 45000;
            AggroResetEnd_Timer = 5000;
            EnrageHard_Timer = 10*60000;

            WhirlWind = false;
            AggroReset = false;
            Enraged = false;
            EnragedHard = false;
        }

        void EnterCombat(Unit* /*who*/) OVERRIDE
        {
            Talk(SAY_AGGRO);
        }

         void JustDied(Unit* /*killer*/) OVERRIDE
         {
             Talk(SAY_DEATH);
         }

         void KilledUnit(Unit* /*victim*/) OVERRIDE
         {
             Talk(SAY_SLAY);
         }

        void UpdateAI(uint32 diff) OVERRIDE
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (WhirlWind)
            {
                if (WhirlWindRandom_Timer <= diff)
                {
                    //Attack random Gamers
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true))
                    {
                        me->AddThreat(target, 1.0f);
                        me->TauntApply(target);
                        AttackStart(target);
                    }
                    WhirlWindRandom_Timer = std::rand() % 7000 + 3000;
                } else WhirlWindRandom_Timer -= diff;

                if (WhirlWindEnd_Timer <= diff)
                {
                    WhirlWind = false;
                    WhirlWind_Timer = std::rand() % 40000 + 25000;
                } else WhirlWindEnd_Timer -= diff;
            }

            if (!WhirlWind)
            {
                if (WhirlWind_Timer <= diff)
                {
                    DoCast(me, SPELL_WHIRLWIND);
                    WhirlWind = true;
                    WhirlWindEnd_Timer = 15000;
                } else WhirlWind_Timer -= diff;

                if (AggroReset_Timer <= diff)
                {
                    //Attack random Gamers
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true))
                    {
                        me->AddThreat(target, 1.0f);
                        me->TauntApply(target);
                        AttackStart(target);
                    }
                    AggroReset = true;
                    AggroReset_Timer = std::rand() % 5000 + 2000;
                } else AggroReset_Timer -= diff;

                if (AggroReset)
                {
                    if (AggroResetEnd_Timer <= diff)
                    {
                        AggroReset = false;
                        AggroResetEnd_Timer = 5000;
                        AggroReset_Timer = std::rand() % 45000 + 35000;
                    } else AggroResetEnd_Timer -= diff;
                }

                //If she is 20% enrage
                if (!Enraged)
                {
                    if (!HealthAbovePct(20) && !me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_ENRAGE);
                        Enraged = true;
                    }
                }

                //After 10 minutes hard enrage
                if (!EnragedHard)
                {
                    if (EnrageHard_Timer <= diff)
                    {
                        DoCast(me, SPELL_ENRAGEHARD);
                        EnragedHard = true;
                    } else EnrageHard_Timer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        }
    };
};

class npc_sartura_royal_guard : public CreatureScript
{
public:
    npc_sartura_royal_guard() : CreatureScript("npc_sartura_royal_guard") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new npc_sartura_royal_guardAI(creature);
    }

    struct npc_sartura_royal_guardAI : public ScriptedAI
    {
        npc_sartura_royal_guardAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 WhirlWind_Timer;
        uint32 WhirlWindRandom_Timer;
        uint32 WhirlWindEnd_Timer;
        uint32 AggroReset_Timer;
        uint32 AggroResetEnd_Timer;
        uint32 KnockBack_Timer;

        bool WhirlWind;
        bool AggroReset;

        void Reset() OVERRIDE
        {
            WhirlWind_Timer = 30000;
            WhirlWindRandom_Timer = std::rand() % 7000 + 3000;
            WhirlWindEnd_Timer = 15000;
            AggroReset_Timer = std::rand() % 55000 + 45000;
            AggroResetEnd_Timer = 5000;
            KnockBack_Timer = 10000;

            WhirlWind = false;
            AggroReset = false;
        }

        void EnterCombat(Unit* /*who*/) OVERRIDE
        {
        }

        void UpdateAI(uint32 diff) OVERRIDE
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (!WhirlWind && WhirlWind_Timer <= diff)
            {
                DoCast(me, SPELL_WHIRLWINDADD);
                WhirlWind = true;
                WhirlWind_Timer = std::rand() % 40000 + 25000;
                WhirlWindEnd_Timer = 15000;
            } else WhirlWind_Timer -= diff;

            if (WhirlWind)
            {
                if (WhirlWindRandom_Timer <= diff)
                {
                    //Attack random Gamers
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true))
                    {
                        me->AddThreat(target, 1.0f);
                        me->TauntApply(target);
                        AttackStart(target);
                    }

                    WhirlWindRandom_Timer = std::rand() % 7000 + 3000;
                } else WhirlWindRandom_Timer -= diff;

                if (WhirlWindEnd_Timer <= diff)
                {
                    WhirlWind = false;
                } else WhirlWindEnd_Timer -= diff;
            }

            if (!WhirlWind)
            {
                if (AggroReset_Timer <= diff)
                {
                    //Attack random Gamers
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true))
                    {
                        me->AddThreat(target, 1.0f);
                        me->TauntApply(target);
                        AttackStart(target);
                    }

                    AggroReset = true;
                    AggroReset_Timer = std::rand() % 5000 + 2000;
                } else AggroReset_Timer -= diff;

                if (KnockBack_Timer <= diff)
                {
                    DoCast(me, SPELL_WHIRLWINDADD);
                    KnockBack_Timer = std::rand() % 20000 + 10000;
                } else KnockBack_Timer -= diff;
            }

            if (AggroReset)
            {
                if (AggroResetEnd_Timer <= diff)
                {
                    AggroReset = false;
                    AggroResetEnd_Timer = 5000;
                    AggroReset_Timer = std::rand() % 40000 + 30000;
                } else AggroResetEnd_Timer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_sartura()
{
    new boss_sartura();
    new npc_sartura_royal_guard();
}
