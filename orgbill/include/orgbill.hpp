#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

# define BYTES_PER_CREDIT "bytespercr"
# define RAM_IN_BYTES_PER_SYS_TOKEN "ramrate"
# define PER_USE_FIXED_FEES "protocolfees"

# define PROFILE_CONTRACT_NAME "openprof.gm"

CONTRACT orgbill : public contract {
  public:
    using contract::contract;
    struct notify_args {
      name org;
      uint32_t total_credits;
      uint32_t used_credits;
    };

    ACTION addsettings (name key, uint32_t value);
    ACTION recognize (name trusted_contract);

    [[eosio::on_notify("eosio.token::transfer")]] 
    void buycredits(name from, name to, asset quantity, string memo);
    ACTION syscredits(name org);
    ACTION ramcredits(name org, name contract, uint64_t bytes, string memo);
    ACTION notify (name org, uint32_t total_credits, uint32_t used_credits);
    

  private:

    TABLE authorized {
      name trusted_contract;
      auto primary_key() const {return trusted_contract.value; }
    };
    typedef multi_index<name("authorized"), authorized> authorized_contracts_table;

    bool check_authorization (name source_contract) {
      authorized_contracts_table _authorized_contracts( _self, _self );
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
    
    TABLE credits {
      name    org;
      uint32_t  total_credits;
      uint32_t used_credits;
      auto primary_key() const { return org.value; }
    };
    typedef multi_index<name("credits"), credits> credits_table;

    TABLE settings {
      name key;
      uint32_t value;
      auto primary_key() const { return key.value; }
    };
    typedef multi_index<name("settings"), settings> settings_table;

    uint32_t bytes_to_credits (uint64_t bytes) {
      uint32_t bytes_per_credit = getvalue(name(BYTES_PER_CREDIT));
      uint32_t credits = bytes/bytes_per_credit + (bytes%bytes_per_credit != 0);
      return credits;
    }

    uint32_t token_amount_to_credits (uint64_t amount) {
      uint64_t bytes = getvalue(name(RAM_IN_BYTES_PER_SYS_TOKEN)) * amount/10000;
      uint32_t credits = bytes_to_credits (bytes_bought);
      return credits;
    }

    uint32_t getvalue (name key) {
      settings_table _settings(get_self(), get_self().value);
      auto itr = _settings.find(name(BYTES_PER_CREDIT).value);
      check(itr != _settings.end(), "Missing <key> in settings table");
      return itr->value;
    }

};