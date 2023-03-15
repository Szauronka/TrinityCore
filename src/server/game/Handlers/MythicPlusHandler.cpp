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


void WorldSession::HandleMythicPlusRequestMapStats(WorldPackets::MythicPlus::MythicPlusRequestMapStats& mythicPlusRequestMapStats)
{
    WorldPackets::MythicPlus::MythicPlusRequestMapStatsResult result;

    SendPacket(result.Write());
}

void WorldSession::HandleMythicPlusCurrentAffixes(WorldPackets::MythicPlus::MythicPlusCurrentAffixes& mythicPlusCurrentAffixes)
{
    WorldPackets::MythicPlus::MythicPlusCurrentAffixesResult result;

    result.Count = 5;

    result.Affixes[0] = 9;
    result.Affixes[1] = 8;
    result.Affixes[2] = 124;
    result.Affixes[3] = 121;
    result.Affixes[4] = 128;

    result.RequiredSeason[0] = 0;
    result.RequiredSeason[1] = 0;
    result.RequiredSeason[2] = 0;
    result.RequiredSeason[3] = 5;
    result.RequiredSeason[4] = 6;


    result.Affixes[0];
    result.Affixes[1];
    result.Affixes[2];
    result.Affixes[3];
    result.Affixes[4];

    SendPacket(result.Write());
}

void WorldSession::HandleMythicPlusSeasonData(WorldPackets::MythicPlus::MythicPlusSeasonData& mythicPlusSeasonData)
{
    WorldPackets::MythicPlus::MythicPlusSeasonDataResult result;

    SendPacket(result.Write());
}

void WorldSession::HandleResetChallengeMode(WorldPackets::MythicPlus::ResetChallengeMode& packet)
{
    if (auto const& instanceScript = _player->GetInstanceScript())
        if (instanceScript->instance->IsMythic())
            instanceScript->ResetChallengeMode();
}

void WorldSession::HandleStartChallengeMode(WorldPackets::MythicPlus::StartChallengeMode& packet)
{
    //if (packet.GameObjectGUID.GetEntry() != ChallengeModeOrb)
    //    return;

    if (Item* item = _player->GetItemByEntry(180653))
    {
        if (Group* group = _player->GetGroup())
        {
            InstanceMap* inst = _player->GetMap()->ToInstanceMap();
            if (!inst)
            {
                // ChatHandler(_player).PSendSysMessage("Error: Is not a Instance Map.");
                return;
            }

            /*   if (inst->GetSpawnMode() == DIFFICULTY_MYTHIC_KEYSTONE)
               {
                   ChatHandler(player).PSendSysMessage("Error: For run Mythic please rerun instance.");
                   return;
               }*/

            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                Player* player = itr->GetSource();
                if (!player || !player->IsAlive())
                {
                    // ChatHandler(_player).PSendSysMessage("Error: Player not found or die.");
                    return;
                }

                if (player->IsInCombat())
                {
                    //  ChatHandler(_player).PSendSysMessage("Error: Player in combat.");
                    return;
                }

                if (!player->GetMap() || player->GetMap()->ToInstanceMap() != inst)
                {
                    //   ChatHandler(_player).PSendSysMessage("Error: Player in group not this map.");
                    return;
                }
            }

            if (_player->m_challengeKeyInfo.InstanceID)
            {
                //  ChatHandler(_player).PSendSysMessage("Error: Key allready run in other instance.");
                return;
            }

            if (_player->m_challengeKeyInfo.Level < 2)
            {
                //  ChatHandler(_player).PSendSysMessage("Error: Key is bugged.");
                return;
            }

            /*  float x = 0.0f; float y = 0.0f; float z = 0.0f; float o = 0.0f;
              if (!sChallengeModeMgr->GetStartPosition(_player->GetMapId(), x, y, z, o, _player->GetGUID()))
              {
                  ChatHandler(_player).PSendSysMessage("Error: Start position not found.");
                  return;
              }*/

            group->m_challengeEntry = sMapChallengeModeStore.LookupEntry(_player->m_challengeKeyInfo.ID);
            group->m_affixes.fill(0);

            MapChallengeModeEntry const* challengeEntry = sDB2Manager.GetChallengeModeByMapID(_player->GetMapId());
            if (!group->m_challengeEntry || !challengeEntry || challengeEntry->MapID != group->m_challengeEntry->MapID)
            {
                group->m_challengeEntry = nullptr;
                // ChatHandler(_player).PSendSysMessage("Error: Is not this challenge.");
                return;
            }

            group->m_challengeOwner = _player->GetGUID();
            group->m_challengeItem = item->GetGUID();
            group->m_challengeLevel = _player->m_challengeKeyInfo.Level;
            //group->m_noLoot = !_player->m_challengeKeyInfo.KeyIsCharded;

            if (group->m_challengeLevel > 3)
                group->m_affixes[0] = _player->m_challengeKeyInfo.Affix;
            if (group->m_challengeLevel > 6)
                group->m_affixes[1] = _player->m_challengeKeyInfo.Affix1;
            if (group->m_challengeLevel > 9)
                group->m_affixes[2] = _player->m_challengeKeyInfo.Affix2;

            // WorldPackets::Instance::ChangePlayerDifficultyResult result;
            // result.Result = AsUnderlyingType(ChangeDifficultyResult::DIFFICULTY_CHANGE_SET_COOLDOWN_S);
            // result.CooldownReason = 2813862382;
           //  group->BroadcastPacket(result.Write(), true);

            // WorldPackets::Instance::ChangePlayerDifficultyResult result2;
            // result2.Result = AsUnderlyingType(ChangeDifficultyResult::DIFFICULTY_CHANGE_BY_PARTY_LEADER);
           //  result2.InstanceMapID = _player->GetMapId();
           //  result2.DifficultyRecID = DIFFICULTY_MYTHIC_KEYSTONE;

            group->SetDungeonDifficultyID(DIFFICULTY_MYTHIC_KEYSTONE);

            for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            {
                if (Player* player = itr->GetSource())
                {
                    player->SetDungeonDifficultyID(DIFFICULTY_MYTHIC_KEYSTONE);
                    // player->SendDirectMessage(result2.Write());
                   //  player->TeleportToChallenge(_player->GetMapId(), x, y, z, o);
                  //   player->CastSpell(player, ChallengeSpells::ChallengersBurden, true);
                }
            }
            if (GameObject* challengeOrb = ObjectAccessor::FindGameObject(packet.GameObjectGUID))
            {
                challengeOrb->SetGoState(GOState::GO_STATE_ACTIVE);
                challengeOrb->SetFlag(GameObjectFlags::GO_FLAG_NODESPAWN);
            }
        }
        // else
           //  ChatHandler(player).PSendSysMessage("Error: You need group for run challenge");
    }
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

