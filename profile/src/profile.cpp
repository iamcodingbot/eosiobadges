#include <profile.hpp>

// todo 
// 1) move credits logic to orgbill
// 2) fill error messages.
// 3) check moving profile data to a separate contract and use this contract as orchestrator.
// 4) rename details/name of badge

//ACTION profile::writetoaa (name org, name contract, name badge, bool write) {
//  require_auth (org);
  // toggle write to aa
//}



ACTION profile::initcoll (name org, name collection_name) {
  require_auth( org );
  aacollection_table _aacollection ( _self, _self.value);
  auto itr = _aacollection.find (org.value);
  check (itr == _aacollection.end(), "<org> already has a <collection_name> allocated");
  _aacollection.emplace(get_self(), [&](auto& row){
    row.org = org;
    row.collection = collection_name;
  });

  vector<name> authorized_accounts ;
  authorized_accounts.push_back(get_self());

  vector<name> notify_accounts ;
  notify_accounts.push_back(get_self());

  std::map <std::string, ATOMIC_ATTRIBUTE> data;
  
  action {
    permission_level{get_self(), name("active")},
    name(ATOMIC_ASSETS_CONTRACT),
    name("createcol"),
    createcol_args {
      .author = get_self(),
      .collection_name = collection_name,
      .allow_notify = true,
      .authorized_accounts = authorized_accounts,
      .notify_accounts = notify_accounts,
      .market_fee = 0.00,
      .data = data}
  }.send();

  vector <FORMAT> schema_format;
  schema_format.push_back( FORMAT{.name = "name", .type = "string"});
  schema_format.push_back(FORMAT{.name = "contract",.type = "string"});
  schema_format.push_back(FORMAT{.name = "badge",.type = "string"});
  schema_format.push_back(FORMAT{.name = "img",.type = "string"});
  schema_format.push_back(FORMAT{.name = "details",.type = "string"});


  action {
    permission_level{get_self(), name("active")},
    name(ATOMIC_ASSETS_CONTRACT),
    name("createschema"),
    createschema_args {
      .authorized_creator = get_self(),
      .collection_name = collection_name,
      .schema_name = name(ATOMIC_ASSETS_SCHEMA_NAME),
      .schema_format = schema_format}
  }.send();
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
  check (is_account(account), "profile : <account> is not a valid account");
  authorization_details auth_details = auth(org);
  check(auth_details.is_authorized, "Action called by an unauthorized contract. Add this contract through authcontract action");
  check_account_prefs (org, account); 
  badgedata_details bd_details = get_update_badgedata_state(org, badge, auth_details.authorized_account, count);
  
  uint64_t native_bytes_used = write_native_achievement(org, account, bd_details.badge_id, count);
  uint8_t aa_bytes_used = write_aa_achievement(org, account, bd_details.write_to_aa, bd_details.aa_template_id, count);
  
  deduct_credit (org, native_bytes_used + aa_bytes_used);
  
}


ACTION profile::initbadge (name org, name badge, string ipfs, string details, bool write_to_aa) {
  authorization_details auth_details = auth(org);
  check(auth_details.is_authorized, "Action called by an unauthorized contract. Add this contract through authcontract action");
  
  uint64_t native_bytes_used = write_native_badgedata( org, auth_details.authorized_account, badge, ipfs, details, write_to_aa);
  uint64_t aa_bytes_used = write_aa_template(org, auth_details.authorized_account, badge, ipfs, details, write_to_aa);

  deduct_credit (org, native_bytes_used + aa_bytes_used);
 
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

  aacollection_table _aacollection ( _self, _self.value);
  auto collection_index = _aacollection.get_index<name("colkey")>();
  auto collection_iterator = collection_index.require_find (collection_name.value, "somethings wrong, collection not found");
  name org = collection_iterator->org;
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