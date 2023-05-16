#include "WorldSession.h"
#include "Player.h"
#include "Group.h"
#include "GameObject.h"
#include "Item.h"
#include "MythicPlusPacketsCommon.h"
#include "InstanceScript.h"
#include <Map.h>
#include <ObjectAccessor.h>
#include <ChallengeModeMgr.h>


void WorldSession::HandleMythicPlusRequestMapStats(WorldPackets::MythicPlus::MythicPlusRequestMapStats& /*mythicPlusRequestMapStats*/)
{
    WorldPackets::MythicPlus::MythicPlusRequestMapStatsResult result;

    SendPacket(result.Write());
}

void WorldSession::SendMythicPlusMapStatsUpdate(uint32 keyID)
{
    WorldPacket packet(6 * 4);

    for (auto const& l_ChallengeData : _player->m_CompletedChallenges)
    {
        if (l_ChallengeData.first == keyID)
        {
            CompletedChallenge l_CompletedChallenge = l_ChallengeData.second;

            packet << uint32(l_CompletedChallenge.MapID);
            packet << uint32(l_ChallengeData.first);
            packet << uint32(l_CompletedChallenge.BestCompletion);
            packet << uint32(l_CompletedChallenge.LastCompletion);
            packet << uint32(l_CompletedChallenge.Medal);
            packet << uint32(l_CompletedChallenge.MedalDate);

            uint32 l_SpecCount = 5;
            packet << uint32(l_SpecCount);

            for (uint32 I = 0; I < 3; ++I)
                packet << uint32(0);

            for (uint32 J = 0; J < l_SpecCount; ++J)
                packet << uint16(0);    ///pecID
        }
    }

    SendPacket(&packet);
}

void WorldSession::HandleMythicPlusCurrentAffixes(WorldPackets::MythicPlus::MythicPlusCurrentAffixes& /*mythicPlusCurrentAffixes*/)
{
    WorldPackets::MythicPlus::MythicPlusCurrentAffixesResult result;
    ChallengeData affix;
    result.Count = 4;

    result.RequiredSeason[0] = 9;
    result.RequiredSeason[1] = 0;
    result.RequiredSeason[2] = 9;
    result.RequiredSeason[3] = 3;



    result.Affixes[0] = sWorld->getWorldState(WS_CHALLENGE_AFFIXE1_RESET_TIME);
    result.Affixes[1] = sWorld->getWorldState(WS_CHALLENGE_AFFIXE2_RESET_TIME);
    result.Affixes[2] = sWorld->getWorldState(WS_CHALLENGE_AFFIXE3_RESET_TIME);
    result.Affixes[3] = sWorld->getWorldState(WS_CHALLENGE_AFFIXE4_RESET_TIME);


    SendPacket(result.Write());
}

void WorldSession::HandleMythicPlusSeasonData(WorldPackets::MythicPlus::MythicPlusSeasonData& /*mythicPlusSeasonData*/)
{
    WorldPackets::MythicPlus::MythicPlusSeasonDataResult result;

    SendPacket(result.Write());
}

void WorldSession::HandleResetChallengeMode(WorldPackets::MythicPlus::ResetChallengeMode& /*packet*/)
{
    if (auto const& instanceScript = _player->GetInstanceScript())
        if (instanceScript->instance->IsMythic())
            instanceScript->ResetChallengeMode();
}

void WorldSession::HandleStartChallengeMode(WorldPackets::MythicPlus::StartChallengeMode& packet)
{

    GameObject* object = _player->GetGameObjectIfCanInteractWith(packet.GameObjectGUID, GAMEOBJECT_TYPE_KEYSTONE_RECEPTACLE);
    if (!object)
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - {} not found or you can not interact with it.", packet.GameObjectGUID.ToString().c_str());
        return;
    }

    Item* key = _player->GetItemByPos(packet.Bag, packet.Slot);
    if (!key)
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - item in Bag {} and Slot {} not found.", packet.Bag, packet.Slot);
        return;
    }

    if (key->GetTemplate()->GetClass() != ITEM_CLASS_REAGENT || key->GetTemplate()->GetSubClass() != ITEM_SUBCLASS_KEYSTONE)
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - Tried to start a challenge with item {} which have class {} and subclass {}.",
            key->GetGUID().ToString().c_str(),
            key->GetTemplate()->GetClass(),
            key->GetTemplate()->GetSubClass());
        return;
    }

    uint32 challengeModeId = key->GetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID);
    uint32 challengeModeLevel = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL);
    uint32 challengeModeAffix1 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1);
    uint32 challengeModeAffix2 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2);
    uint32 challengeModeAffix3 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3);
    uint32 challengeModeAffix4 = key->GetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_4);

    MapChallengeModeEntry const* entry = sMapChallengeModeStore.LookupEntry(challengeModeId);
    if (!entry || !challengeModeLevel || entry->MapID != _player->GetMapId())
    {
        TC_LOG_INFO("server.info", "WORLD: HandleChallengeModeStart - Tried to start a challenge with wrong challengeModeId {} and level {}.", challengeModeId, challengeModeLevel);
        return;
    }

    if (InstanceScript* instanceScript = _player->GetInstanceScript())
        instanceScript->StartChallengeMode(challengeModeId, challengeModeLevel, challengeModeAffix1, challengeModeAffix2, challengeModeAffix3, challengeModeAffix4);

    // Blizzard do not delete the key at challenge start, will require mort research
    _player->DestroyItem(packet.Bag, packet.Slot, true);
}

void WorldSession::HandleRequestLeaders(WorldPackets::MythicPlus::RequestLeaders& packet)
{
    WorldPackets::MythicPlus::RequestLeadersResult result;
    result.MapID = packet.MapId;
    result.ChallengeID = packet.ChallengeID;

    result.LastGuildUpdate = time(nullptr);
    result.LastRealmUpdate = time(nullptr);

    if (auto bestGuild = sChallengeModeMgr->BestGuildChallenge(_player->GetGuildId(), packet.ChallengeID))
    {
        for (auto itr = bestGuild->member.begin(); itr != bestGuild->member.end(); ++itr)
        {
            WorldPackets::MythicPlus::ModeAttempt guildLeaders;
            guildLeaders.InstanceRealmAddress = GetVirtualRealmAddress();
            guildLeaders.AttemptID = bestGuild->ID;
            guildLeaders.CompletionTime = bestGuild->RecordTime;
            guildLeaders.CompletionDate = bestGuild->Date;
            guildLeaders.MedalEarned = bestGuild->ChallengeLevel;

            for (auto const& v : bestGuild->member)
            {
                WorldPackets::MythicPlus::ModeAttempt::Member memberData;
                memberData.VirtualRealmAddress = GetVirtualRealmAddress();
                memberData.NativeRealmAddress = GetVirtualRealmAddress();
                memberData.Guid = v.guid;
                memberData.SpecializationID = v.specId;
                guildLeaders.Members.emplace_back(memberData);
            }

            result.GuildLeaders.emplace_back(guildLeaders);
        }
    }

    if (ChallengeData* bestServer = sChallengeModeMgr->BestServerChallenge(packet.ChallengeID))
    {
        WorldPackets::MythicPlus::ModeAttempt realmLeaders;
        realmLeaders.InstanceRealmAddress = GetVirtualRealmAddress();
        realmLeaders.AttemptID = bestServer->ID;
        realmLeaders.CompletionTime = bestServer->RecordTime;
        realmLeaders.CompletionDate = bestServer->Date;
        realmLeaders.MedalEarned = bestServer->ChallengeLevel;

        for (auto const& v : bestServer->member)
        {
            WorldPackets::MythicPlus::ModeAttempt::Member memberData;
            memberData.VirtualRealmAddress = GetVirtualRealmAddress();
            memberData.NativeRealmAddress = GetVirtualRealmAddress();
            memberData.Guid = v.guid;
            memberData.SpecializationID = v.specId;
            realmLeaders.Members.emplace_back(memberData);
        }
        result.RealmLeaders.push_back(realmLeaders);
    }

    SendPacket(result.Write());
}

