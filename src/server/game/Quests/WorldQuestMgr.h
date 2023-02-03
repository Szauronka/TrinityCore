/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
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

#ifndef _WORLDQUESTMGR_H
#define _WORLDQUESTMGR_H

#define WORLD_QUEST_MAX_FILL 77
#define WORLD_QUEST_EMISSARY 3

#include "QuestDef.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Player.h"
#include "CriteriaHandler.h"
#include "QuestPackets.h"
#include "WorldStatePackets.h"
#include <unordered_map>
#include <vector>

struct WorldQuestTemplate;
struct ActiveWorldQuest;
struct WorldQuestReward;

typedef std::unordered_map<uint8 /*expansion*/, std::unordered_map<uint8 /*teamId*/, std::unordered_map<uint32 /*questId*/, WorldQuestTemplate*>>> WorldQuestTemplateMap;
typedef std::unordered_map<uint8 /*expansion*/, std::unordered_map<uint8 /*teamId*/, std::unordered_map<uint32 /*questId*/, ActiveWorldQuest*>>> ActiveWorldQuestMap;
typedef std::unordered_map<uint32, std::set<Quest const*>> QuestAreaTaskMap;
typedef std::unordered_map<uint32 /*RewardId*/, std::vector<WorldQuestReward>> WorldQuestRewardMap;
typedef std::unordered_map<uint32 /*QuestInfo*/, std::vector<uint32 /*RewardId*/>> WorldQuestRewardByQuestInfoMap;

enum WorldQuestRewardType
{
    WORLD_QUEST_REWARD_ITEM = 0,
    WORLD_QUEST_REWARD_CURRENCY = 1,
    WORLD_QUEST_REWARD_GOLD = 2,
};

class TC_GAME_API WorldQuestMgr
{
public:
    WorldQuestMgr();
    ~WorldQuestMgr();

    static WorldQuestMgr* instance();
    const std::vector<uint8> WorldQuestsExpansions = { EXPANSION_LEGION, EXPANSION_BATTLE_FOR_AZEROTH, EXPANSION_SHADOWLANDS, EXPANSION_DRAGONFLIGHT };
    const std::vector<uint8> WorldQuestsTeams = { TEAM_ALLIANCE, TEAM_HORDE };

    void LoadWorldQuestTemplates();
    void LoadWorldQuestRewardTemplates();
    void AddWorldQuestTemplate(Quest const* quest, WorldQuestTemplate* worldQuestTemplate);
    void LoadActiveWorldQuests();
    void Update();

    void CleanWorldQuestTemplates();

    void AddWorldQuestTask(Quest const* quest);
    void RemoveWorldQuestTask(Quest const* quest);
    std::set<Quest const*> const* GetWorldQuestTask(uint32 areaId) const;

    void ActivateQuest(WorldQuestTemplate* worldQuestTemplate);
    void DisableQuest(ActiveWorldQuest* activeWorldQuest, bool deleteFromMap/* = true*/);
    void GetAreaPoiID(ActiveWorldQuestMap activeWorldQuests, AreaPOIEntry areaPoi);

    bool IsQuestActive(uint32 questId);

    void RewardQuestForPlayer(Player* player, uint32 questId);

    WorldQuestTemplate* GetWorldQuestTemplate(uint32 questId);
    ActiveWorldQuest* GetActiveWorldQuest(uint32 questId);

    uint8 GetActiveEmissaryQuestsCount(uint8 expansion, uint8 teamId);
    uint32 GetActiveQuestsCount(uint8 expansion, uint8 teamId);

    uint32 GetRandomRewardForQuestType(uint32 questType);
    std::vector<WorldQuestReward const*> GetRewardsForPlayerById(Player* player, uint32 rewardId);

    void BuildPacket(Player* player, WorldPackets::Quest::WorldQuestUpdateResponse& packet);
    void BuildRewardPacket(Player* player, uint32 questId, WorldPackets::Quest::QueryQuestRewardResponse& packet);
    void FillInitWorldStates(WorldPackets::WorldState::InitWorldStates& packet);

    std::vector<CriteriaEntry const*> GetCriteriasForQuest(uint32 quest_id);

    void RefreshEmissaryQuests();
    void AddEmissaryQuestsOnPlayerIfNeeded(Player* player);

    uint32 GetTimerForQuest(uint32 questId);
    TeamId GetQuestTeamId(Quest const* quest);

private:
    WorldQuestTemplateMap _worldQuestTemplates;
    WorldQuestTemplateMap _emissaryWorldQuestTemplates;
    QuestAreaTaskMap _worldQuestAreaTaskStore;
    QuestAreaTaskMap _questAreaTaskStore;

    WorldQuestRewardMap _worldQuestRewards;
    WorldQuestRewardByQuestInfoMap _worldQuestRewardByQuestInfos;

    ActiveWorldQuestMap _activeWorldQuests;
    //std::vector<uint32, ActiveWorldQuestMap> _activeWorldQuests;
};

#define sWorldQuestMgr WorldQuestMgr::instance()

struct WorldQuestTemplate
{
    WorldQuestTemplate(uint32 questId, uint32 duration, uint32 variableId, uint8 value) :
        QuestId(questId), Duration(duration), VariableId(variableId), Value(value) { }

    std::vector<uint32> AreaIDs;
    WorldQuestTemplate const* worldQuest = nullptr;
    Quest const* quest = nullptr;
    int32 AreaID = 0;
    uint32 QuestId = 0;
    uint32 Duration = 0;
    uint32 VariableId = 0;
    uint8 Value = 0;

    Quest const* GetQuest() const { return sObjectMgr->GetQuestTemplate(QuestId); }
};

struct WorldQuestReward
{
    WorldQuestReward(uint32 id, uint8 questType, uint8 rewardType, uint32 rewardId, uint32 rewardCount, uint32 rewardContext) :
        Id(id), QuestType(questType), RewardType(rewardType), RewardId(rewardId), RewardCount(rewardCount), RewardContext(rewardContext) { }

    uint32 Id = 0;
    uint8 QuestType = 0;
    uint8 RewardType = 0;
    uint32 RewardId = 0;
    uint32 RewardCount = 0;
    uint32 RewardContext = 0;
};

struct ActiveWorldQuest
{
    ActiveWorldQuest(uint32 questId, uint32 rewardId, int32 startTime) :
        QuestId(questId), RewardId(rewardId), StartTime(startTime) { }

    uint32 QuestId = 0;
    uint32 RewardId = 0;
    int32 StartTime = 0;

    WorldQuestTemplate const* GetTemplate() const { return sWorldQuestMgr->GetWorldQuestTemplate(QuestId); }

    int32 GetRemainingTime() const
    {
        if (WorldQuestTemplate const* worldQuestTemplate = GetTemplate())
            return (StartTime + worldQuestTemplate->Duration) - time(nullptr);

        return 0;
    }

    bool IsEmissaryQuest() const
    {
        if (WorldQuestTemplate const* worldQuestTemplate = GetTemplate())
            if (Quest const* quest = worldQuestTemplate->GetQuest())
                return quest->IsEmissaryQuest();

        return false;
    }
};

#endif
