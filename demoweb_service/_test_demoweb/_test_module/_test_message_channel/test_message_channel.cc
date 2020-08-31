/**
 * e8yes demo web.
 *
 * <p>Copyright (C) 2020 Chifeng Wen {daviesx66@gmail.com}
 *
 * <p>This program is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * <p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * <p>You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
 */

#include <optional>
#include <string>
#include <vector>

#include "common/unit_test_util/unit_test_util.h"
#include "demoweb_service/demoweb/common_entity/message_channel_entity.h"
#include "demoweb_service/demoweb/common_entity/user_entity.h"
#include "demoweb_service/demoweb/environment/test_environment_context.h"
#include "demoweb_service/demoweb/module/create_user.h"
#include "demoweb_service/demoweb/module/message_channel.h"
#include "proto_cc/message_channel.pb.h"
#include "proto_cc/pagination.pb.h"

static e8::UserId const kCreatorId = 1;
static constexpr char const *kChannelName = "channel1";
static constexpr char const *kChannelDesc = "Description of channel1";

struct CreateNewChannelInfo {
    e8::UserEntity creator;
    e8::MessageChannelEntity message_channel;
};

CreateNewChannelInfo CreateNewChannel(e8::DemoWebTestEnvironmentContext *env) {
    std::optional<e8::UserEntity> user = e8::CreateUser(
        /*security_key=*/"", std::vector<std::string>(), kCreatorId, env->CurrentHostId(),
        env->DemowebDatabase());

    e8::MessageChannelEntity channel = e8::CreateMessageChannel(
        kCreatorId, kChannelName, kChannelDesc,
        /*encrypted=*/false,
        /*close_group_channel=*/true, env->CurrentHostId(), env->DemowebDatabase());

    CreateNewChannelInfo info;
    info.creator = *user;
    info.message_channel = channel;
    return info;
}

bool CreateAndListMessageChannelTest() {
    e8::DemoWebTestEnvironmentContext env;

    CreateNewChannelInfo channel_info = CreateNewChannel(&env);

    e8::Pagination page1;
    page1.set_page_number(0);
    page1.set_result_per_page(2);
    std::vector<e8::JoinedInMessageChannel> retrieved_channel =
        e8::GetJoinedInMessageChannels(kCreatorId, page1, env.DemowebDatabase());

    TEST_CONDITION(retrieved_channel.size() == 1);
    TEST_CONDITION(*retrieved_channel[0].message_channel.id.Value() ==
                   *channel_info.message_channel.id.Value());
    TEST_CONDITION(*retrieved_channel[0].message_channel.channel_name.Value() == kChannelName);
    TEST_CONDITION(*retrieved_channel[0].message_channel.description.Value() == kChannelDesc);
    TEST_CONDITION(*retrieved_channel[0].message_channel.encryption_enabled.Value() == false);
    TEST_CONDITION(*retrieved_channel[0].message_channel.close_group_channel.Value() == true);
    TEST_CONDITION(retrieved_channel[0].member_type == e8::MessageChannelMemberType::MCMT_ADMIN);

    return true;
}

bool CreateAndListChannelMemberTest() {
    e8::DemoWebTestEnvironmentContext env;

    CreateNewChannelInfo channel_info = CreateNewChannel(&env);

    e8::Pagination page1;
    page1.set_page_number(0);
    page1.set_result_per_page(2);
    std::vector<e8::MessageChannelMember> retrieved_members = e8::GetMessageChannelMembers(
        *channel_info.message_channel.id.Value(), page1, env.DemowebDatabase());

    TEST_CONDITION(retrieved_members.size() == 1);
    TEST_CONDITION(*retrieved_members[0].member.id.Value() == kCreatorId);
    TEST_CONDITION(retrieved_members[0].member_type == e8::MessageChannelMemberType::MCMT_ADMIN);
    TEST_CONDITION(retrieved_members[0].join_at > 0);

    return true;
}

int main() {
    e8::BeginTestSuite("message_channel");
    e8::RunTest("CreateAndListMessageChannelTest", CreateAndListMessageChannelTest);
    e8::RunTest("CreateAndListChannelMemberTest", CreateAndListChannelMemberTest);
    e8::EndTestSuite();
    return 0;
}
