#include "ScriptMgr.h"
#include "Item.h"
#include "ScriptedGossip.h"
#include "LFGMgr.h"
#include "MiscPackets.h"

//180653 item id
class item_challenge_key : public ItemScript
{
public:
    item_challenge_key() : ItemScript("item_challenge_key") { }

    bool OnCreate(Player* player, Item* item) override
    {
        if (player->HasItemCount(180653, 1, false))
        {
            player->InitChallengeKey(item);
            item->SetState(ITEM_CHANGED);
        }
        else
        {
            player->CreateChallengeKey(item);
            item->SetState(ITEM_CHANGED);
        }
        return false;
    }
};

void AddSCitem_scripts()
{
    new item_challenge_key();
}
