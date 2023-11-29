/*
 * Copyright (C) 2021 BfaCore Reforged
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

#include "LfgListPackets.h"
#include "Optional.h"

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::LFGListBlacklist const& blackList)
{
    data << blackList.ActivityID;
    data << blackList.Reason;

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LfgList::LFGListBlacklist& blackList)
{
    data >> blackList.ActivityID;
    data >> blackList.Reason;
    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LfgList::LfgListRideTicket& ticket)
{
    data >> ticket.RequesterGuid;
    data >> ticket.Id;
    ticket.Type = data.read<WorldPackets::LfgList::RideType>();
    data >> ticket.Time;
    ticket.Unknown925 = data.ReadBit();
    data.ResetBitPos();

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::LfgListRideTicket const& ticket)
{
    data << ticket.RequesterGuid;
    data << uint32(ticket.Id);
    data << uint32(ticket.Type);
    data << ticket.Time;
    data.WriteBit(ticket.Unknown925);
    data.FlushBits();

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::ListRequest const& join)
{
    data << join.ActivityID;

    data << join.ItemLevel;
    data << join.HonorLevel;
    data << join.PvPRating;
    data << join.PlayStyle;

    data.WriteBits(join.GroupName.length(), 8);
    data.WriteBits(join.Comment.length(), 11);
    data.WriteBits(join.VoiceChat.length(), 8);
    data.WriteBit(join.AutoAccept);
    data.WriteBit(join.PrivateGroup);
    data.WriteBit(join.QuestID.has_value() && *join.QuestID != 0);
    data.WriteBit(join.MythicPlusRating.has_value() && *join.MythicPlusRating != 0);
    data.FlushBits();

    data.WriteString(join.GroupName);
    data.WriteString(join.Comment);
    if (join.VoiceChatReq)
        data.WriteString(join.VoiceChat);
 

    if (join.QuestID.has_value() && *join.QuestID != 0)
        data << *join.QuestID;

    if (join.MythicPlusRating.has_value() && *join.MythicPlusRating != 0)
        data << *join.MythicPlusRating;

    if (join.IsCrossFaction)
        data << *join.IsCrossFaction;

    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LfgList::ListRequest& join)
{
    data >> join.ActivityID;

    data >> join.ItemLevel;
    data >> join.HonorLevel;
    data >> join.PvPRating;
    data >> join.PlayStyle;

    uint32 NameLen = data.ReadBits(8);
    uint32 CommenteLen = data.ReadBits(11);
    uint32 VoiceChateLen = data.ReadBits(8);
    join.AutoAccept = data.ReadBit();
    join.PrivateGroup = data.ReadBit();
    bool isForQuest = data.ReadBit();
    bool isMythicPlusActivity = data.ReadBit();
    bool isVoiceChatRequired = data.ReadBit();
    bool isCrossFaction = data.ReadBit();

    join.GroupName = data.ReadString(NameLen);
    join.Comment = data.ReadString(CommenteLen);
    if(isVoiceChatRequired)
        join.VoiceChat = data.ReadString(VoiceChateLen);

    if (isForQuest)
        data >> *join.QuestID;
    if (isMythicPlusActivity)
        data >> *join.MythicPlusRating;
    if (isCrossFaction)
        data >> *join.IsCrossFaction;


    return data;
}

WorldPacket const* WorldPackets::LfgList::LfgListUpdateBlacklist::Write()
{
    std::sort(Blacklist.begin(), Blacklist.end(), [](LFGListBlacklist const& a, LFGListBlacklist const& b) -> bool
    {
            return a.ActivityID < b.ActivityID;
    });

    _worldPacket << BlacklistEntryCount;
    
    for (uint32 i = 0; i < BlacklistEntryCount; i++)
    {
        _worldPacket << Blacklist[i];
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListUpdateStatus::Write()
{
    _worldPacket << ApplicationTicket;
    _worldPacket << RemainingTime;
    _worldPacket << ResultId;
    _worldPacket << Request;
    _worldPacket.FlushBits();
    _worldPacket.WriteBit(Listed);

    return &_worldPacket;
}

void WorldPackets::LfgList::LfgListInviteResponse::Read()
{
    _worldPacket >> ApplicantTicket;
    Accept = _worldPacket.ReadBit();
    _worldPacket.FlushBits();
}

void WorldPackets::LfgList::LfgListLeave::Read()
{
    _worldPacket >> ApplicationTicket;
}

WorldPacket const* WorldPackets::LfgList::LfgListJoinResult::Write()
{
    _worldPacket << ApplicationTicket;
    _worldPacket << Result;
    _worldPacket << Status;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListSearchStatus::Write()
{
    _worldPacket << Ticket;
    _worldPacket << Status;
    _worldPacket.WriteBit(UnkBit);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::LfgList::LfgListSearch::Read()
{
    auto count = _worldPacket.ReadBits(5);
    _worldPacket.FlushBits();

    if (count)
    {
        for (uint32 i = 0; i < count; i++)
        {
            auto len = new uint32[3];
            for (int i = 0; i < 3; i++)
                len[i] = _worldPacket.ReadBits(5);

            _worldPacket.FlushBits();

            for (int i = 0; i < 3; i++)
                LanguageFilter += _worldPacket.ReadBits(len[i]);
        }
    }

    _worldPacket >> GroupFinderCategoryId;
    _worldPacket >> SubActivityGroupID;
    _worldPacket >> LFGListFilter;
    _worldPacket >> LanguageFilter;
    Blacklist.resize(_worldPacket.read<uint32>());
    PartyGUID.resize(_worldPacket.read<uint32>());

    for (auto& v : Blacklist)
    {
        _worldPacket >> v.ActivityID;
        _worldPacket >> v.Reason;
    }

    for (auto& v : PartyGUID)
        _worldPacket >> v;

}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::ListSearchResult const& listSearch)
{
    data << listSearch.ApplicationTicket;
    data << listSearch.ResultID;
    data << listSearch.LeaderGuid;
    data << listSearch.LastTouchedAny;
    data << listSearch.LastTouchedName;
    data << listSearch.LastTouchedComment;
    data << listSearch.LastTouchedVoiceChat;
    data << listSearch.VirtualRealmAddress;
    data << static_cast<uint32>(listSearch.BNetFriendsGuids.size());
    data << static_cast<uint32>(listSearch.NumCharFriendsGuids.size());
    data << static_cast<uint32>(listSearch.NumGuildMateGuids.size());
    data << static_cast<uint32>(listSearch.Members.size());
    data << listSearch.CompletedEncounters;
    data << listSearch.CreationTime;
    data << listSearch.ApplicationStatus;


    for (ObjectGuid const& v : listSearch.BNetFriendsGuids)
        data << v;

    for (ObjectGuid const& v : listSearch.NumCharFriendsGuids)
        data << v;

    for (ObjectGuid const& v : listSearch.NumGuildMateGuids)
        data << v;

    for (auto const& v : listSearch.Members)
        data << v;

    data << listSearch.JoinRequest;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::MemberInfo const& memberInfo)
{
    data << memberInfo.ClassID;
    data << memberInfo.Role;

    return data;
}

WorldPacket const* WorldPackets::LfgList::LfgListSearchResults::Write()
{
    _worldPacket << AppicationsCount;
    _worldPacket << static_cast<uint32>(SearchResults.size());
    for (auto const& v : SearchResults)
        _worldPacket << v;

    return &_worldPacket;
}

void WorldPackets::LfgList::LfgListApplyToGroup::Read()
{
    _worldPacket >> application.ApplicationTicket;
    _worldPacket >> application.ActivityID;
    application.Role = _worldPacket.read<uint8>();
    application.Comment = _worldPacket.ReadString(_worldPacket.ReadBits(12));
}

void WorldPackets::LfgList::LfgListCancelApplication::Read()
{
    _worldPacket >> ApplicantTicket;
}

void WorldPackets::LfgList::LfgListDeclineApplicant::Read()
{
    _worldPacket >> ApplicantTicket;
    _worldPacket >> ApplicationTicket;
}

void WorldPackets::LfgList::LfgListInviteApplicant::Read()
{
    _worldPacket >> ApplicantTicket;
    _worldPacket >> ApplicationTicket;
    Applicant.resize(_worldPacket.read<uint32>());
    for (auto& v : Applicant)
    {
        _worldPacket >> v.PlayerGUID;
        _worldPacket >> v.Role;
    }
}

void WorldPackets::LfgList::LfgListUpdateRequest::Read()
{
    _worldPacket >> Ticket;
    _worldPacket >> UpdateRequest;
}

WorldPacket const* WorldPackets::LfgList::LfgListApplyToGroupResponce::Write()
{
    _worldPacket << ApplicantTicket;
    _worldPacket << ApplicationTicket;
    _worldPacket << InviteExpireTimer;
    _worldPacket << Status;
    _worldPacket << Role;
    _worldPacket << SearchResult;
    _worldPacket.WriteBits(ApplicationStatus, 4);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListApplicationUpdate::Write()
{
    _worldPacket << ApplicationTicket;
    _worldPacket << ListRideTicket;
    _worldPacket << UnkInt;
    _worldPacket << ResultId;
    _worldPacket << Role;
    _worldPacket << Status;
    

    return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListSearchResultUpdate::Write()
{
    _worldPacket << ResultUpdate.size();
    for (auto const& update : ResultUpdate)
    {
        _worldPacket << static_cast<uint32>(update.Members.size());
        for (auto const& member : update.Members)
            _worldPacket << member;
        _worldPacket << update.SequenceNum;
        _worldPacket << update.LeaderGuid;
        _worldPacket << update.LastTouchedAny.GetGUIDLow();
        _worldPacket << update.LastTouchedName.GetGUIDLow();
        _worldPacket << update.LastTouchedComment;
        _worldPacket << update.LastTouchedVoiceChat;
        _worldPacket << update.VirtualRealmAddress;
        _worldPacket.WriteBit(update.UnkBit96);
        _worldPacket << *update.BnetFriendCount;
        _worldPacket << *update.CharacterFriendCount;
        _worldPacket << *update.GuildMatesCount;
        _worldPacket << *update.MemberCount;
        _worldPacket << update.CompletedEncounters;
        _worldPacket << update.CreationTime;
        _worldPacket << update.ApplicationStatus;
        _worldPacket << update.PartyGUID;
        _worldPacket << update.BNetFriends;
        _worldPacket << update.CharacterFriends;
        _worldPacket << update.JoinRequest;
        _worldPacket.WriteBit(update.UnkBIt);
        _worldPacket.WriteBit(update.UnkBIt2);
        _worldPacket.WriteBit(update.UnkBIt3);
        _worldPacket.WriteBit(update.UnkBIt4);

    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListApplicantlistUpdate::Write()
{
	_worldPacket << ApplicationTicket;
	_worldPacket << Applicants.size();
	_worldPacket << UnkInt;
	for (auto const& v : Applicants)
	{
		_worldPacket << v.ApplicantTicket;
		_worldPacket << v.ApplicantPartyLeader;
		_worldPacket << v.Member.size();
		for (auto const& z : v.Member)
		{
			_worldPacket << z.PlayerGUID;
			_worldPacket << z.VirtualRealmAddress;
			_worldPacket << z.ItemLevel;
			_worldPacket << z.Level;
			_worldPacket << z.HonorLevel;
			_worldPacket << z.PossibleRoleMask;
			_worldPacket << z.SelectedRoleMask;
			_worldPacket << z.AcStat.size();
			for (auto const& x : z.AcStat)
			{
				_worldPacket << x.UnkInt4;
				_worldPacket << x.UnkInt5;
			}
		}

		_worldPacket.WriteBits(v.ApplicationStatus, 4);
		_worldPacket.WriteBit(v.Listed);
		_worldPacket.WriteString(v.Comment);
	}

	return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListUpdateExpiration::Write()
{
    _worldPacket << ApplicationTicket;
    _worldPacket << TimeoutTime;
    _worldPacket << Status;

    return &_worldPacket;
}

void WorldPackets::LfgList::LfgListJoin::Read()
{
    _worldPacket >> Request;
}
