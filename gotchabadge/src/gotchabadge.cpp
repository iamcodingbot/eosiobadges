#include <gotchabadge.hpp>

  ACTION gotchabadge::create (name org, name badge, time_point_sec starttime, uint64_t cycle_length, uint8_t max_cap, string ipfsimage, string details) {
    require_auth(org);

    metadata_table _metadata (_self, org.value);
    
    // todo add in all badges
    auto badge_itr = _metadata.find(badge.value);
    check(badge_itr == _metadata.end(), "<badge> already exists");

    _metadata.emplace(org, [&](auto& row) {
      row.badge = badge;
      row.starttime = starttime;
      row.cycle_length = cycle_length;
      row.last_known_cycle_start = starttime;
      row.last_known_cycle_end = starttime + cycle_length - 1;
      row.max_cap = max_cap;
      row.ipfsimage = ipfsimage;
      row.details = details;
    });

    action {
      permission_level{get_self(), name("active")},
      name("profiles"),
      name("initbadge"),
      initbadge_args {
        .org = org,
        .badge = badge,
        .ipfs = ipfsimage,
        .details = details}
    }.send();
  }

    ACTION gotchabadge::give (name org, name badge, name from, name to, uint8_t amount, string memo ) {
    require_auth(org);

    require_recipient(from);
    require_recipient(to);
    // check from not equals to.

    metadata_table _metadata (_self, org.value);
    auto badge_itr = _metadata.require_find(badge.value, "Not a valid gotcha badge");
    time_point_sec last_known_cycle_start = badge_itr->last_known_cycle_start;
    time_point_sec last_known_cycle_end = badge_itr->last_known_cycle_end;
    uint64_t cycle_length = badge_itr->cycle_length;
    uint8_t max_cap = badge_itr->max_cap;

    time_point_sec current_time = time_point_sec(current_time_point());
    check(current_time >= last_known_cycle_start, "can not give this badge yet");

    bool current_cycle_found = last_known_cycle_start <= current_time && last_known_cycle_end >= current_time;
    bool cycle_update_needed = false;

    while(!current_cycle_found) {
      cycle_update_needed = true; // optimize this.
      last_known_cycle_start = last_known_cycle_start + cycle_length;
      last_known_cycle_end = last_known_cycle_end + cycle_length;
      current_cycle_found = last_known_cycle_start <= current_time && last_known_cycle_end >= current_time;
    }

    time_point_sec current_cycle_start = last_known_cycle_start;
    time_point_sec current_cycle_end = last_known_cycle_end;

    if(cycle_update_needed) {
      _metadata.modify(badge_itr, org, [&](auto& row) {
        row.last_known_cycle_end = current_cycle_end;
        row.last_known_cycle_start = current_cycle_start;
      }); 
    } 


    stats_table _stats (_self, org.value);
    auto account_badge_index = _stats.get_index<name("accountbadge")>();
    uint128_t account_badge_key = ((uint128_t) from.value) << 64 | badge.value;
    auto account_badge_iterator = account_badge_index.find (account_badge_key);

    if(account_badge_iterator == account_badge_index.end()) {
      _stats.emplace(org, [&](auto& row) {
        row.id = _stats.available_primary_key();
        row.account = from;
        row.badge = badge;
        row.balance = amount;
        row.last_claimed_time = current_time;
      });
    } else if (current_cycle_start <= account_badge_iterator->last_claimed_time) {
      check(account_badge_iterator->balance + amount <= max_cap, "Overdrawn balance");
      account_badge_index.modify(account_badge_iterator, org, [&](auto& row) {
        row.balance = row.balance + amount;
        row.last_claimed_time = current_time;
      });
    } else {
      account_badge_index.modify(account_badge_iterator, org, [&](auto& row) {
        row.balance = amount;
        row.last_claimed_time = current_time;
      });
    }

    action {
      permission_level{get_self(), name("active")},
      name("profiles"),
      name("achievement"),
      achievement_args {
        .org = org,
        .badge = badge,
        .account = to,
        .count = amount}
    }.send();

  }

 
