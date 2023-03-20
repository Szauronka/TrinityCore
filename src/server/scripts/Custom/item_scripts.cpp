#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "LFGMgr.h"
#include "MiscPackets.h"

//180653 item id
class item_challenge_key : public ItemScript
{
public:
    item_challenge_key() : ItemScript("item_challenge_key") { }

    bool OnCreate(Player* player, Item* item)
    {
        if (player->HasItemCount(158923, 1, true))
            player->InitChallengeKey(item);
        else
            player->CreateChallengeKey(item);
        return false;
    }
};

void AddSCitem_scripts()
{
    new item_challenge_key();
}
