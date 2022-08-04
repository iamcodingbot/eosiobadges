#include <profile.hpp>

// todo 
// 1) move credits logic to orgbill
// 2) fill error messages.
// 3) check moving profile data to a separate contract and use this contract as orchestrator.

//ACTION profile::writetoaa (name org, name contract, name badge, bool write) {
//  require_auth (org);
  // toggle write to aa
//}


ACTION profile::del (name org) {
  badgedata_table _badgedata( _self, org.value );
  for( auto itr = _badgedata.begin(); itr != _badgedata.end();) {
    itr = _badgedata.erase(itr);
  }
}


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
  check_account_prefs (org, account); 
  badgedata_details bd_details = get_update_badgedata_state(org, badge, auth_details.authorized_account, count);
  
  uint8_t native_rows_inserted = write_native_achievement(org, account, bd_details.badge_id, count);
  uint8_t aa_rows_inserted = write_aa_achievement(account, bd_details.write_to_aa, bd_details.aa_template_id, count);
  
  if(native_rows_inserted + aa_rows_inserted > 0) {
    uint64_t data_bytes = achievement_bytes();
    deduct_credit (org, (data_bytes) * (native_rows_inserted + aa_rows_inserted));
  }
}


ACTION profile::initbadge (name org, name badge, string ipfs, string details, bool write_to_aa) {
  authorization_details auth_details = auth(org);
  check(auth_details.is_authorized, "Action called by an unauthorized contract. Add this contract through authcontract action");
  
  uint8_t native_rows_inserted = write_native_badgedata( org, auth_details.authorized_account, badge, ipfs, details, write_to_aa);
  uint8_t aa_rows_inserted = write_aa_template(org, auth_details.authorized_account, badge, ipfs, details, write_to_aa);
  
  if(native_rows_inserted + aa_rows_inserted > 0) {
    uint64_t data_bytes = init_bytes(details, ipfs);
    deduct_credit (org, (data_bytes) * (native_rows_inserted + aa_rows_inserted));
  }
}


void profile::updatebadge(
    int32_t template_id,
    name authorized_creator,
    name collection_name,
    name schema_name,
    bool transferable,
    bool burnable,
    uint32_t max_supply,
    ATTRIBUTE_MAP immutable_data) {

  
  name org = name(get<string>(immutable_data["org"]));
  name badge = name(get<string>(immutable_data["badge"]));
  name contract= name(get<string>(immutable_data["contract"]));
  badgedata_table _badgedata( _self, org.value );
  auto contract_badge_index = _badgedata.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) contract.value) << 64 | badge.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);

  check (contract_badge_iterator != contract_badge_index.end(), "somethings wrong, not found");
  contract_badge_index.modify(contract_badge_iterator, get_self(), [&](auto& row){
    row.aa_template_id = template_id;
  });
}