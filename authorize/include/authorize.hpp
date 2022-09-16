#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT authorize : public contract {
  public:
    using contract::contract;

    struct authorization_details {
      bool is_authorized;
      name authorized_account;
    };

    struct initbadge_args {
      name org;
      name contract;
      name badge;
      string ipfs;
      string details;
    }

    struct achievement_args {
      name org;
      name contract;
      name badge;
      name account;
      uint8_t count;    
    }

    ACTION recognize (name org, name trusted_contract);
    ACTION initbadge (name org, name contract, name badge, string ipfs, string details);
    ACTION achievement (name org, name contract, name badge, name account, uint8_t count);

  private:
    TABLE authorized {
      name trusted_contract;
      auto primary_key() const {return trusted_contract.value; }
    };
    typedef multi_index<name("authorized"), authorized> authorized_contracts_table;

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
};
