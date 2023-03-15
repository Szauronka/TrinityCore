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
void MythicPlusRequestMapStats::Read()
{
}

void MythicPlusCurrentAffixes::Read()
{
}

void MythicPlusSeasonData::Read()
{
}

WorldPacket const* MythicPlusRequestMapStatsResult::Write()
{
    _worldPacket << uint32(mythicPlusRuns.size());
    _worldPacket << uint32(RewardCount);
    _worldPacket << int32(Season);
    _worldPacket << int32(Subseason);

    for (MythicPlusRun mythicPlusRun : mythicPlusRuns)
        _worldPacket << mythicPlusRun;

    return &_worldPacket;
}

WorldPacket const* MythicPlusCurrentAffixesResult::Write()
{
    _worldPacket << uint32(Count);

    for (uint32 i = 0; i < Count; ++i)
    {
        _worldPacket << uint32(Affixes[i]);
        _worldPacket << uint32(RequiredSeason[i]);
    }

    return &_worldPacket;
}

void WorldPackets::MythicPlus::StartChallengeMode::Read()
{
    _worldPacket >> IsKeyCharged;
    _worldPacket >> UnkInt;
    _worldPacket >> GameObjectGUID;
}

WorldPacket const* MythicPlusSeasonDataResult::Write()
{
    _worldPacket << uint32(IsMythicPlusActive);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::MythicPlus::UpdateDeathCount::Write()
{
	_worldPacket << (uint32)DeathCount;

	return &_worldPacket;
}

WorldPacket const* WorldPackets::MythicPlus::Complete::Write()
{
    _worldPacket << CompletionMilliseconds;
    _worldPacket << MapID;
    _worldPacket << ChallengeID;
    _worldPacket << StartedChallengeLevel;

    _worldPacket.WriteBit(IsCompletedInTimer);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::MythicPlus::Start::Write()
{
    _worldPacket << (uint32)MapID;
    _worldPacket << (uint32)ChallengeID;
    _worldPacket << (uint32)ChallengeLevel;

    _worldPacket << (uint32)Affixes1;
    _worldPacket << (uint32)Affixes2;
    _worldPacket << (uint32)Affixes3;
    _worldPacket << (uint32)Affixes4;

    _worldPacket << (uint32)DeathCount;
    _worldPacket << (uint32)ClientEncounterStartPlayerInfo;

    _worldPacket << (uint8)Energized;
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::MythicPlus::ChallengeModeMapStatsUpdate::Write()
{
    _worldPacket << MapId;
    _worldPacket << BestCompletionMilliseconds;
    _worldPacket << LastCompletionMilliseconds;
    _worldPacket << CompletedChallengeLevel;
    _worldPacket << ChallengeID;
    _worldPacket << BestMedalDate;
    _worldPacket << BestSpecID[0];
    _worldPacket << Affixes[4];

    return &_worldPacket;
}

void WorldPackets::MythicPlus::RequestLeaders::Read()
{
    _worldPacket >> MapId;
    LastGuildUpdate = _worldPacket.read<uint32>();
    LastRealmUpdate = _worldPacket.read<uint32>();
    _worldPacket >> ChallengeID;
}

WorldPacket const* WorldPackets::MythicPlus::RequestLeadersResult::Write()
{
    _worldPacket << MapID;
    _worldPacket << ChallengeID;
    _worldPacket.AppendPackedTime(LastGuildUpdate);
    _worldPacket.AppendPackedTime(LastRealmUpdate);

    _worldPacket << static_cast<uint32>(GuildLeaders.size());
    _worldPacket << static_cast<uint32>(RealmLeaders.size());

    for (auto const& guildLeaders : GuildLeaders)
        _worldPacket << guildLeaders;

    for (auto const& realmLeaders : RealmLeaders)
        _worldPacket << realmLeaders;

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


}
}
