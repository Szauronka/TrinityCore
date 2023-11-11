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

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::ListRequest const& join)
{
    data << join.ActivityID;

    data << join.ItemLevel;
    data << join.HonorLevel;

    data.WriteBits(join.GroupName.length(), 8);
    data.WriteBits(join.Comment.length(), 12);
    data.WriteBits(join.VoiceChat.length(), 6);
    data.WriteBit(join.PrivateGroup);
    data.WriteBit(join.HasQuest);
    data.WriteBit(join.AutoAccept);
    data.FlushBits();

    data.WriteString(join.GroupName);
    data.WriteString(join.Comment);
    data.WriteString(join.VoiceChat);

    if (join.HasQuest && *join.QuestID != 0)
        data << *join.QuestID;


    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LfgList::ListRequest& join)
{
    data >> join.ActivityID;

    data >> join.ItemLevel;
    data >> join.HonorLevel;

    uint32 NameLen = data.ReadBits(8);
    uint32 CommenteLen = data.ReadBits(12);
    uint32 VoiceChateLen = data.ReadBits(6);
    join.PrivateGroup = data.ReadBit();
    join.HasQuest = data.ReadBit();
    join.AutoAccept = data.ReadBit();

    join.GroupName = data.ReadString(NameLen);
    join.Comment = data.ReadString(CommenteLen);
    join.VoiceChat = data.ReadString(VoiceChateLen);

    if (join.HasQuest)
        data >> *join.QuestID;

    return data;
}

WorldPacket const* WorldPackets::LfgList::LfgListUpdateBlacklist::Write()
{
    std::sort(Blacklist.begin(), Blacklist.end(), [](LFGListBlacklist const& a, LFGListBlacklist const& b) -> bool
    {
            return a.ActivityID < b.ActivityID;
    });

    _worldPacket << BlacklistEntryCount;
    
    for (int i = 0; i < BlacklistEntryCount; i++)
    {
        _worldPacket << Blacklist[i];
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListUpdateStatus::Write()
{
    _worldPacket << ApplicationTicket;
    _worldPacket << JoinDate;
    _worldPacket << ServerDate;
    _worldPacket << ResultId;
    _worldPacket << Request;
    _worldPacket.FlushBits();

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
        for (int i = 0; i < count; i++)
        {
            auto len = new uint32[3];
            for (int i = 0; i < 3; i++)
                len[i] = _worldPacket.ReadBits(5);

            _worldPacket.FlushBits();

            for (int i = 0; i < 3; i++)
                LanguageSearchFilter += " " + _worldPacket.ReadString(len[i]);
        }
    }

    _worldPacket >> GroupFinderCategoryId;
    _worldPacket >> SubActivityGroupID;
    _worldPacket >> SearchTerms;
    _worldPacket >> Filter;
    _worldPacket >> PreferredFilters;
    _worldPacket << PartyGUID;

}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::LfgListSearchResult const& listSearch)
{
    data << listSearch.SequenceNum;
    data << listSearch.LeaderGuid;
    data << listSearch.LastTouchedAny;
    data << listSearch.LastTouchedName;
    data << listSearch.LastTouchedComment;
    data << listSearch.LastTouchedVoiceChat;
    data << listSearch.VirtualRealmAddress;
    data << listSearch.VirtualRealmAddress;
    data << uint32(listSearch.BnetFriendCount.has_value());
    data << uint32(listSearch.CharacterFriendCount.has_value());
    data << uint32(listSearch.GuildMatesCount.has_value());
    data << uint32(listSearch.MemberCount.has_value());
    data << listSearch.CompletedEncounters;
    data << listSearch.CreationTime;
    data << listSearch.ApplicationStatus;
    data << listSearch.PartyGUID;
    data << listSearch.BNetFriends;
    data << listSearch.CharacterFriends;
    data << listSearch.GuildMates;
    data << listSearch.Members.size();
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
        _worldPacket << (v.Comment, 12);
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::LfgList::LfgListSearchResultUpdate::Write()
{
    _worldPacket << ResultUpdate.size();
    for (auto const& update : ResultUpdate)
    {
        _worldPacket << update.SequenceNum;
        _worldPacket << update.LeaderGuid;
        _worldPacket << update.LastTouchedAny.GetGUIDLow();
        _worldPacket << update.LastTouchedName.GetGUIDLow();
        _worldPacket << update.LastTouchedComment;
        _worldPacket << update.LastTouchedVoiceChat;
        _worldPacket << update.VirtualRealmAddress;
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
        _worldPacket << update.Members.size();
        for (auto const& member : update.Members)
            _worldPacket << member;
        _worldPacket << update.JoinRequest;

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
