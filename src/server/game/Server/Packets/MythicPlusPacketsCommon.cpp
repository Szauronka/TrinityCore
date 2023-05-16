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

#include "MythicPlusPacketsCommon.h"

namespace WorldPackets
{
    namespace MythicPlus
    {
        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreMapSummary const& dungeonScoreMapSummary)
        {
            data << int32(dungeonScoreMapSummary.ChallengeModeID);
            data << float(dungeonScoreMapSummary.MapScore);
            data << int32(dungeonScoreMapSummary.BestRunLevel);
            data << int32(dungeonScoreMapSummary.BestRunDurationMS);
            data.WriteBit(dungeonScoreMapSummary.FinishedSuccess);
            data.FlushBits();

            return data;
        }

        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreSummary const& dungeonScoreSummary)
        {
            data << float(dungeonScoreSummary.OverallScoreCurrentSeason);
            data << float(dungeonScoreSummary.LadderScoreCurrentSeason);
            data << uint32(dungeonScoreSummary.Runs.size());
            for (DungeonScoreMapSummary const& dungeonScoreMapSummary : dungeonScoreSummary.Runs)
                data << dungeonScoreMapSummary;

            return data;
        }

        ByteBuffer& operator<<(ByteBuffer& data, MythicPlusMember const& mythicPlusMember)
        {
            data << mythicPlusMember.BnetAccountGUID;
            data << uint64(mythicPlusMember.GuildClubMemberID);
            data << mythicPlusMember.GUID;
            data << mythicPlusMember.GuildGUID;
            data << uint32(mythicPlusMember.NativeRealmAddress);
            data << uint32(mythicPlusMember.VirtualRealmAddress);
            data << int32(mythicPlusMember.ChrSpecializationID);
            data << int16(mythicPlusMember.RaceID);
            data << int32(mythicPlusMember.ItemLevel);
            data << int32(mythicPlusMember.CovenantID);
            data << int32(mythicPlusMember.SoulbindID);

            return data;
        }

        ByteBuffer& operator<<(ByteBuffer& data, MythicPlusRun const& mythicPlusRun)
        {
            data << int32(mythicPlusRun.MapChallengeModeID);
            data << uint32(mythicPlusRun.Level);
            data << int32(mythicPlusRun.DurationMs);
            data << mythicPlusRun.StartDate;
            data << mythicPlusRun.CompletionDate;
            data << int32(mythicPlusRun.Season);
            data.append(mythicPlusRun.KeystoneAffixIDs.data(), mythicPlusRun.KeystoneAffixIDs.size());
            data << uint32(mythicPlusRun.Members.size());
            data << float(mythicPlusRun.RunScore);
            for (MythicPlusMember const& member : mythicPlusRun.Members)
                data << member;

            data.WriteBit(mythicPlusRun.Completed);
            data.FlushBits();

            return data;
        }

        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreBestRunForAffix const& dungeonScoreBestRunForAffix)
        {
            data << int32(dungeonScoreBestRunForAffix.KeystoneAffixID);
            data << float(dungeonScoreBestRunForAffix.Score);
            data << dungeonScoreBestRunForAffix.Run;

            return data;
        }

        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreMapData const& dungeonScoreMapData)
        {
            data << int32(dungeonScoreMapData.MapChallengeModeID);
            data << uint32(dungeonScoreMapData.BestRuns.size());
            data << float(dungeonScoreMapData.OverAllScore);
            for (DungeonScoreBestRunForAffix const& bestRun : dungeonScoreMapData.BestRuns)
                data << bestRun;

            return data;
        }

        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreSeasonData const& dungeonScoreSeasonData)
        {
            data << int32(dungeonScoreSeasonData.Season);
            data << uint32(dungeonScoreSeasonData.SeasonMaps.size());
            data << uint32(dungeonScoreSeasonData.LadderMaps.size());
            data << float(dungeonScoreSeasonData.SeasonScore);
            data << float(dungeonScoreSeasonData.LadderScore);
            for (DungeonScoreMapData const& map : dungeonScoreSeasonData.SeasonMaps)
                data << map;

            for (DungeonScoreMapData const& map : dungeonScoreSeasonData.LadderMaps)
                data << map;

            return data;
        }

        ByteBuffer& operator<<(ByteBuffer& data, DungeonScoreData const& dungeonScoreData)
        {
            data << uint32(dungeonScoreData.Seasons.size());
            data << int32(dungeonScoreData.TotalRuns);
            for (DungeonScoreSeasonData const& season : dungeonScoreData.Seasons)
                data << season;

            return data;
        }

        void WorldPackets::MythicPlus::MythicPlusRequestMapStats::Read()
        {
            _worldPacket >> BnetAccountGUID;
            _worldPacket >> MapChallengeModeID;
        }

        WorldPacket const* MythicPlusRequestMapStatsResult::Write()
        {
            _worldPacket << RunCount;
            _worldPacket << RewardCount;
            _worldPacket << Season;
            _worldPacket << Subseason;
            _worldPacket << mythicPlusRuns.size();
            _worldPacket << mythicPlusRewards.size();
            _worldPacket.FlushBits();

            for (MythicPlusReward mythicPlusReward : mythicPlusRewards)
            {
                _worldPacket << mythicPlusReward.Unk1;
                _worldPacket << mythicPlusReward.Unk2;
                _worldPacket << mythicPlusReward.Unk3;
                _worldPacket << mythicPlusReward.Unk4;
                _worldPacket << mythicPlusReward.Unk5;
                _worldPacket << mythicPlusReward.UnknownBool;
            }

            for (MythicPlusRun mythicPlusRun : mythicPlusRuns)
            {
                _worldPacket << mythicPlusRun.Completed;
                _worldPacket << mythicPlusRun.CompletionDate;
                _worldPacket << mythicPlusRun.DurationMs;
                _worldPacket << mythicPlusRun.KeystoneAffixIDs.size();
                _worldPacket << mythicPlusRun.Level;
                _worldPacket << mythicPlusRun.MapChallengeModeID;
                _worldPacket << mythicPlusRun.Members.size();
                _worldPacket << mythicPlusRun.RunScore;
                _worldPacket << mythicPlusRun.Season;
                _worldPacket << mythicPlusRun.StartDate;
            }

            return &_worldPacket;
        }

        WorldPacket const* MythicPlusCurrentAffixesResult::Write()
        {
            _worldPacket << Count;

            for (uint32 i = 0; i < Count; ++i)
            {
                _worldPacket << uint32(Affixes[i]);
                _worldPacket << uint32(RequiredSeason[i]);
            }

            return &_worldPacket;
        }

        void WorldPackets::MythicPlus::StartChallengeMode::Read()
        {
            _worldPacket >> Bag;
            _worldPacket >> Slot;
            _worldPacket >> GameObjectGUID;
        }

        WorldPacket const* MythicPlusSeasonDataResult::Write()
        {
            _worldPacket << IsMythicPlusActive;

            return &_worldPacket;
        }

        WorldPacket const* WorldPackets::MythicPlus::UpdateDeathCount::Write()
        {
            _worldPacket << DeathCount;

            return &_worldPacket;
        }

        WorldPacket const* WorldPackets::MythicPlus::Complete::Write()
        {
            _worldPacket << ChallengeLevel;
            _worldPacket << MapId;
            _worldPacket << ChallengeId;
            _worldPacket << ChallengeLevel;

            _worldPacket << IsCompletedInTimer;
            _worldPacket.FlushBits();

            return &_worldPacket;
        }

        WorldPacket const* WorldPackets::MythicPlus::Start::Write()
        {
            _worldPacket << MapID;
            _worldPacket << ChallengeID;
            _worldPacket << ChallengeLevel;

            _worldPacket << Affixes1;
            _worldPacket << Affixes2;
            _worldPacket << Affixes3;
            _worldPacket << Affixes4;

            _worldPacket << DeathCount;
            _worldPacket << ClientEncounterStartPlayerInfo;

            _worldPacket << Energized;

            for (uint32 i = 0; i < DeathCount; ++i)
            {
                _worldPacket << DeathCount;
            }

            _worldPacket.FlushBits();

            return &_worldPacket;
        }

        void WorldPackets::MythicPlus::RequestLeaders::Read()
        {
            LastGuildUpdate = _worldPacket.read<uint32>();
            LastRealmUpdate = _worldPacket.read<uint32>();
            _worldPacket >> MapId;
            _worldPacket >> ChallengeID;

        }

        WorldPacket const* WorldPackets::MythicPlus::RequestLeadersResult::Write()
        {
            _worldPacket << GuildLeadersCount;
            _worldPacket << RealmLeadersCount;
            _worldPacket.AppendPackedTime(LastGuildUpdate);
            _worldPacket.AppendPackedTime(LastRealmUpdate);
            _worldPacket << MapID;
            _worldPacket << ChallengeID;

            for (auto const& guildLeaders : GuildLeaders)
            {
                _worldPacket << guildLeaders.InstanceRealmAddress;
                _worldPacket << guildLeaders.AttemptID;
                _worldPacket << guildLeaders.CompletionTime;
                _worldPacket << guildLeaders.CompletionDate;
                _worldPacket << guildLeaders.MedalEarned;
            }

            for (auto const& realmLeaders : RealmLeaders)
            {
                _worldPacket << realmLeaders.InstanceRealmAddress;
                _worldPacket << realmLeaders.AttemptID;
                _worldPacket << realmLeaders.CompletionTime;
                _worldPacket << realmLeaders.CompletionDate;
                _worldPacket << realmLeaders.MedalEarned;
            }

            return &_worldPacket;
        }

        ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::MythicPlus::ModeAttempt const& modeAttempt)
        {
            data << modeAttempt.InstanceRealmAddress;
            data << modeAttempt.AttemptID;
            data << modeAttempt.CompletionTime;
            data.AppendPackedTime(modeAttempt.CompletionDate);
            data << modeAttempt.MedalEarned;
            data << static_cast<uint32>(modeAttempt.Members.size());
            for (auto const& map : modeAttempt.Members)
            {
                data << map.VirtualRealmAddress;
                data << map.NativeRealmAddress;
                data << map.Guid;
                data << map.SpecializationID;
            }

            return data;
        }

        void WorldPackets::MythicPlus::MythicPlusCurrentAffixes::Read()
        {
        }

        void WorldPackets::MythicPlus::MythicPlusSeasonData::Read()
        {
        }

        WorldPacket const* WorldPackets::MythicPlus::NewPlayerRecord::Write()
        {
            _worldPacket << (int32)MapID;
            _worldPacket << (int32)CompletionMilliseconds;
            _worldPacket << (uint32)ChallengeLevel;

            return &_worldPacket;
        }

        WorldPacket const* Rewards::Write()
        {
            _worldPacket << Week;
            _worldPacket << uint32(MapChallengeModeRewards.size());


            for (auto const& map : MapChallengeModeRewards)
            {
                _worldPacket << map.Rewards.size();
            }


            return &_worldPacket;
        }
        WorldPacket const* ChallengeModeReset::Write()
        {
            _worldPacket << MapID;

            return &_worldPacket;
        }
}
}
