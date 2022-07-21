#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

using namespace std;
using namespace eosio;


CONTRACT profile : public contract {
  public:
    using contract::contract;

  struct authorization_details {
    bool is_authorized;
    name authorized_account;
  };

  struct usecredits_args {
    name source_contract;
    name billed_org;
    uint8_t credits_needed;
  };
  
  ACTION authcontract (name org, name trusted_contract);

  ACTION initbadge(name org, name badge, string ipfs, string details);

  ACTION achievement (name org, name badge, name account, uint8_t count);

  private:
    // scoped by org
    // stores trusted and adopted badge contracts.
    TABLE authorized {
      name trusted_contract;
      auto primary_key() const {return trusted_contract.value; }
    };
    typedef multi_index<name("authorized"), authorized> authorized_contracts_table;

    // scoped by org
    // stores metadata information for a badge issued by trusted/adopted badge contract
    TABLE badgedata {
      uint64_t badge_id;
      name contract;
      name badge;
      string ipfs;
      string details;
      uint32_t rarity_counts;
      auto primary_key() const {return badge.value; }
      uint128_t contract_badge_key() const {
        return ((uint128_t) contract.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("badgedata"), badgedata,
    indexed_by<name("contractbadge"), const_mem_fun<badgedata, uint128_t, &badgedata::contract_badge_key>>    
    > badgedata_table;
    
    // scoped by org
    TABLE achievements {
      uint64_t id;
      name account;
      uint64_t badge_id;
      uint32_t count;
      auto primary_key() const {return id; }
      uint128_t acc_badge_key() const {
        return ((uint128_t) account.value) << 64 | badge_id;
      }
    };
    typedef multi_index<name("achievements"), achievements,
    indexed_by<name("accountbadge"), const_mem_fun<achievements, uint128_t, &achievements::acc_badge_key>>
    > achievements_table;


    uint64_t get_update_badgedata_state (name org, name badge, name contract, uint8_t count) {
      badgedata_table _badgedata( _self, org.value );
      auto contract_badge_index = _badgedata.get_index<name("contractbadge")>();
      uint128_t contract_badge_key = ((uint128_t) contract.value) << 64 | badge.value;
      auto contract_badge_iterator = contract_badge_index.find (contract_badge_key);

      check(contract_badge_iterator != contract_badge_index.end(), "<org> has not created <badge> for <contract>");

      contract_badge_index.modify(contract_badge_iterator, get_self(), [&](auto& row){
        row.rarity_counts = row.rarity_counts + count;
      });
      return contract_badge_iterator->badge_id;
    }


    authorization_details auth (name org) {
      authorized_contracts_table _authorized_contracts( _self, org.value );
      for(auto itr = _authorized_contracts.begin(); itr != _authorized_contracts.end(); ++itr ) {
        if (has_auth(itr->trusted_contract)) {
          return authorization_details {
            .is_authorized = true,
            .authorized_account = itr->trusted_contract
          };
        }
      }
      return authorization_details {
        .is_authorized = false
      };
    }

    void add_achievement(name org, name account, uint64_t badge_id, uint8_t count) {
      achievements_table _achievements( _self, org.value );
      auto account_badge_index = _achievements.get_index<name("accountbadge")>();
      uint128_t account_badge_key = ((uint128_t) account.value) << 64 | badge_id;
      auto account_badge_iterator = account_badge_index.find (account_badge_key);

      if(account_badge_iterator == account_badge_index.end()) {
        _achievements.emplace(org, [&](auto& row){
          row.id = _achievements.available_primary_key();
          row.account = account;
          row.badge_id = badge_id;
          row.count = count;
        });
        // use credit
        action {
          permission_level{get_self(), name("active")},
          name("orgbill"),
          name("usecredits"),
          usecredits_args {
            .source_contract = get_self(),
            .billed_org = org,
            .credits_needed = 1 }
        }.send();
      } else {
        account_badge_index.modify(account_badge_iterator, org, [&](auto& row){
          row.count = row.count + count;
        });
      }
    }
};
