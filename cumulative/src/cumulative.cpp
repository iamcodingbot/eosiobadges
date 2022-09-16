#include <cumulative.hpp>

void cumulative::notifyachiev(
  name org, 
  name badge_contract, 
  name badge_name, 
  uint64_t badge_id, 
  name account, 
  uint8_t count, 
  vector<name> notify_accounts) {

    achievements_table _achievements( _self, org.value );
    auto account_badge_index = _achievements.get_index<name("accountbadge")>();
    uint128_t account_badge_key = ((uint128_t) account.value) << 64 | badge_id;
    auto account_badge_iterator = account_badge_index.find (account_badge_key);

    if(account_badge_iterator == account_badge_index.end()) {
      _achievements.emplace(get_self(), [&](auto& row){
        row.id = _achievements.available_primary_key();
        row.account = account;
        row.badge_id = badge_id;
        row.count = count;
      });
      deduct_credit (org, 284, "cumulative_achievement: <org> ,<account>, <badge_id>, <count>");
    } else {
      account_badge_index.modify(account_badge_iterator, get_self(), [&](auto& row){
        row.count = row.count + count;
      });
    }
}

ACTION cumulative::dummy() {
  // created as a workaround for empty abi.
}


