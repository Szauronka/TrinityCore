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

#include "WorldSession.h"
#include "LFGListMgr.h"
#include "Group.h"
#include "LFGList.h"
#include "LfgListPackets.h"
#include "SocialMgr.h"
#include "Player.h"
#include "Chat.h"
#include "ObjectAccessor.h"

void WorldSession::HandleRequestLfgListBlackList(WorldPackets::LfgList::RequestLfgListBlacklist& /*packet*/)
{
    WorldPackets::LfgList::LfgListUpdateBlacklist blacklist;
    SendPacket(blacklist.Write()); /// Activity and Reason loop - We dont need it
}

void WorldSession::HandleLfgListSearch(WorldPackets::LfgList::LfgListSearch& packet)
{
    WorldPackets::LfgList::LfgListSearchResults results;
    if (!sGroupFinderCategoryStore.LookupEntry(packet.CategoryID))
    {
        SendPacket(results.Write());
        return;
    }

    auto list = sLFGListMgr->GetFilteredList(packet.CategoryID, packet.SearchTerms, packet.LanguageSearchFilter, GetPlayer());
    results.AppicationsCount = list.size();

    for (auto& lfgEntry : list)
    {
        WorldPackets::LfgList::ListSearchResult result;
        auto group = lfgEntry->ApplicationGroup;
        if (!group)
            continue;

        auto leader = ObjectAccessor::FindPlayer(group->GetLeaderGUID());
        if (!leader)
            continue;

        if (lfgEntry->PrivateGroup)
            if (GetPlayer()->GetGuildId() == 0 || GetPlayer()->GetGuildId() != leader->GetGuildId())
                continue;

        auto activityID = lfgEntry->GroupFinderActivityData->ID;

        result.ApplicationTicket.RequesterGuid = group->GetGUID();
        result.ApplicationTicket.Id = group->GetGUID().GetCounter();
        result.ApplicationTicket.Type = WorldPackets::LFG::RideType::LfgListApplication;
        result.ApplicationTicket.Time = lfgEntry->CreationTime;
        result.LastTouchedVoiceChat = group->GetLeaderGUID();
        result.PartyGUID = group->GetLeaderGUID();
        result.BNetFriends = group->GetLeaderGUID();
        result.GuildMates = group->GetLeaderGUID();
        result.VirtualRealmAddress = GetVirtualRealmAddress();
        result.CompletedEncounters = 0;
        result.Age = lfgEntry->CreationTime;
        result.ResultID = 3;
        result.ApplicationStatus = AsUnderlyingType(LFGListApplicationStatus::None);

        for (auto const& member : group->GetMemberSlots())
        {
            uint8 role = member.roles >= 2 ? std::log2(member.roles) - 1 : member.roles;
            result.Members.emplace_back(member._class, role);
        }
        for (auto const& member : lfgEntry->ApplicationsContainer)
            if (auto applicant = member.second.GetPlayer())
                result.Members.emplace_back(applicant->GetClass(), member.second.RoleMask);

        result.JoinRequest.ActivityID = activityID;
        result.JoinRequest.ItemLevel = lfgEntry->ItemLevel;
        result.JoinRequest.AutoAccept = lfgEntry->AutoAccept;
        result.JoinRequest.TypeActivity = lfgEntry->TypeActivity;

        result.JoinRequest.HasQuest = lfgEntry->HasQuest;
        result.JoinRequest.GroupName = lfgEntry->GroupName;
        result.JoinRequest.Comment = lfgEntry->Comment;
        result.JoinRequest.VoiceChat = lfgEntry->VoiceChat;
        result.JoinRequest.minChallege = lfgEntry->minChallege;
        result.JoinRequest.PrivateGroup = lfgEntry->PrivateGroup;

        if(result.JoinRequest.HasQuest = true)
            result.JoinRequest.QuestID = lfgEntry->QuestID;

        if (result.JoinRequest.minChallege)
            result.JoinRequest.MinMyticPlusRating;
        

        results.SearchResults.emplace_back(result);
    }

    SendPacket(results.Write());
}

void WorldSession::HandleLfgListJoin(WorldPackets::LfgList::LfgListJoin& packet)
{
    auto list = new LFGListEntry;
    list->GroupFinderActivityData = sGroupFinderActivityStore.LookupEntry(packet.Request.ActivityID);
    list->ItemLevel = packet.Request.ItemLevel;
    list->HonorLevel = packet.Request.HonorLevel;
    list->GroupName = packet.Request.GroupName;
    list->Comment = packet.Request.Comment;
    list->VoiceChat = packet.Request.VoiceChat;
    list->TypeActivity = packet.Request.TypeActivity;
    list->PrivateGroup = packet.Request.PrivateGroup;
    list->HasQuest = packet.Request.HasQuest;
    list->AutoAccept = packet.Request.AutoAccept;
    if (packet.Request.HasQuest)
        list->QuestID = *packet.Request.QuestID;
    list->minChallege = packet.Request.minChallege;
    if (list->minChallege)
        packet.Request.MinMyticPlusRating;
    list->ApplicationGroup = nullptr;
    ChatHandler(GetPlayer()->GetSession()).PSendSysMessage("GroupFinder Join");
    sLFGListMgr->Insert(list, GetPlayer());
}

void WorldSession::HandleLfgListLeave(WorldPackets::LfgList::LfgListLeave& packet)
{
    auto entry = sLFGListMgr->GetEntrybyGuid(packet.ApplicationTicket.Id);
    if (!entry || !entry->ApplicationGroup->IsLeader(GetPlayer()->GetGUID()))
        return;

    sLFGListMgr->Remove(packet.ApplicationTicket.Id, GetPlayer());
}

void WorldSession::HandleLfgListInviteResponse(WorldPackets::LfgList::LfgListInviteResponse& packet)
{
    sLFGListMgr->ChangeApplicantStatus(sLFGListMgr->GetApplicationByID(packet.ApplicantTicket.Id), packet.Accept ? LFGListApplicationStatus::InviteAccepted : LFGListApplicationStatus::InviteDeclined);
}

void WorldSession::HandleLfgListGetStatus(WorldPackets::LfgList::LfgListGetStatus& /*packet*/)
{
    WorldPackets::LfgList::LfgListUpdateStatus status;
    SendPacket(status.Write());
}

void WorldSession::HandleLfgListApplyToGroup(WorldPackets::LfgList::LfgListApplyToGroup& packet)
{
    if (GetPlayer()->GetGroup()) // need for rolecheck and result
        ChatHandler(GetPlayer()->GetSession()).PSendSysMessage("You can't join it while you in group!");
    else
        sLFGListMgr->OnPlayerApplyForGroup(GetPlayer(), &packet.application.ApplicationTicket, packet.application.ActivityID, packet.application.Comment, packet.application.Role);
}

void WorldSession::HandleLfgListCancelApplication(WorldPackets::LfgList::LfgListCancelApplication& packet)
{
    if (auto entry = sLFGListMgr->GetEntryByApplicant(packet.ApplicantTicket))
        sLFGListMgr->ChangeApplicantStatus(entry->GetApplicant(packet.ApplicantTicket.Id), LFGListApplicationStatus::Cancelled);
}

void WorldSession::HandleLfgListDeclineApplicant(WorldPackets::LfgList::LfgListDeclineApplicant& packet)
{
    if (!_player->GetGroup()->IsAssistant(_player->GetGUID()) && !_player->GetGroup()->IsLeader(_player->GetGUID()))
        return;

    if (auto entry = sLFGListMgr->GetEntrybyGuid(packet.ApplicantTicket.Id))
        sLFGListMgr->ChangeApplicantStatus(entry->GetApplicant(packet.ApplicationTicket.Id), LFGListApplicationStatus::Declined);
}

void WorldSession::HandleLfgListInviteApplicant(WorldPackets::LfgList::LfgListInviteApplicant& packet)
{
    if (!_player->GetGroup()->IsAssistant(_player->GetGUID()) && !_player->GetGroup()->IsLeader(_player->GetGUID()))
        return;

    if (auto entry = sLFGListMgr->GetEntrybyGuid(packet.ApplicantTicket.Id))
    {
        auto applicant = entry->GetApplicant(packet.ApplicationTicket.Id);
        applicant->RoleMask = (*packet.Applicant.begin()).Role;

        sLFGListMgr->ChangeApplicantStatus(applicant, LFGListApplicationStatus::Invited);
    }
}

void WorldSession::HandleLfgListUpdateRequest(WorldPackets::LfgList::LfgListUpdateRequest& packet)
{
    auto entry = sLFGListMgr->GetEntrybyGuid(packet.Ticket.Id);
    if (!entry || !entry->ApplicationGroup->IsLeader(_player->GetGUID()))
        return;

    entry->AutoAccept = packet.UpdateRequest.AutoAccept;
    entry->GroupName = packet.UpdateRequest.GroupName;
    entry->Comment = packet.UpdateRequest.Comment;
    entry->VoiceChat = packet.UpdateRequest.VoiceChat;
    entry->HonorLevel = packet.UpdateRequest.HonorLevel;
    if (packet.UpdateRequest.HasQuest)
        entry->QuestID = *packet.UpdateRequest.QuestID;

    if (packet.UpdateRequest.ItemLevel < sLFGListMgr->GetPlayerItemLevelForActivity(entry->GroupFinderActivityData, _player))
        entry->ItemLevel = packet.UpdateRequest.ItemLevel;
    entry->PrivateGroup = packet.UpdateRequest.PrivateGroup;

    sLFGListMgr->AutoInviteApplicantsIfPossible(entry);
    sLFGListMgr->SendLFGListStatusUpdate(entry);
}
