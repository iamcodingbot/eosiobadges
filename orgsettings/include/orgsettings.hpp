#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

#define AUTHORIZED_CONTRACT = "authorized"

// orchestra

CONTRACT orgsettings : public contract {
  public:
    using contract::contract;

    ACTION recognize (name org, name contract_name);

    ACTION addnotify (name org, name notification_account);
    
    ACTION pause (name org, name contract, name badge);

    ACTION resume (name org, name contract, name badge);

    ACTION initbadge(name org, );
    // check subscription
    // create a badge, remote
    // call billing
    ACTION achievement();
    // check subscription
    // call cumulative, granular
    // call billing for granular

  private:
    TABLE authorized {
      name trusted_contract;
      auto primary_key() const {return trusted_contract.value; }
    };
    typedef multi_index<name("authorized"), authorized> authorized_contracts_table;
    // bytes used, credits used.
    // addMembers
    // addCreators
    // Issuers (badge)
    // One time meeting.
    // Multi day meeting.
    // Time, Limit (10 hr, 10 badges, number of cycles, gap).
    // scoped by org
    // stores metadata information for a badge issued by trusted/adopted badge contract
    // name, discovery_id
    // require_recipient();
    // 
    TABLE badge {
      uint64_t badge_id;
      name contract;
      name badge;
      vector<name> stream_sinks;
      vector<name> notify_accounts;
      vector<string> ipfs;
      vector<string> details;
      uint32_t rarity_counts;
      auto primary_key() const {return badge_id; }
      uint128_t contract_badge_key() const {
        return ((uint128_t) contract.value) << 64 | badge.value;
      }
    };
    typedef multi_index<name("badge"), badge,
    indexed_by<name("contractbadge"), const_mem_fun<badge, uint128_t, &badge::contract_badge_key>>    
    > badgedata_table;


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
};
