#include <simplebadge.hpp>

  ACTION simplebadge::create (name org, name badge, vector<name> parentbadge, string ipfsimage, string details) {
    require_auth(org);

    metadata_table _metadata (_self, org.value);
    auto badge_itr = _metadata.find(badge.value);

    check(badge_itr == _metadata.end(), "<badge> already exists");
    for(auto i = 0; i < parentbadge.size(); i++) { 
      auto parentbadge_itr = _metadata.require_find(parentbadge[i].value, "<parent badge> not found");
    }
    // todo - check for cycles
    _metadata.emplace(org, [&](auto& row) {
      row.badge = badge;
      row.parentbadge = parentbadge;
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


  ACTION simplebadge::give (name org, name to, name badge, string memo ) {
    require_auth(org);

    require_recipient(to);

    metadata_table _metadata (_self, org.value);
    auto badge_itr = _metadata.require_find(badge.value, "invalid badge");
    vector<name> all_badges;
    queue<name> _helper_queue;

    all_badges.push_back(badge);
    for(auto i = 0; i < badge_itr->parentbadge.size(); i++) {
      _helper_queue.push(badge_itr->parentbadge[i]);
    }

    while(!_helper_queue.empty()) {
      auto parent_itr = _metadata.require_find(_helper_queue.front().value, "invalid parent badge");
      all_badges.push_back(_helper_queue.front()); 
      _helper_queue.pop();
      for(auto i = 0; i < parent_itr->parentbadge.size(); i++) {
          _helper_queue.push(parent_itr->parentbadge[i]);
      }
    }

    for (auto i = 0 ; i < all_badges.size() ; i++ ) {
      action {
        permission_level{get_self(), name("active")},
        name("profiles"),
        name("achievement"),
        achievement_args {
          .org = org,
          .badge = all_badges[i],
          .account = to,
          .count = 1}
      }.send();
    }
    
  }

    
