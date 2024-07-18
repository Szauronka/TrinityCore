/*
 * Copyright 2021 ShadowCore
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

#include "ChallengeModeMgr.h"
#include "Containers.h"
#include "DB2Stores.h"
#include "GameEventMgr.h"
#include "GameTables.h"
#include "GameObject.h"
#include "Item.h"
#include "ItemBonusMgr.h"
#include "LootPackets.h"
#include "InstanceScript.h"
#include "StringConvert.h"
#include "Map.h"
#include "MythicPlusPacketsCommon.h"
#include "WorldSession.h"
#include <sstream>
#include <ObjectAccessor.h>


ChallengeModeMgr* ChallengeModeMgr::instance()
{
    static ChallengeModeMgr instance;
    return &instance;
}

MapChallengeModeEntry const* ChallengeModeMgr::GetMapChallengeModeEntry(uint32 mapId)
{
    for (uint32 i = 0; i < sMapChallengeModeStore.GetNumRows(); ++i)
        if (MapChallengeModeEntry const* challengeModeEntry = sMapChallengeModeStore.LookupEntry(i))
            if (challengeModeEntry->MapID == mapId)
                return challengeModeEntry;

    return nullptr;
}

bool ChallengeMember::operator<(const ChallengeMember& i) const
{
    return guid.GetCounter() > i.guid.GetCounter();
}

uint32 ChallengeModeMgr::GetChallengeLevel() const
{
    return std::min(_challengeLevel, sWorld->getIntConfig(CONFIG_CHALLENGE_LEVEL_LIMIT));
}

bool ChallengeModeMgr::HasOploteLoot(ObjectGuid const& guid)
{
    return Trinity::Containers::MapGetValuePtr(_oploteWeekLoot, guid);
}

OploteLoot* ChallengeModeMgr::GetOploteLoot(ObjectGuid const& guid)
{
    return Trinity::Containers::MapGetValuePtr(_oploteWeekLoot, guid);
}

uint32 ChallengeModeMgr::GetCAForLoot(ChallengeModeMgr* const challenge)
{
    if (!challenge)
        return 0;

    switch (challenge->_mapID)
    {
    case 1754: //freehold 
    {
        // Lesser Dungeons
        switch (challenge->GetChallengeLevel())
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 147398;
        case 4:
        case 5:
        case 6:
            return 147399;
        case 7:
        case 8:
        case 9:
            return 147400;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 147401;
        case 15:
        default:
            return 147719;
        }
    }
    case 1771: //tol dagor             
    case 1841: //underrot              
    case 1762: //king´s rest             
    case 2097: //op mechagon           
    case 1822: //siege of boralus  
    case 1862: //waycrest manor     
    case 1864: //shrine of the storm   
    {
        // Regular Dungeons
        switch (challenge->GetChallengeLevel())
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 147402;
        case 4:
        case 5:
        case 6:
            return 147403;
        case 7:
        case 8:
        case 9:
            return 147404;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 147405;
        case 15:
        default:
            return 147718;
        }
    }
    case 1877: //temple of serthraliss 
    case 1594: //the motherlode!!      
    {
        // Greater Dungeons
        switch (challenge->GetChallengeLevel())
        {
        case 0:
        case 1:
            return 0;
        case 2:
        case 3:
            return 147406;
        case 4:
        case 5:
        case 6:
            return 147407;
        case 7:
        case 8:
        case 9:
            return 147408;
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            return 147409;
        case 15:
        default:
            return 147720;
        }
    }
    default:
        break;
    }

    return 0;
}

uint32 ChallengeModeMgr::GetBigCAForLoot(ChallengeModeMgr* const challenge, uint32& count)
{
    if (!challenge || challenge->GetChallengeLevel() <= 10)
        return 0;

    if (challenge->GetChallengeLevel() >= 15)
        count = challenge->GetChallengeLevel() - 15;
    else
        count = challenge->GetChallengeLevel() - 10;

    switch (challenge->_mapID)
    {
    case 1754: //freehold 
    {
        // Lesser Dungeons
        return 147808; // Lesser Adept's Spoils
    }
    case 1771: //tol dagor            
    case 1841: //underrot             
    case 1762: //king´s rest          
    case 2097: //op mechagon          
    case 1864: //shrine of the storm  
    case 1822: //siege of boralus  
    case 1862: //waycrest manor     
    {
        // Regular Dungeons
        return 147809; // Adept's Spoils
    }
    case 1877: //temple of serthraliss 
    case 1594: //the motherlode!!      
    {
        // Greater Dungeons
        return 147810; // Greater Adept's Spoils
    }
    case 2516: // The Nokhud Offensive
    case 2515: // The Azure Vault
    case 960: // Temple Of The Jade Serpent
    case 1176: // Shadowmoon Burial Grounds
    case 2521: // Ruby Life Pools
    case 1477: // Halls Of Valor
    case 1571: // Court of Stars
    case 2526: // Algeth'ar Academy
    default:
        break;
    }

    return 0;
}

uint32 ChallengeModeMgr::GetCAForOplote(uint32 challengeLevel)
{
    switch (challengeLevel)
    {
        // Is bug???
    case 0:
    case 1:
        return 0;
    case 2:
    case 3:
        return 147551;
    case 4:
    case 5:
    case 6:
        return 147550;
    case 7:
    case 8:
    case 9:
        return 147549;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return 147548;
    default: // 15+
        return 147721;
    }
}


uint32 ChallengeModeMgr::GetBigCAForOplote(uint32 challengeLevel, uint32& count)
{
    if (challengeLevel <= 10)
        return 0;

    if (challengeLevel >= 15)
        count = challengeLevel - 15;
    else
        count = challengeLevel - 10;

    return 147819;
}

bool ChallengeMember::operator==(const ChallengeMember& i) const
{
    return guid.GetCounter() == i.guid.GetCounter();
}

MapChallengeModeEntry const* ChallengeModeMgr::GetMapChallengeModeEntryByModeId(uint32 modeId)
{
    for (uint32 i = 0; i < sMapChallengeModeStore.GetNumRows(); ++i)
        if (MapChallengeModeEntry const* challengeModeEntry = sMapChallengeModeStore.LookupEntry(i))
            if (challengeModeEntry->ID == modeId)
                return challengeModeEntry;

    return nullptr;
}

uint8 ChallengeModeMgr::GetActiveAffixe()
{
    if (sGameEventMgr->IsActiveEvent(126))
        return 0;
    if (sGameEventMgr->IsActiveEvent(127))
        return 1;
    if (sGameEventMgr->IsActiveEvent(128))
        return 2;
    if (sGameEventMgr->IsActiveEvent(129))
        return 3;
    if (sGameEventMgr->IsActiveEvent(130))
        return 4;
    if (sGameEventMgr->IsActiveEvent(131))
        return 5;
    if (sGameEventMgr->IsActiveEvent(132))
        return 6;
    if (sGameEventMgr->IsActiveEvent(133))
        return 7;
    if (sGameEventMgr->IsActiveEvent(134))
        return 8;
    if (sGameEventMgr->IsActiveEvent(135))
        return 9;
    if (sGameEventMgr->IsActiveEvent(136))
        return 10;
    if (sGameEventMgr->IsActiveEvent(137))
        return 11;

    return 0;
}


uint32 ChallengeModeMgr::GetDamageMultiplier(uint8 challengeLevel)
{
    if (GtChallengeModeDamageEntry const* challengeDamage = sChallengeModeDamage.GetRow(challengeLevel))
        return uint32(100.f * (challengeDamage->Scalar - 1.f));

    return 1;
}

uint32 ChallengeModeMgr::GetHealthMultiplier(uint8 challengeLevel)
{
    if (GtChallengeModeHealthEntry const* challengeHealth = sChallengeModeHealth.GetRow(challengeLevel))
        return uint32(100.f * (challengeHealth->Scalar - 1.f));

    return 1;
}

ChallengeData* ChallengeModeMgr::BestServerChallenge(uint16 ChallengeID)
{
   return Trinity::Containers::MapGetValuePtr(_bestForMap, ChallengeID);
}

InstanceScript* ChallengeModeMgr::GetInstanceScript() const
{
    return _instanceScript;
}

float ChallengeModeMgr::GetChanceItem(uint8 mode, uint32 challengeLevel)
{
    float base_chance = 200.0f;
    float add_chance = 0.0f;

    if (challengeLevel > 10)
        add_chance += (challengeLevel - 10) * 40.0f;

    switch (mode)
    {
    case CHALLENGE_TIMER_LEVEL_3: /// 3 chests + 3 levels
    case CHALLENGE_TIMER_LEVEL_2: /// 2 chests + 2 levels
    case CHALLENGE_TIMER_LEVEL_1: /// 1 chest + 1 level
        base_chance += 100.0f; // 300% is 3 items
        break;
    case CHALLENGE_NOT_IN_TIMER:  /// 0 chest
        base_chance += 0.0f; // 200% is 2 items
        break;
    default:
        break;
    }

    base_chance += add_chance;

    return base_chance;
}

bool ChallengeModeMgr::IsChest(uint32 goEntry)
{
    switch (goEntry)  
    {
    case 282736: // Tol Dagor
    case 282737: // Atal'Dazar
    case 288642: // King's Rest
    case 288644: // Siege of Boralus
    case 290544: // The MOTHERLODE!!
    case 290621: // Waycrest Manor
    case 290758: // Temple of Sertraliss
    case 290759: // The Underrot
    case 290761: // Shrine of the Storm
    case 282735: // Freehold
    case 381955: // Ruby Life Pools
    case 381946: // The Azure Vault
    case 381966: // The Nokhud Offensive
    case 381972: // Temple of the Jade Serpent
    case 381971: // Shadowmoon Burial Grounds
    case 381969: // Halls Of Valor
    case 252062: // Halls Of Valor Superior
    case 381970: // Court Of Stars
    case 252687: // Court Of Stars Superior
    case 252688: // Court Of Stars Peerless
         return true;
    default:
        break;
    }

    return false;
}

uint32 ChallengeModeMgr::GetChallengeTimer()
{
    if (!_challengeTimer)
        return 0;

    return _challengeTimer / IN_MILLISECONDS;
}

void ChallengeModeMgr::CheckBestMapId(ChallengeData* challengeData)
{
    if (!challengeData)
        return;

    if (!_bestForMap[challengeData->ChallengeID] || _bestForMap[challengeData->ChallengeID]->RecordTime > challengeData->RecordTime)
        _bestForMap[challengeData->ChallengeID] = challengeData;
}

void ChallengeModeMgr::CheckBestGuildMapId(ChallengeData* challengeData)
{
    if (!challengeData || !challengeData->GuildID)
        return;

    if (!m_GuildBest[challengeData->GuildID][challengeData->ChallengeID] || m_GuildBest[challengeData->GuildID][challengeData->ChallengeID]->RecordTime > challengeData->RecordTime)
        m_GuildBest[challengeData->GuildID][challengeData->ChallengeID] = challengeData;
}
void ChallengeModeMgr::SetChallengeMapData(ObjectGuid::LowType const& ID, ChallengeData* data)
{
    _challengeMap[ID] = data;
}

typedef std::set<ChallengeMember> ChallengeMemberList;

void ChallengeModeMgr::SaveChallengeToDB(ChallengeData const* challengeData)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE);
    stmt->setUInt32(0, challengeData->ID);
    stmt->setUInt64(1, challengeData->GuildID);
    stmt->setUInt32(2, challengeData->MapID);
    stmt->setUInt32(3, challengeData->ChallengeID);
    stmt->setUInt32(4, challengeData->RecordTime);
    stmt->setUInt32(5, challengeData->Date);
    stmt->setUInt8(6, challengeData->ChallengeLevel);
    stmt->setUInt8(7, challengeData->TimerLevel);
    std::ostringstream affixesListIDs;
    for (uint32 affixe : challengeData->Affixes)
        if (affixe)
            affixesListIDs << affixe << ' ';
    stmt->setString(8, affixesListIDs.str());
    stmt->setUInt32(12, challengeData->ChestID);
    trans->Append(stmt);

    for (auto const& v : challengeData->member)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE_MEMBER);
        stmt->setUInt32(0, challengeData->ID);
        stmt->setUInt64(1, v.guid.GetCounter());
        stmt->setUInt16(2, v.specId);
        stmt->setUInt32(3, v.ChallengeLevel);
        stmt->setUInt32(4, v.Date);
        stmt->setUInt32(5, v.ChestID);
        trans->Append(stmt);
    }

    CharacterDatabase.CommitTransaction(trans);
}

bool ChallengeModeMgr::CheckBestMemberMapId(ObjectGuid const& guid, ChallengeData* challengeData)
{
    bool isBest = false;
    if (!_challengesOfMember[guid][challengeData->ChallengeID] || _challengesOfMember[guid][challengeData->ChallengeID]->RecordTime > challengeData->RecordTime)
    {
        _challengesOfMember[guid][challengeData->ChallengeID] = challengeData;
        isBest = true;
    }

    if (!_lastForMember[guid][challengeData->ChallengeID] || _lastForMember[guid][challengeData->ChallengeID]->Date < challengeData->Date)
        _lastForMember[guid][challengeData->ChallengeID] = challengeData;

    _challengeWeekList[guid].insert(challengeData);

    return isBest;
}


void ChallengeModeMgr::HitTimer()
{
    if (GetChallengeTimer() < _chestTimers[2])
        _rewardLevel = CHALLENGE_TIMER_LEVEL_3; /// 3 chests + 3 levels
    else if (GetChallengeTimer() < _chestTimers[1])
        _rewardLevel = CHALLENGE_TIMER_LEVEL_2; /// 2 chests + 2 levels
    else if (GetChallengeTimer() < _chestTimers[0])
        _rewardLevel = CHALLENGE_TIMER_LEVEL_1; /// 1 chest + 1 level
    else
        _rewardLevel = CHALLENGE_NOT_IN_TIMER;

    if (!_map)
        return;

    for (auto const& guid : _challengers)
        _countItems[guid] = 0;

    switch (_rewardLevel)
    {
        case CHALLENGE_TIMER_LEVEL_3: /// 3 chests + 3 levels
        case CHALLENGE_TIMER_LEVEL_2: /// 2 chests + 2 levels
        case CHALLENGE_TIMER_LEVEL_1: /// 1 chest + 1 level
        case CHALLENGE_NOT_IN_TIMER:  /// 0 chest
        {
            if (InstanceScript* script = GetInstanceScript())
            {
                if (GameObject* chest = _map->GetGameObject(script->_challengeChest))
                    chest->SetRespawnTime(7 * DAY);

                float _chance = GetChanceItem(_rewardLevel, _challengeLevel);
                auto countItems = int32(_chance / 100.0f);
                _chance -= countItems * 100.0f;

                if (roll_chance_f(_chance))
                    countItems++;

                while (countItems > 0)
                {
                    auto& _tempList = _challengers;
                    //Trinity::Containers::RandomResize(_tempList, countItems);

                    for (auto const& guid : _tempList)
                    {
                        countItems--;
                        _countItems[guid] += 1;
                    }
                }
            }
        }
            break;
        default:
            break;
    }
}

void ChallengeModeMgr::ResetGo()
{
    if (InstanceScript* script = GetInstanceScript())
        for (ObjectGuid guid : script->_challengeDoorGuids)
        {
            GameObject* obj = ObjectAccessor::FindGameObject(guid);
            if (GameObject* challengeDoor = obj)
                challengeDoor->SetGoState(GO_STATE_READY);
        }
   
   if (InstanceScript* script = GetInstanceScript())
       if (GameObject* challengeOrb = _map->GetGameObject(script->_challengeOrbGuid))
       {
           challengeOrb->SetGoState(GO_STATE_READY);
           challengeOrb->RemoveFlag(GO_FLAG_NODESPAWN);
       }
}

ChallengeData* ChallengeModeMgr::BestGuildChallenge(ObjectGuid::LowType const& GuildID, uint16 ChallengeID)
{
    if (!GuildID)
        return nullptr;

    auto itr = m_GuildBest.find(GuildID);
    if (itr == m_GuildBest.end())
        return nullptr;

    return Trinity::Containers::MapGetValuePtr(itr->second, ChallengeID);
}
ChallengeByMap* ChallengeModeMgr::BestForMember(ObjectGuid const& guid)
{
   return Trinity::Containers::MapGetValuePtr(_challengesOfMember, guid);
}

ChallengeByMap* ChallengeModeMgr::LastForMember(ObjectGuid const& guid)
{
    return Trinity::Containers::MapGetValuePtr(_lastForMember, guid);
}

ChallengeData* ChallengeModeMgr::LastForMemberMap(ObjectGuid const& guid, uint32 ChallengeID)
{
    if (ChallengeByMap* _lastResalt = LastForMember(guid))
    {
        auto itr = _lastResalt->find(ChallengeID);
        if (itr != _lastResalt->end())
            return itr->second;
    }

    return nullptr;
}

ChallengeData* ChallengeModeMgr::BestForMemberMap(ObjectGuid const& guid, uint32 ChallengeID)
{
    if (ChallengeByMap* _lastResalt = BestForMember(guid))
    {
        auto itr = _lastResalt->find(ChallengeID);
        if (itr != _lastResalt->end())
            return itr->second;
    }

    return nullptr;
}


uint32 ChallengeModeMgr::GetRandomChallengeId(uint32 flags/* = 4*/)
{
    std::vector<uint32> challenges;

    // Season1 Dragonflight
    // 399 Ruby Life Pools, 402 Algeth'ar Academy , 401 The Azure Vault, 400 The Nokhud Offensive, 200 Halls of Valor, 210 Court of Stars, 165 Shadowmoon Burial Grounds, 2 Temple of the Jade Serpent
    //
    // Season 2 Dragonflight
    // 406 Halls of Infusion, 405 Brackenhide Hollow, 403 Uldaman: Legacy of Tyr, 404 Neltharus, 206 Neltharion's Lair, 245 Freehold, 251 Underrot, Vortex Pinacle
    for (uint32 i = 0; i < sMapChallengeModeStore.GetNumRows(); ++i)
        if (MapChallengeModeEntry const* challengeModeEntry = sMapChallengeModeStore.LookupEntry(i))
            if (challengeModeEntry->Flags & flags &&
                (challengeModeEntry->ID == 399 ||
				 challengeModeEntry->ID == 402 || 
				 challengeModeEntry->ID == 401 ||
				 challengeModeEntry->ID == 400 ||
				 challengeModeEntry->ID == 200 ||
				 challengeModeEntry->ID == 210 ||
				 challengeModeEntry->ID == 165 ||
				 challengeModeEntry->ID == 2  )) // Temp fix, only doable dungeons here
                        challenges.push_back(challengeModeEntry->ID);

        if (challenges.empty())
            return 0;
   
        return Trinity::Containers::SelectRandomContainerElement(challenges);
}

std::vector<int32> ChallengeModeMgr::GetBonusListIdsForRewards(uint32 baseItemIlevel, uint8 challengeLevel)
{
    if (challengeLevel < 2)
        return {};

    std::vector<std::pair<int32, uint32>> bonusDescriptionByChallengeLevel =
    {
        { 3410, 5  },   // Mythic 2
        { 3411, 5  },   // Mythic 3
        { 3412, 10 },   // Mythic 4
        { 3413, 15 },   // Mythic 5
        { 3414, 20 },   // Mythic 6
        { 3415, 20 },   // Mythic 7
        { 3416, 25 },   // Mythic 8
        { 3417, 25 },   // Mythic 9
        { 3418, 30 },   // Mythic 10
        { 3509, 35 },   // Mythic 11
        { 3510, 40 },   // Mythic 12
        { 3534, 45 },   // Mythic 13
        { 3535, 50 },   // Mythic 14
        { 3535, 55 },   // Mythic 15
    };

    const uint32 baseMythicIlevel = 885;
    std::pair<int32, uint32> bonusAndDeltaPair = bonusDescriptionByChallengeLevel[challengeLevel < 15 ? (challengeLevel - 2): 13];
    return { bonusAndDeltaPair.first, (int32)ItemBonusMgr::GetItemBonusListForItemLevelDelta(baseMythicIlevel - baseItemIlevel + bonusAndDeltaPair.second)};
}

void ChallengeModeMgr::Reward(Player* player, uint8 challengeLevel)
{
    if (!GetMapChallengeModeEntry(player->GetMapId()))
        return;

    JournalInstanceEntry const* journalInstance  = sDB2Manager.GetJournalInstanceByMapId(player->GetMapId());
    if (!journalInstance)
        return;

    auto encounters = sDB2Manager.GetJournalEncounterByJournalInstanceId(journalInstance->ID);
    if (!encounters)
        return;

    std::vector<JournalEncounterItemEntry const*> items;
    for (auto encounter : *encounters)
        if (std::vector<JournalEncounterItemEntry const*> const* journalItems = sDB2Manager.GetJournalItemsByEncounter(encounter->ID))
            items.insert(items.end(), journalItems->begin(), journalItems->end());

    if (items.empty())
        return;

    std::vector<ItemTemplate const*> stuffLoots;
    for (JournalEncounterItemEntry const* journalEncounterItem : items)
    {
        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(journalEncounterItem->ItemID);
        if (!itemTemplate)
            continue;

        if (!itemTemplate->IsUsableByLootSpecialization(player, false))
            continue;

        if (itemTemplate->GetInventoryType() != INVTYPE_NON_EQUIP)
            stuffLoots.push_back(itemTemplate);
    }

    ItemTemplate const* randomStuffItem = Trinity::Containers::SelectRandomContainerElement(stuffLoots);
    if (!randomStuffItem)
        return;

    uint32 itemId = randomStuffItem->GetId();
    ItemPosCountVec dest;
    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, 1);
    if (msg != EQUIP_ERR_OK)
    {
        player->SendEquipError(msg, nullptr, nullptr, itemId);
        return;
    }

    std::vector<int32> bonusListIds = GetBonusListIdsForRewards(randomStuffItem->GetBaseItemLevel(), challengeLevel);
    Item* pItem = player->StoreNewItem(dest, itemId, true, GenerateItemRandomBonusListId(itemId), GuidSet(), ItemContext::NONE, &bonusListIds);
    player->SendNewItem(pItem, 1, true, false, true);

    player->SendDisplayToast(itemId, DisplayToastType::NewItem, false, 1, DisplayToastMethod::Default, 0, pItem);
}

void ChallengeModeMgr::LoadFromDB()
{
    if (QueryResult result = CharacterDatabase.Query("SELECT `ID`, `GuildID`, `MapID`, ChallengeID, `RecordTime`, `Date`, `ChallengeLevel`, `TimerLevel`, `Affixes`, `ChestID`, `ChallengeID` FROM `challenge`"))
    {
        do
        {
            Field* fields = result->Fetch();

            auto challengeData = new ChallengeData;
            challengeData->ID = fields[0].GetUInt64();
            challengeData->GuildID = fields[1].GetUInt64();
            challengeData->MapID = fields[2].GetUInt32();
            challengeData->ChallengeID = fields[3].GetUInt32();
            challengeData->RecordTime = fields[4].GetUInt32();
            if (challengeData->RecordTime < 10000)
                challengeData->RecordTime *= IN_MILLISECONDS;
            challengeData->Date = fields[5].GetUInt32();
            challengeData->ChallengeLevel = fields[6].GetUInt8();
            challengeData->TimerLevel = fields[7].GetUInt8();
            challengeData->Affixes.fill(0);

            uint8 i = 0;
            for (std::string_view token : Trinity::Tokenize(fields[8].GetString().c_str(), ' ', false))
                if (Optional<int32> affix = Trinity::StringTo<int32>(token))
                    challengeData->Affixes[i] = *affix;
            challengeData->ChestID = fields[12].GetUInt32();

            if (!challengeData->ChallengeID)
                if (MapChallengeModeEntry const* challengeEntry = sDB2Manager.GetChallengeModeByMapID(challengeData->MapID))
                    challengeData->ChallengeID = challengeEntry->ID;

            _challengeMap[challengeData->ID] = challengeData;
            CheckBestMapId(challengeData);
            CheckBestGuildMapId(challengeData);

        } while (result->NextRow());
    }

    if (QueryResult result = CharacterDatabase.Query("SELECT `id`, `member`, `specID`, `ChallengeLevel`, `Date`, `ChestID` FROM `challenge_member`"))
    {
        do
        {
            Field* fields = result->Fetch();
            ChallengeMember member;
            member.guid = ObjectGuid::Create<HighGuid::Player>(fields[1].GetUInt64());
            member.specId = fields[2].GetUInt16();
            member.ChallengeLevel = fields[3].GetUInt32();
            member.Date = fields[4].GetUInt32();
            member.ChestID = fields[5].GetUInt32();

            auto itr = _challengeMap.find(fields[0].GetUInt64());
            if (itr == _challengeMap.end())
                continue;

            itr->second->member.insert(member);
            CheckBestMemberMapId(member.guid, itr->second);
        } while (result->NextRow());
    }

    for (auto v : _challengeMap)
        if (v.second->member.empty())
            CharacterDatabase.PQuery("DELETE FROM `challenge` WHERE `ID` = '{}';", v.first);


    if (QueryResult result = CharacterDatabase.Query("SELECT `guid`, `chestListID`, `date`, `ChallengeLevel` FROM `challenge_oplote_loot`"))
    {
        do
        {
            Field* fields = result->Fetch();
            ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(fields[0].GetUInt64());
            OploteLoot& lootOplote = _oploteWeekLoot[guid];
            lootOplote.Date = fields[2].GetUInt32();
            lootOplote.ChallengeLevel = fields[3].GetUInt32();
            lootOplote.needSave = false;
            lootOplote.guid = guid;

            for (std::string_view chestLists : Trinity::Tokenize(fields[1].GetString().c_str(), ' ', false))
                if (Optional<int32> chestList = Trinity::StringTo<int32>(chestLists))
                    lootOplote.chestListID.insert(*chestList);

        } while (result->NextRow());
    }

    if (sWorld->getWorldState(WS_CHALLENGE_AFFIXE1_RESET_TIME) == 0 || sWorld->getWorldState(WS_CHALLENGE_AFFIXE1_RESET_TIME) == 0 || sWorld->getWorldState(WS_CHALLENGE_AFFIXE1_RESET_TIME) == 0 || sWorld->getWorldState(WS_CHALLENGE_AFFIXE1_RESET_TIME) == 0)
        GenerateCurrentWeekAffixes();

    if ((sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX1) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX1) < 15) &&
        (sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX2) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX2) < 15) &&
        (sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX3) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX3) < 15) &&
        (sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX4) > 0 && sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX4) < 15))
        GenerateManualAffixes();
}


void ChallengeModeMgr::SaveOploteLootToDB()
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    for (auto const& v : _oploteWeekLoot)
    {
        if (v.second.needSave)
        {
            auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHALLENGE_OPLOTE_LOOT);
            stmt->setUInt32(0, v.second.guid.GetCounter());
            std::ostringstream chestLists;
            for (uint32 chestList : v.second.chestListID)
                if (chestList)
                    chestLists << chestList << ' ';
            stmt->setString(1, chestLists.str());
            stmt->setUInt32(2, v.second.Date);
            stmt->setUInt32(3, v.second.ChallengeLevel);
            trans->Append(stmt);
        }
    }
    CharacterDatabase.CommitTransaction(trans);
}

void ChallengeModeMgr::GenerateManualAffixes()
{
    sWorld->setWorldState(WS_CHALLENGE_AFFIXE1_RESET_TIME, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX1));
    sWorld->setWorldState(WS_CHALLENGE_AFFIXE2_RESET_TIME, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX2));
    sWorld->setWorldState(WS_CHALLENGE_AFFIXE3_RESET_TIME, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX3));
    sWorld->setWorldState(WS_CHALLENGE_AFFIXE4_RESET_TIME, sWorld->getIntConfig(CONFIG_CHALLENGE_MANUAL_AFFIX4));
}


void ChallengeModeMgr::GenerateCurrentWeekAffixes()
{
    // Season 1 DragonFlight
    uint32 affixes[10][4] =
    {
        { Fortified, Raging, Quaking, Thundering},
        { Tyrannical, Bursting, Grievous, Thundering},
        { Fortified, Sanguine, Volcanic, Thundering},
        { Tyrannical, Raging, Storming, Thundering},
        { Fortified, Spiteful, Grievous, Thundering},
        { Tyrannical, Sanguine, Explosive, Thundering},
        { Fortified, Bolstering, Storming, Thundering},
        { Tyrannical, Spiteful, Quaking, Thundering},
        { Fortified, Bursting, Explosive, Thundering},
        { Tyrannical, Bolstering, Volcanic, Thundering},
    };

    auto weekContainer = affixes[GetActiveAffixe()];

    sWorld->setWorldState(WS_CHALLENGE_AFFIXE1_RESET_TIME, weekContainer[0]);
    sWorld->setWorldState(WS_CHALLENGE_AFFIXE2_RESET_TIME, weekContainer[1]);
    sWorld->setWorldState(WS_CHALLENGE_AFFIXE3_RESET_TIME, weekContainer[2]);
    sWorld->setWorldState(WS_CHALLENGE_AFFIXE4_RESET_TIME, weekContainer[3]);
}

void ChallengeModeMgr::GenerateOploteLoot(bool manual)
{
    bool _manual = false;
    manual = _manual;
    TC_LOG_ERROR("misc", "GenerateOploteLoot manual {} _challengeWeekList {}", _manual, _challengeWeekList.size());

    CharacterDatabase.Query("DELETE FROM challenge_oplote_loot WHERE date <= UNIX_TIMESTAMP()");
    _oploteWeekLoot.clear();

    for (auto const& c : _challengeWeekList)
    {
        for (auto const& v : c.second)
        {
            if (manual && (v->Date > sWorld->getWorldState(WS_CHALLENGE_LAST_RESET_TIME) || v->Date < (sWorld->getWorldState(WS_CHALLENGE_LAST_RESET_TIME) - (7 * DAY))))
                continue;

            if (!manual && (v->Date > sWorld->getWorldState(WS_CHALLENGE_KEY_RESET_TIME) || v->Date < sWorld->getWorldState(WS_CHALLENGE_LAST_RESET_TIME)))
                continue;

            if (!v->ChestID)
                continue;

            auto itr = _oploteWeekLoot.find(c.first);
            if (itr != _oploteWeekLoot.end())
            {
                if (itr->second.ChallengeLevel < v->ChallengeLevel)
                    itr->second.ChallengeLevel = v->ChallengeLevel;

                itr->second.chestListID.insert(v->ChestID);
            }
            else
            {
                OploteLoot& lootOplote = _oploteWeekLoot[c.first];
                lootOplote.Date = sWorld->getNextChallengeKeyReset();
                lootOplote.ChallengeLevel = v->ChallengeLevel;
                lootOplote.needSave = true;
                lootOplote.guid = c.first;
                lootOplote.chestListID.insert(v->ChestID);
            }
        }
    }
    _challengeWeekList.clear();
    SaveOploteLootToDB();
}

uint32 ChallengeModeMgr::GetChest(uint32 challangeId)
{
    switch (challangeId)
    {
    case 375:
        return 354972; //Mists of Tirna Scithe                   
        break;
    case 376:
        return 354990; // The Necrotic Wake	
        break;
    case 377:
        return 354985; //  De Other Side
        break;
    case 378:
        return 354986; // Halls of Atonement
        break;
    case 379:
        return 354987; // Plaguefall
        break;
    case 380:
        return 354988; // Sanguine Depths
        break;
    case 381:
        return 354989; // Spires of Ascension
        break;
    case 382:
        return 354991; // Theater of Pain
        break;
    case 391:
        return 354991; // Tazavesh: Streets of Wonder  //chest id??? idk so use theater of pain chest
        break;
    case 392:
        return 354991; // Tazavesh: So'leah's Gambit //chest id???
        break;
    default:
        return 354972;
        break;
    }
    return 0;
}

uint32 ChallengeModeMgr::GetRandomChallengeAffixId(uint32 affix, uint32 level/* = 2*/)
{
    std::vector<uint32> affixs;
    switch (affix)
    {
    case 1:
        if (level >= 4)
        {
            affixs.push_back(5);
            affixs.push_back(6);
            affixs.push_back(7);
            affixs.push_back(8);
            affixs.push_back(11);
        }
        break;
    case 2:
        if (level >= 7)
        {
            affixs.push_back(13);
            affixs.push_back(14);
            affixs.push_back(12);
            affixs.push_back(2);
            affixs.push_back(4);
            affixs.push_back(3);
        }
        break;
    case 3:
        if (level >= 10)
        {
            affixs.push_back(9);
            affixs.push_back(10);
            affixs.push_back(15);
        }
        break;
    default:
        break;
    }

    if (affixs.empty())
        return 0;

    return Trinity::Containers::SelectRandomContainerElement(affixs);
}


