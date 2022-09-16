#include <metadata.hpp>

ACTION metadata::recognize (name org, name trusted_contract) {
  require_auth (org);
  authorized_contracts_table _authorized_contracts( _self, org.value );
  auto itr = _authorized_contracts.find(trusted_contract.value);
  check(itr == _authorized_contracts.end(), "<trusted_contract> already authorized to issues badges");
  _authorized_contracts.emplace(get_self(), [&](auto& row){
    row.trusted_contract = trusted_contract;
  });
}

ACTION metadata::initbadge (name org, name badge_contract, name badge_name, string ipfs_image, string memo) {
  check_authorization(org, contract);
  init(org, contract, badge, ipfs);
}

ACTION metadata::addnotify (name org, name badge_contract, name badge_name, name notify_account, string memo) {
  require_auth(org);
  
  badge_table _badge( _self, org.value );
  auto contract_badge_index = _badge.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) badge_contract.value) << 64 | badge_name.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);
  check(contract_badge_iterator != contract_badge_index.end(), "<contractname>,<action name> : <org> <contract> <badge> not found");
  
  vector<name> new_notify_accounts ;
  for( auto i = 0; i < contract_badge_iterator->notify_accounts.size(); i++) {
    check(notify_account != notify_accounts[i], "<thiscontractname>,<action name> : <contract> <badge> is already ")
    new_notify_accounts.push_back(notify_accounts[i]);
  }
  new_notify_accounts.push_back (notify_account);
  contract_badge_index.modify(contract_badge_iterator, get_self(), [&](auto& row){
    row.notify_accounts = new_notify_accounts;
  });

  vector<name> accounts_to_be_notified;
  accounts_to_be_notified.push_back(notify_account);
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("notifyinit"),
    notifyinit_args {
      .org = org,
      .badge_contract = contract_badge_iterator->badge_contract,
      .badge_name = contract_badge_iterator->badge_name,
      .notify_accounts = accounts_to_be_notified,
      .memo = memo,
      .badge_id = contract_badge_iterator->badge_id,
      .ipfs_hashes = ipfs_hashes,
      .rarity_counts = rarity_counts
      }
  }.send();

}

ACTION metadata::notifyinit( 
  name org,
  name badge_contract,
  name badge_name,
  vector<name> notify_accounts,
  string memo, 
  uint64_t badge_id, 
  vector<ipfs_hash> ipfs_hashes,
  uint32_t rarity_counts) {
  require_auth(get_self());
  for(auto i = 0; i < notify_accounts.size(); i++) {
    require_recipient (notify_accounts[i]);
  }
}


ACTION metadata::achievement (name org, name badge_contract, name badge_name, name account, uint8_t count, string memo) {
  check_authorization (org, badge_contract);
  check_account_prefs (org, account);
  
  badge_table _badge( _self, org.value );
  auto contract_badge_index = _badge.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) badge_contract.value) << 64 | badge_name.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);
  check(contract_badge_iterator != contract_badge_index.end(), "<contractname>,<action name> : <org> <contract> <badge> not found");
 
  action {
    permission_level{get_self(), name("active")},
    get_self(),
    name("notifyachiev"),
    notifyachievement_args {
      .org = org,
      .badge_contract = badge_contract,
      .badge_name = badge_name,
      .account = account,
      .count = count,
      .memo = memo,
      .badge_id = contract_badge_iterator->badge_id,
      .notify_accounts = contract_badge_iterator->notify_accounts}
  }.send();

  deduct_platform_fees (org);

}

ACTION metadata::notifyachiev (name org, 
  name badge_contract, 
  name badge_name,
  name account, 
  uint8_t count,
  string memo,
  uint64_t badge_id,  
  vector<name> notify_accounts) {
  require_auth(get_self());
  for(auto i = 0; i < notify_accounts.size(); i++) {
    require_recipient (notify_accounts[i]);
  }
}

ACTION metadata::removenotify (name org, name badge_contract, name badge_name, name notify_account, string memo) {
  require_auth(org);
  
  badge_table _badge( _self, org.value );
  auto contract_badge_index = _badge.get_index<name("contractbadge")>();
  uint128_t contract_badge_key = ((uint128_t) badge_contract.value) << 64 | badge_name.value;
  auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);
  check(contract_badge_iterator != contract_badge_index.end(), "<contractname>,<action name> : <org> <contract> <badge> not found");

  vector<name> new_notify_accounts ;
  for( auto i = 0; i < contract_badge_iterator->notify_accounts.size(); i++) {
    if(notify_account != notify_accounts[i]) {
      new_notify_accounts.push_back(notify_accounts[i]);
    } else {
      action {
        permission_level{get_self(), name("active")},
        get_self(),
        name("notifyinit"),
        notifyinit_args {
          .org = org,
          .badge_contract = contract_badge_iterator->badge_contract,
          .badge_name = contract_badge_iterator->badge_name,
          .notify_account = notify_account,
          .memo = memo,
          .badge_id = contract_badge_iterator->badge_id,
          .ipfs_hashes = ipfs_hashes,
          .rarity_counts = rarity_counts
          }
      }.send();
    }
  }
}

