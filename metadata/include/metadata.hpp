#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define BILLING_CONTRACT "billing"
#define ACCOUNT_PREFERENCES_CONTRACT ""

CONTRACT metadata : public contract {
  public:
    using contract::contract;

  struct ipfs_hash {
    string key;
    string value;
  };
 
  ACTION recognize (name org, name trusted_badge_contract);
  
  ACTION initbadge (name org, 
    name badge_contract, 
    name badge_name, 
    string ipfs_image, 
    string memo);

  ACTION notifyinit(
    name org,
    name badge_contract,
    name badge_name,
    name notify_account,
    string memo, 
    uint64_t badge_id, 
    vector<ipfs_hash> ipfs_hashes,
    uint32_t rarity_counts);
  
  ACTION achievement (name org,
    name badge_contract,
    name badge_name,
    name account,
    uint8_t count,
    string memo);

  ACTION notifyachiev (name org, 
    name badge_contract, 
    name badge_name,
    name account, 
    uint8_t count,
    string memo,
    uint64_t badge_id,  
    vector<name> notify_accounts);
  
  ACTION addnotify (name org, 
    name badge_contract,
    name badge_name,
    name notify_account,
    string memo);

  ACTION removenotify (name org, 
    name badge_contract,
    name badge_name,
    name notify_account,
    string memo);

  private:

    struct ramcredits_arg {
      name org;
      name contract;
      uint64_t bytes;
      string memo;
    };

    struct syscredits_arg {
      name org;
    };

    struct notifyachievement_args {
      name org;
      name badge_contract;
      name badge_name;
      name account;
      uint64_t count;
      string memo;
      uint64_t badge_id;
      vector<name> notify_accounts;
    };

    struct notifyinit_args {
      name org;
      name badge_contract;
      name badge_name;
      vector<name> notify_accounts;
      string memo;
      uint64_t badge_id;
      vector<ipfs_hash> ipfs_hashes;
      uint64_t rarity_counts;
    };

    TABLE authorized {
      name trusted_contract;
      auto primary_key() const {return trusted_contract.value; }
    };
    typedef multi_index<name("authorized"), authorized> authorized_contracts_table;

    TABLE badge {
      uint64_t badge_id;
      name badge_contract;
      name badge_name;
      vector<name> notify_accounts;
      vector<ipfs_hash> ipfs_hashes;
      uint32_t rarity_counts;
      auto primary_key() const {return badge_id; }
      uint128_t contract_badge_key() const {
        return ((uint128_t) contract.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("badge"), badge,
    indexed_by<name("contractbadge"), const_mem_fun<badge, uint128_t, &badge::contract_badge_key>>    
    > badge_table;
    
    bool check_authorization (name org, name source_contract) {
      authorized_contracts_table _authorized_contracts( _self, org.value );
      for(auto itr = _authorized_contracts.begin(); itr != _authorized_contracts.end(); ++itr ) {
        if (has_auth(itr->trusted_contract)) {
          if (itr->trusted_contract == source_contract) {
            return true;
          } else {
            check (false, "<source_contract> is not same as <trusted_contract>");
          }
        }
      }
      check(false, "action does not have authorization of any trusted contract");
    }

    void init (name org, name badge_contract, name badge_name, string ipfs_image) {
      badge_table _badge( _self, org.value );
      auto contract_badge_index = _badge.get_index<name("contractbadge")>();
      uint128_t contract_badge_key = ((uint128_t) badge_contract.value) << 64 | badge_name.value;
      auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);
      check(contract_badge_iterator == contract_badge_index.end(), "<badge> already exists for <contract> ");
      vector<ipfs_hash> ipfs_hashes;
      ipfs_hashes.push_back ( ipfs_hash {
        .key = "img",
        .value = ipfs_image
      });
      _badge.emplace(get_self(), [&](auto& row){
        row.badge_id = _badge.available_primary_key();
        row.contract = contract;
        row.badge = badge;
        row.ipfs_hashes = ipfs_hashes;
        row.rarity_counts = 0;
      });
      deduct_credit (org, 291 + ipfs_image.length()), "native init : <org> <contract> <badge>";
    }

    void deduct_credit (name org, uint32_t bytes, string memo) {
      action {
        permission_level{get_self(), name("active")},
        name(BILLING_CONTRACT),
        name("ramcredits"),
        ramcredits_arg {
          .org = org,
          .contract = get_self(),
          .bytes = bytes,
          .memo = memo}
      }.send();
    }

    void deduct_platform_fees (name org) {
      action {
        permission_level{get_self(), name("active")},
        name(BILLING_CONTRACT),
        name("sysredits"),
        syscredits_arg {
          .org = get_self()}
      }.send();
    }

    void check_account_prefs (name org, name account) {
      action {
        permission_level{get_self(), name("active")},
        name(ACCOUNT_PREFERENCES_CONTRACT),
        name("checkallow"),
        checkallow_args {
          .org = org,
          .account = account }
      }.send();
    }
};
