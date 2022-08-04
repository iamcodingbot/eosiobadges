#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

# define BYTES_CONSUMED_PER_CREDIT "bytespercr"
# define RAM_IN_BYTES_PER_SYS_TOKEN "ramrate"
# define PROFILE_CONTRACT_NAME "profiles"

CONTRACT orgbill : public contract {
  public:
    using contract::contract;
    struct notify_args {
      name org;
      uint32_t total_credits;
      uint32_t used_credits;
    };

    ACTION addsettings (name key, uint32_t value);

    [[eosio::on_notify("eosio.token::transfer")]] 
    void buycredits(name from, name to, asset quantity, string memo);
    ACTION usecredit(name billed_org, uint64_t bytes_consumed);
    ACTION notify (name org, uint32_t total_credits, uint32_t used_credits);
    

  private:
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

    uint8_t bytes_to_credits (uint64_t bytes) {
      settings_table _settings(get_self(), get_self().value);
      auto itr = _settings.find(name(BYTES_CONSUMED_PER_CREDIT).value);
      check(itr != _settings.end(), "Missing creditrate in bytes");

      uint32_t bytes_per_credit = itr->value;
      uint8_t credits_needed = bytes/bytes_per_credit + (bytes%bytes_per_credit != 0);
      return credits_needed;
    }

    uint32_t token_to_credits (uint64_t amount) {
      settings_table _settings(get_self(), get_self().value);
      auto itr = _settings.find(name(RAM_IN_BYTES_PER_SYS_TOKEN).value);
      check(itr != _settings.end(), "Missing ramrate per unit chain token");
      // todo bug here possibly
      uint64_t bytes_bought = itr->value * amount/10000;
      uint32_t credits_bought = bytes_to_credits (bytes_bought);
      return credits_bought;
    }

};