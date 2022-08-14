#include <gotchabadge.hpp>
// todos
// toggle write to aa.
// pause issuance.
// resume issuance.
// auth contracts
// change duration.
// change supply per duration.


  ACTION gotchabadge::create (name org, 
    name badge, 
    time_point_sec starttime, 
    uint64_t cycle_length, 
    uint8_t supply_per_cycle, 
    string ipfsimage, 
    string display_name,
    string details, 
    bool write_to_aa) {
    require_auth(org);

      metadata_table _metadata (_self, org.value);
      
      // todo add in all badges
      auto badge_itr = _metadata.find(badge.value);
      check(badge_itr == _metadata.end(), "<badge> with this name already exists");

      // duration of cycle is in seconds.
      // for duration of 60 secs starting at time 100 
      // cycle start, cycle end
      // 100, 159
      // 160, 219  
      _metadata.emplace(org, [&](auto& row) {
        row.badge = badge;
        row.starttime = starttime;
        row.cycle_length = cycle_length;
        row.last_known_cycle_start = starttime;
        row.last_known_cycle_end = starttime + cycle_length - 1;
        row.supply_per_cycle = supply_per_cycle;
        row.ipfsimage = ipfsimage;
        row.details = details;
        row.write_to_aa = write_to_aa;
      });

      action {
        permission_level{get_self(), name("active")},
        name(PROFILES_CONTRACT),
        name("initbadge"),
        initbadge_args {
          .org = org,
          .badge = badge,
          .ipfs = ipfsimage,
          .display_name = display_name,
          .details = details,
          .write_to_aa =write_to_aa}
      }.send();
  }

  ACTION gotchabadge::give (name org, name badge, name from, name to, uint8_t amount, string memo ) {
    require_auth(org);

    require_recipient(from);
    require_recipient(to);
    check(from != to, "can not be same - from, to");

    metadata_table _metadata (_self, org.value);
    auto badge_itr = _metadata.require_find(badge.value, "Not a valid gotcha badge");
    time_point_sec last_known_cycle_start = badge_itr->last_known_cycle_start;
    time_point_sec last_known_cycle_end = badge_itr->last_known_cycle_end;
    uint64_t cycle_length = badge_itr->cycle_length;
    uint8_t supply_per_cycle = badge_itr->supply_per_cycle;
    time_point_sec starttime = badge_itr->starttime;
    time_point_sec current_time = time_point_sec(current_time_point());
    check(current_time < starttime, "can not give this badge yet");

    // find current cycle start and end.

    bool current_cycle_found = last_known_cycle_start <= current_time && last_known_cycle_end >= current_time;
    bool cycle_update_needed = false;

    while(!current_cycle_found) {
      cycle_update_needed = true; // optimize this.
      last_known_cycle_start = last_known_cycle_start + cycle_length;
      last_known_cycle_end = last_known_cycle_end + cycle_length;
      current_cycle_found = last_known_cycle_start <= current_time && last_known_cycle_end >= current_time;
    }
    //// update current cycle in local metadata
    if(cycle_update_needed) {
      _metadata.modify(badge_itr, org, [&](auto& row) {
        row.last_known_cycle_end = last_known_cycle_end;
        row.last_known_cycle_start = last_known_cycle_start;
      }); 
    } 

    time_point_sec current_cycle_start = last_known_cycle_start;
    time_point_sec current_cycle_end = last_known_cycle_end;


    // check and update balance in local stats

    stats_table _stats (_self, org.value);
    auto account_badge_index = _stats.get_index<name("accountbadge")>();
    uint128_t account_badge_key = ((uint128_t) from.value) << 64 | badge.value;
    auto account_badge_iterator = account_badge_index.find (account_badge_key);

    if(account_badge_iterator == account_badge_index.end()) {
      check(amount <= supply_per_cycle, "<amount> exceeds available <supply_per_cycle>. Can only issue <supply_per_cycle> <badge>");
      _stats.emplace(org, [&](auto& row) {
        row.id = _stats.available_primary_key();
        row.account = from;
        row.badge = badge;
        row.balance = amount;
        row.last_claimed_time = current_time;
      });
    } else if (current_cycle_start <= account_badge_iterator->last_claimed_time) {
      check(account_badge_iterator->balance + amount <= supply_per_cycle, "<balance + amount> exceeds available <supply_per_cycle>. Can only issue <supply_per_cycle - balance> <badge>");
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

    //  remote balance update and badge issuance
    action {
      permission_level{get_self(), name("active")},
      name(PROFILES_CONTRACT),
      name("achievement"),
      achievement_args {
        .org = org,
        .badge = badge,
        .account = to,
        .count = amount}
    }.send();

  

  }

 
