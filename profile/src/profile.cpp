#include <profile.hpp>
#include <queue>

ACTION profile::authcontract (name org, name trusted_contract) {
  require_auth (org);
  authorized_contracts_table _authorized_contracts( _self, org.value );
  auto itr = _authorized_contracts.find(trusted_contract.value);
  check(itr == _authorized_contracts.end(), "<trusted_contract> already authorized to issues badges");
  _authorized_contracts.emplace(get_self(), [&](auto& row){
    row.trusted_contract = trusted_contract;
  });
}

ACTION profile::achievement (name org, name badge, name account, uint8_t count) {
  authorization_details auth_details = auth(org);
  check(auth_details.is_authorized, "Action called by an unauthorized contract. Add this contract through authcontract action");
  uint64_t badge_id = get_update_badgedata_state(org, badge, auth_details.authorized_account, count);
  add_achievement(org, account, badge_id, count);
}

ACTION profile::initbadge (name org, name badge, string ipfs, string details) {
  authorization_details auth_details = auth(org);
  check(auth_details.is_authorized, "Action called by an unauthorized contract. Add this contract through authcontract action");
  badgedata_table _badgedata( _self, org.value );
  auto contract_badge_index = _badgedata.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) auth_details.authorized_account.value) << 64 | badge.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);

  check(contract_badge_iterator == contract_badge_index.end(), "<badge> already exists for <contract> ");

  _badgedata.emplace(get_self(), [&](auto& row){
    row.badge_id = _badgedata.available_primary_key();
    row.contract = auth_details.authorized_account;
    row.badge = badge;
    row.ipfs = ipfs;
    row.details = details;
    row.rarity_counts = 0;
  });
}


