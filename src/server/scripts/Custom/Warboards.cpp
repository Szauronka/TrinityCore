#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectMgr.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "ScriptMgr.h"

// Alliance gob: 281339  |  Horde gob: 281340
struct go_warboard : public GameObjectAI
{
    go_warboard(GameObject* go) : GameObjectAI(go) { }

    bool OnReportUse(Player* player)
    {
        if (player->GetLevel() < 10)
            return false;

        if (player->GetTeam() == ALLIANCE)
            //me->CastSpell(player, 261654, true);
            player->SendPlayerChoice(me->GetGUID(), 352);

        if (player->GetTeam() == HORDE)
            //me->CastSpell(player, 261655, true);
            player->SendPlayerChoice(me->GetGUID(), 342);

        return true;
    }

};

void AddSC_Warboard_Scripts()
{
    RegisterGameObjectAI(go_warboard);
}
