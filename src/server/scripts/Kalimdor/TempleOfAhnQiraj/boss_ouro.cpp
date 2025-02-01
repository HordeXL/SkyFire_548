/*
* This file is part of Project SkyFire https://www.projectskyfire.org. 
* See LICENSE.md file for Copyright information
*/

/* ScriptData
SDName: Boss_Ouro
SD%Complete: 85
SDComment: No model for submerging. Currently just invisible.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "temple_of_ahnqiraj.h"

enum Spells
{
    SPELL_SWEEP                 = 26103,
    SPELL_SANDBLAST             = 26102,
    SPELL_GROUND_RUPTURE        = 26100,
    SPELL_BIRTH                 = 26262, // The Birth Animation
    SPELL_DIRTMOUND_PASSIVE     = 26092
};

class boss_ouro : public CreatureScript
{
public:
    boss_ouro() : CreatureScript("boss_ouro") { }

    CreatureAI* GetAI(Creature* creature) const OVERRIDE
    {
        return new boss_ouroAI(creature);
    }

    struct boss_ouroAI : public ScriptedAI
    {
        boss_ouroAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 Sweep_Timer;
        uint32 SandBlast_Timer;
        uint32 Submerge_Timer;
        uint32 Back_Timer;
        uint32 ChangeTarget_Timer;
        uint32 Spawn_Timer;

        bool Enrage;
        bool Submerged;

        void Reset() OVERRIDE
        {
            Sweep_Timer = std::rand() % 10000 + 5000;
            SandBlast_Timer = std::rand() % 35000 + 20000;
            Submerge_Timer = std::rand() % 150000 + 90000;
            Back_Timer = std::rand() % 45000 + 30000;
            ChangeTarget_Timer = std::rand() % 8000 + 5000;
            Spawn_Timer = std::rand() % 20000 + 10000;

            Enrage = false;
            Submerged = false;
        }

        void EnterCombat(Unit* /*who*/) OVERRIDE
        {
            DoCastVictim(SPELL_BIRTH);
        }

        void UpdateAI(uint32 diff) OVERRIDE
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            //Sweep_Timer
            if (!Submerged && Sweep_Timer <= diff)
            {
                DoCastVictim(SPELL_SWEEP);
                Sweep_Timer = std::rand() % 30000 + 15000;
            }
            else Sweep_Timer -= diff;

            //SandBlast_Timer
            if (!Submerged && SandBlast_Timer <= diff)
            {
                DoCastVictim(SPELL_SANDBLAST);
                SandBlast_Timer = std::rand() % 35000 + 20000;
            }
            else SandBlast_Timer -= diff;

            //Submerge_Timer
            if (!Submerged && Submerge_Timer <= diff)
            {
                //Cast
                me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->setFaction(35);
                DoCast(me, SPELL_DIRTMOUND_PASSIVE);

                Submerged = true;
                Back_Timer = std::rand() % 45000 + 30000;
            }
            else Submerge_Timer -= diff;

            //ChangeTarget_Timer
            if (Submerged && ChangeTarget_Timer <= diff)
            {
                Unit* target = NULL;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0);

                if (target)
                    DoTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

                ChangeTarget_Timer = std::rand() % 20000 + 10000;
            }
            else ChangeTarget_Timer -= diff;

            //Back_Timer
            if (Submerged && Back_Timer <= diff)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->setFaction(14);

                DoCastVictim(SPELL_GROUND_RUPTURE);

                Submerged = false;
                Submerge_Timer = std::rand() % 120000 + 60000;
            }
            else Back_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_ouro()
{
    new boss_ouro();
}
