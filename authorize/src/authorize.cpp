#include <authorize.hpp>

ACTION authorize::recognize (name org, name trusted_contract) {
  require_auth (org);
  authorized_contracts_table _authorized_contracts( _self, org.value );
  auto itr = _authorized_contracts.find(trusted_contract.value);
  check(itr == _authorized_contracts.end(), "<trusted_contract> already authorized to issues badges");
  _authorized_contracts.emplace(get_self(), [&](auto& row){
    row.trusted_contract = trusted_contract;
  });
}

ACTION authorize::initbadge (name org, name contract, name badge, string ipfs, string details) {
  check_authorization(org, contract);
  action {
    permission_level{get_self(), name("active")},
    name(METADATA_CONTRACT),
    name("initbadge"),
    initbadge_args {
      .org = org,
      .contract = contract,
      .badge = badge,
      .ipfs = ipfs,
      .details = details
    }
  }.send();
}

ACTION authorize::achievement (name org, name contract, name badge, name account, uint8_t count) {
  check_authorization(org, contract);
  action {
    permission_level{get_self(), name("active")},
    name(METADATA_CONTRACT),
    name("achievement"),
    achievement_args {
      .org = org,
      .contract = auth_details.authorized_account,
      .badge = badge,
      .account = account,
      .count = count
    }
  }.send();
}
