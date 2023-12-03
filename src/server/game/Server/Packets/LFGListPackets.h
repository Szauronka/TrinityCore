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

#ifndef LfgListPackets_h__
#define LfgListPackets_h__

#include "Packet.h"
#include "LFGPacketsCommon.h"
#include "LFGPackets.h"
#include "Optional.h"


namespace WorldPackets
{
    namespace LfgList
    {
        struct LFGListBlacklist
        {
            int32 ActivityID = 0;
            int32 Reason = 0;
        };

        enum class RideType : uint32
        {
            None = 0,
            Battlegrounds = 1,
            Lfg = 2,
            PvPPetBattle = 3,
            LfgListApplication = 4, // LFGListParty
            PetBattle = 5,
            LfgListApplicant = 6, // LFGListPlayer
        };

        struct LfgListRideTicket
        {
            ObjectGuid RequesterGuid;
            uint32 Id = 0;
            RideType Type = RideType::None;
            Timestamp<> Time;
            bool Unknown925 = false;
        };

        struct ApplicationToGroup
        {
            LFG::RideTicket ApplicationTicket;
            uint32 ActivityID = 0;
            std::string Comment;
            uint8 Role = 0;
        };

        struct ListRequest
        {
            ListRequest() = default;
            ListRequest(Optional<uint32> questId, Optional<uint32> mythicRating, int32 activityId, uint32 honorLevel, uint32 pvpRating, uint32 itemLevel, uint8 playStyle, std::string groupName, std::string comment, std::string voiceChat, bool autoAccept, bool privateGroup, bool voiceChatReq, bool crossFaction) :
                QuestID(questId),MythicPlusRating(mythicRating), ActivityID(activityId), HonorLevel(honorLevel), PvPRating(pvpRating), ItemLevel(itemLevel), PlayStyle(playStyle), GroupName(groupName), Comment(comment), VoiceChat(voiceChat), AutoAccept(autoAccept), PrivateGroup(privateGroup), VoiceChatReq(voiceChatReq), IsCrossFaction(crossFaction)  { }

            Optional<uint32> QuestID = 0;;
            Optional<uint32> MythicPlusRating = 0;
            int32 ActivityID = 0;
            uint32 HonorLevel = 0;
            float ItemLevel = 0.0f;
            uint32 PvPRating = 0;
            uint8 PlayStyle = 0; // LFG_PLAYSTYLE_PVP, LFG_PLAYSTYLE_PVE, LFG_PLAYSTYLE_PVE_MYTHICZERO
            std::string GroupName;
            std::string Comment;
            std::string VoiceChat;
            bool AutoAccept = false;
            bool PrivateGroup = false;
            Optional<bool> VoiceChatReq = false;
            Optional<bool> IsCrossFaction = false;
        };

        struct MemberInfo
        {
            MemberInfo() = default;
            MemberInfo(uint8 classID, uint8 role) : ClassID(classID), Role(role) { }

            uint8 ClassID = CLASS_NONE;
            uint8 Role = 0;
        };

        struct ApplicantStruct
        {
            ApplicantStruct() = default;
            ApplicantStruct(ObjectGuid playerGUID, uint8 role) : PlayerGUID(playerGUID), Role(role) { }

            ObjectGuid PlayerGUID;
            uint8 Role = 0;
        };

        struct ApplicantMember
        {
            ApplicantMember() { }

            struct ACStatInfo
            {
                uint32 UnkInt4 = 0;
                uint32 UnkInt5 = 0;
            };

            std::list<ACStatInfo> AcStat;
            ObjectGuid PlayerGUID;
            uint32 VirtualRealmAddress = 0;
            uint32 Level = 0;
            uint32 HonorLevel = 0;
            uint16 ItemLevel = 0;
            uint8 PossibleRoleMask = 0;
            uint8 SelectedRoleMask = 0;
        };

        struct ApplicantInfo
        {
            std::vector<ApplicantMember> Member;
            LFG::RideTicket ApplicantTicket;
            ObjectGuid ApplicantPartyLeader;
            std::string Comment;
            uint8 ApplicationStatus = 0;
            bool Listed = false;
        };

        struct ListSearchResult
        {
            LFG::RideTicket ApplicationTicket;
            ListRequest JoinRequest;
            std::vector<MemberInfo> Members;
            GuidList BNetFriendsGuids;
            GuidList NumCharFriendsGuids;
            GuidList NumGuildMateGuids;
            ObjectGuid LeaderGuid;
            ObjectGuid LastTouchedAny;
            ObjectGuid LastTouchedName;
            ObjectGuid LastTouchedComment;
            ObjectGuid LastTouchedVoiceChat;
            uint32 VirtualRealmAddress = 0;
            uint32 CompletedEncounters = 0;
            uint32 CreationTime = 0;
            uint32 ResultID = 0;
            uint8 ApplicationStatus = 0;
        };

        struct LfgListSearchResult
        {
            uint32 SequenceNum = 0;
            ObjectGuid LeaderGuid;
            ObjectGuid LastTouchedAny;
            ObjectGuid LastTouchedName;
            ObjectGuid LastTouchedComment;
            ObjectGuid LastTouchedVoiceChat;
            uint32 VirtualRealmAddress = 0;
            Optional<uint32> BnetFriendCount = 0;
            Optional<uint32> CharacterFriendCount = 0;
            Optional<uint32> GuildMatesCount = 0;
            Optional<uint32> MemberCount = 0;
            uint32 CompletedEncounters = 0;
            int32 CreationTime = 0;
            uint8 ApplicationStatus = 0;
            ObjectGuid PartyGUID;
            ObjectGuid BNetFriends;
            ObjectGuid CharacterFriends;
            ObjectGuid GuildMates;
            std::vector<MemberInfo> Members;
            ListRequest JoinRequest;
            bool Delisted = false;
            bool ChangeTitle = false;
            bool ChangeAutoAccept = false;
            bool ChangeHonorLevel = false;
            bool ChangePrivate = false;
        };

        class LfgListApplicantlistUpdate final : public ServerPacket
        {
        public:
            LfgListApplicantlistUpdate() : ServerPacket(SMSG_LFG_LIST_APPLICANT_LIST_UPDATE, 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            std::vector<ApplicantInfo> Applicants;
            LFG::RideTicket ApplicationTicket;
            uint32 UnkInt = 0;
        };

        class LfgListApplyToGroup final : public ClientPacket
        {
        public:
            LfgListApplyToGroup(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_APPLY_TO_GROUP, std::move(packet)) { }

            void Read() override;

            ApplicationToGroup application;
        };

        class LfgListCancelApplication final : public ClientPacket
        {
        public:
            LfgListCancelApplication(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_CANCEL_APPLICATION, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicantTicket;
        };

        class LfgListDeclineApplicant final : public ClientPacket
        {
        public:
            LfgListDeclineApplicant(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_DECLINE_APPLICANT, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicantTicket;
            LFG::RideTicket ApplicationTicket;
        };

        class LfgListInviteApplicant final : public ClientPacket
        {
        public:
            LfgListInviteApplicant(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_INVITE_APPLICANT, std::move(packet)) { }

            void Read() override;

            std::list<ApplicantStruct> Applicant;
            LFG::RideTicket ApplicantTicket;
            LFG::RideTicket ApplicationTicket;
        };

        class LfgListUpdateRequest final : public ClientPacket
        {
        public:
            LfgListUpdateRequest(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_UPDATE_REQUEST, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket Ticket;
            ListRequest UpdateRequest;
        };

        class LfgListGetStatus final : public ClientPacket
        {
        public:
            LfgListGetStatus(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_GET_STATUS, std::move(packet)) { }

            void Read() override { }
        };

        class LfgListInviteResponse final : public ClientPacket
        {
        public:
            LfgListInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicantTicket;
            bool Accept = false;
        };

        class LfgListJoin final : public ClientPacket
        {
        public:
            LfgListJoin(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_JOIN, std::move(packet)) { }

            void Read() override;

            ListRequest Request;
        };

        class LfgListLeave final : public ClientPacket
        {
        public:
            LfgListLeave(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_LEAVE, std::move(packet)) { }

            void Read() override;

            LFG::RideTicket ApplicationTicket;
        };

        class LfgListSearch final : public ClientPacket
        {
        public:
            LfgListSearch(WorldPacket&& packet) : ClientPacket(CMSG_LFG_LIST_SEARCH, std::move(packet)) { }

            void Read() override;

            std::vector<LFGListBlacklist> Blacklist;
            GuidVector PartyGUID;
            int32 GroupFinderCategoryId = 0;
            int32 SubActivityGroupID = 0;
            int32 LFGListFilter = 0;
            uint32 LanguageFilter = 0;
        };

        class RequestLfgListBlacklist final : public ClientPacket
        {
        public:
            RequestLfgListBlacklist(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_LFG_LIST_BLACKLIST, std::move(packet)) { }

            void Read() override  { }


        };

        class LfgListApplicationUpdate final : public ServerPacket
        {
        public:
            LfgListApplicationUpdate() : ServerPacket(SMSG_LFG_LIST_APPLICATION_STATUS_UPDATE) { }

            WorldPacket const* Write() override;

            LfgList::LfgListRideTicket ListRideTicket;
            LFG::RideTicket ApplicationTicket;
            uint32 UnkInt = 0;
            uint8 ResultId = 0;
            uint8 Role = 0;
            uint8 Status = 0;
        };

        class LfgListApplyToGroupResponce final : public ServerPacket
        {
        public:
            LfgListApplyToGroupResponce() : ServerPacket(SMSG_LFG_LIST_APPLY_TO_GROUP_RESULT) { }

            WorldPacket const* Write() override;

            ListSearchResult SearchResult;
            LFG::RideTicket ApplicantTicket;
            LFG::RideTicket ApplicationTicket;
            uint32 InviteExpireTimer = 0;
            uint8 Status = 0;
            uint8 Role = 0;
            uint8 ApplicationStatus = 0;
        };

        class LfgListJoinResult final : public ServerPacket
        {
        public:
            LfgListJoinResult() : ServerPacket(SMSG_LFG_LIST_JOIN_RESULT) { }

            WorldPacket const* Write() override;

            LFG::RideTicket ApplicationTicket;
            uint8 Status = 0;
            uint8 Result = 0;
        };

        class LfgListSearchResults final : public ServerPacket
        {
        public:
            LfgListSearchResults() : ServerPacket(SMSG_LFG_LIST_SEARCH_RESULTS) { }

            WorldPacket const* Write() override;

            std::vector<ListSearchResult> SearchResults;
            uint16 AppicationsCount = 0;
        };

        class LfgListSearchStatus final : public ServerPacket
        {
        public:
            LfgListSearchStatus() : ServerPacket(SMSG_LFG_LIST_SEARCH_STATUS, 30) { }

            WorldPacket const* Write() override;

            LFG::RideTicket Ticket;
            uint8 Status = 0;
            bool UnkBit = false;
        };

        class LfgListUpdateBlacklist final : public ServerPacket
        {
        public:
            LfgListUpdateBlacklist() : ServerPacket(SMSG_LFG_LIST_UPDATE_BLACKLIST) { }

            WorldPacket const* Write() override;

            uint32 BlacklistEntryCount = 0;
            std::vector<LFGListBlacklist> Blacklist;
        };

        class LfgListUpdateStatus final : public ServerPacket
        {
        public:
            LfgListUpdateStatus() : ServerPacket(SMSG_LFG_LIST_UPDATE_STATUS) { }

            WorldPacket const* Write() override;

            LFG::RideTicket ApplicationTicket;
            uint32 RemainingTime = 0;
            uint8 ResultId = 0;
            ListRequest Request;
            bool Listed = false;
        };

        class LfgListSearchResultUpdate final : public ServerPacket
        {
        public:
            LfgListSearchResultUpdate() : ServerPacket(SMSG_LFG_LIST_SEARCH_RESULTS_UPDATE) { }

            WorldPacket const* Write() override;

            Array<LfgListSearchResult, 50> ResultUpdate;
        };

        class LfgListUpdateExpiration final : public ServerPacket
        {
        public:
            LfgListUpdateExpiration() : ServerPacket(SMSG_LFG_LIST_UPDATE_EXPIRATION) { }

            WorldPacket const* Write() override;

            LFG::RideTicket ApplicationTicket;
            uint32 TimeoutTime = 0;
            uint8 Status = 0;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::LFGListBlacklist const& blackList);
ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LfgList::LFGListBlacklist& blackList);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::ListSearchResult const& listSearch);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::MemberInfo const& memberInfo);
ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LfgList::LfgListRideTicket& ticket);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::LfgListRideTicket const& ticket);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::LfgList::ListRequest const& join);
ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::LfgList::ListRequest& join);

#endif // LfgListPackets_h__
