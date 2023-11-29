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

#pragma once

#include "Common.h"
#include "Group.h"
#include "Player.h"
#include "DB2Structure.h"

static uint16 const LFG_LIST_APPLY_FOR_GROUP_TIMEOUT = 300;
static uint8 const LFG_LIST_INVITE_TO_GROUP_TIMEOUT = 60;
static uint16 const LFG_LIST_GROUP_TIMEOUT = 30 * MINUTE;
static uint8 const LFG_LIST_MAX_APPLICATIONS = 5;

enum LocaleConstantFlags : uint32
{
    enUS = (1 << 0),
    koKR = (1 << 1),
    frFR = (1 << 2),
    deDE = (1 << 3),
    zhCN = (1 << 4),
    zhTW = (1 << 5),
    esES = (1 << 6),
    esMX = (1 << 7),
    ruRU = (1 << 8),
    none = (1 << 9),
    ptBR = (1 << 10),
    itIT = (1 << 11),
};

enum LfgListFilter
{
    Recommended = 1,
    NotRecommended = 2,
    PvE = 4,
    PvP = 8,
};

enum class LFGListApplicationStatus : uint8
{
    None = 0,
    Applied = 1,
    Invited = 2,
    Failed = 3,
    Cancelled = 4,
    Declined = 5,
    DeclinedFull = 6,
    DeclinedDelisted = 7,
    Timeout = 8,
    InviteDeclined = 9,
    InviteAccepted = 10,
};

enum LFGListActivityCategory
{
    LFG_LIST_ACTIVITY_CATEGORY_QUESTING                      = 1,
    LFG_LIST_ACTIVITY_CATEGORY_DUNGEON                       = 2,
    LFG_LIST_ACTIVITY_CATEGORY_RAIDS                         = 3,
    LFG_LIST_ACTIVITY_CATEGORY_ARENAS                        = 4,
    LFG_LIST_ACTIVITY_CATEGORY_SCENARIOS                     = 5,
    LFG_LIST_ACTIVITY_CATEGORY_CUSTOM                        = 6,
    LFG_LIST_ACTIVITY_CATEGORY_ARENA_SKIRMISHES              = 7,
    LFG_LIST_ACTIVITY_CATEGORY_BATTLEGROUNDS                 = 8,
    LFG_LIST_ACTIVITY_CATEGORY_RATED_BATTLEGROUNDS           = 9,
    LFG_LIST_ACTIVITY_CATEGORY_ISLAND_EXPEDITIONS            = 111,
    LFG_LIST_ACTIVITY_CATEGORY_THORGAST                      = 113

};

enum class LFGListStatus : uint8
{
    None = 0,
    Joined = 6,

    LFG_LIST_STATUS_LFG_LIST_LIST_ENTRY_EXPIRED_TIMEOUT = 44, // LFG_LIST_ENTRY_EXPIRED_TIMEOUT
    LFG_LIST_STATUS_APPLICATION_STATUS_UPDATED = 59, // LFG_LIST_APPLICATION_STATUS_UPDATED
    LFG_LIST_STATUS_RESTRICTED_ACCOUNT_TRIAL = 65, // ERR_RESTRICTED_ACCOUNT_LFG_LIST_TRIAL

    //LFG_LIST_STATUS_ERR_LFG_LIST_MEMBERS_NOT_PRESENT                  = 16,
    LFG_LIST_STATUS_ERR_LFG_LIST_GROUP_FULL = 30,
    LFG_LIST_STATUS_ERR_LFG_LIST_NO_LFG_LIST_OBJECT = 32,
    LFG_LIST_STATUS_ERR_LFG_LIST_NO_SLOTS_PLAYER = 33,
    LFG_LIST_STATUS_ERR_LFG_LIST_MISMATCHED_SLOTS = 34,
    LFG_LIST_STATUS_ERR_LFG_LIST_MISMATCHED_SLOTS_LOCAL_XREALM = 54,
    LFG_LIST_STATUS_ERR_LFG_LIST_PARTY_PLAYERS_FROM_DIFFERENT_REALMS = 35,
    LFG_LIST_STATUS_ERR_LFG_LIST_MEMBERS_NOT_PRESENT = 36,
    LFG_LIST_STATUS_ERR_LFG_LIST_GET_INFO_TIMEOUT = 37,
    LFG_LIST_STATUS_ERR_LFG_LIST_INVALID_SLOT = 38,
    LFG_LIST_STATUS_ERR_LFG_LIST_DESERTER_PLAYER = 39,
    LFG_LIST_STATUS_ERR_LFG_LIST_DESERTER_PARTY = 40,
    LFG_LIST_STATUS_ERR_LFG_LIST_RANDOM_COOLDOWN_PLAYER = 41,
    LFG_LIST_STATUS_ERR_LFG_LIST_RANDOM_COOLDOWN_PARTY = 42,
    LFG_LIST_STATUS_ERR_LFG_LIST_TOO_MANY_MEMBERS = 43,
    //LFG_LIST_STATUS_ERR_LFG_LIST_CANT_USE_DUNGEONS                    = 44,
    LFG_LIST_STATUS_ERR_LFG_LIST_ROLE_CHECK_FAILED = 45,
    LFG_LIST_STATUS_ERR_LFG_LIST_TOO_FEW_MEMBERS = 51,
    LFG_LIST_STATUS_ERR_LFG_LIST_REASON_TOO_MANY_LFG_LIST = 52,
    LFG_LIST_STATUS_ERR_ALREADY_USING_LFG_LIST_LIST = 62,
    //LFG_LIST_STATUS_ERR_USER_SQUELCHED                                = 65,
};

class Player;
class Group;

struct LFGListEntry
{
    struct LFGListApplicationEntry
    {
        LFGListApplicationEntry(ObjectGuid::LowType playerGuid, LFGListEntry* owner);

        LFGListEntry* m_Owner;
        ObjectGuid::LowType PlayerGuid;
        uint32 ID;
        uint32 ApplicationTime;
        uint32 Timeout;
        LFGListStatus Status;
        std::string Comment;
        LFGListApplicationStatus ApplicationStatus;
        uint8 RoleMask;
        bool Listed;

        Player* GetPlayer() const;
        void ResetTimeout();
        bool Update(uint32 const diff);
    };

    LFGListEntry();

    uint32 LocaleFlags = 0;

    bool IsApplied(ObjectGuid::LowType guid) const;
    bool IsApplied(Player* player) const;

    void BroadcastApplicantUpdate(LFGListApplicationEntry const* applicant);

    LFGListApplicationEntry* GetApplicant(ObjectGuid::LowType id);
    LFGListApplicationEntry* GetApplicantByPlayerGUID(ObjectGuid::LowType Guid);

    bool Update(uint32 const diff);
    void ResetTimeout();

    uint32 GetID() const;

    std::map<ObjectGuid::LowType, LFGListApplicationEntry> ApplicationsContainer;
    GroupFinderActivityEntry const* GroupFinderActivityData;
    Group* ApplicationGroup;

    uint32 Timeout = 0;
    uint32 CreationTime = 0;

    Optional<uint32> QuestID = 0;;
    Optional<uint32> MythicPlusRating = 0;
    int32 ActivityID = 0;
    uint32 HonorLevel = 0;
    uint32 PvPRating = 0;
    uint32 ItemLevel = 0;
    uint8 PlayStyle = 0; // LFG_PLAYSTYLE_PVP, LFG_PLAYSTYLE_PVE, LFG_PLAYSTYLE_PVE_MYTHICZERO
    std::string GroupName;
    std::string Comment;
    std::string VoiceChat;
    bool AutoAccept = false;
    bool PrivateGroup = false;
    bool VoiceChatReq = false;
    bool IsCrossFaction = false;
};
