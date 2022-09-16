#include <org.hpp>

// todo
ACTION org::recognize (name org_admin, name trusted_badge_contract) {
  require_auth (org_admin);
  
  require_recipient(CHECKS_CONTRACT);

  // remote action
}

ACTION org::initcoll (name org_admin, name collection_name) {
  require_auth (org_admin);
  
  require_recipient(CHECKS_CONTRACT);

  // remote action
}

ACTION org::createsimple (name creator, name badge, vector<name> parentbadge, string ipfsimage, string details, bool write_to_aa, bool write_cumulative) {
  require_auth(creator);

  require_recipient(CHECKS_CONTRACT);
  
  action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("create"),
    createsimple_args {
      .org = get_self(),
      .badge = badge,
      .parentbadge = parentbadge,
      .ipfsimage = ipfsimage,
      .details = details }
  }.send();
  
  // action addnotify 
  // action addnotify
}

ACTION org::creategotcha (name creator, name badge, time_point_sec starttime, uint64_t cycle_length, uint8_t max_cap, string ipfsimage, string details, bool write_to_aa, bool write_cumulative) {
  require_auth(creator);
  
  require_recipient(CHECKS_CONTRACT);
  
  action {
    permission_level{get_self(), name("active")},
    name("gotchabadge"),
    name("create"),
    creategotcha_args {
      .org = get_self(),
      .badge = badge,
      .starttime = starttime,
      .cycle_length = cycle_length,
      .max_cap = max_cap,
      .ipfsimage = ipfsimage,
      .details = details }
  }.send();

  // action addnotify 
  // action addnotify
}
/*
ACTION org::createrollup (name creator, name badge, vector<badge_count> rollup_criteria, string ipfsimage, string details) {
  require_auth(creator);

  require_recipient(checkscontract());

  action {
    permission_level{get_self(), name("active")},
    name("profiles"),
    name("createrollup"),
    createrollup_args {
      .org = get_self(),
      .badge = badge,
      .rollup_criteria = rollup_criteria,
      .ipfsimage = ipfsimage,
      .details = details }
  }.send();
}*/

ACTION org::givegotcha (name badge, name from, name to, uint8_t amount, string memo ) {
  require_auth(from);
  
  require_recipient(CHECKS_CONTRACT);

  action {
    permission_level{get_self(), name("active")},
    name("gotchabadge"),
    name("give"),
    givegotcha_args {
      .org = get_self(),
      .badge = badge,
      .from = from,
      .to = to,
      .amount = amount,
      .memo = memo }
  }.send();

}


ACTION org::givesimple (name from, name to, name badge, string memo ) {
  require_auth(from);

  require_recipient(CHECKS_CONTRACT);

  action {
    permission_level{get_self(), name("active")},
    name(SIMPLEBADGE_CONTRACT),
    name("give"),
    givesimple_args {
      .org = get_self(),
      .to = to,
      .badge = badge,
      .memo = memo }
  }.send();
}

/*
  ACTION org::takerollup (name actor, name badge) {
    require_auth(actor);

    require_recipient(checkscontract());
    
    action {
      permission_level{get_self(), name("active")},
      name("profiles"),
      name("takerollup"),
      rollup_args {
        .org = get_self(),
        .account = actor,
        .badge = badge}
    }.send();
  }*/


