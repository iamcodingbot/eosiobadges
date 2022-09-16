#include <orgbill.hpp>

// todo 
// 1) check for system currency in buycredits notified action.
// 2) buyram action from system currency.
// 3) put profiles contract in a variable.
// 4) error messages replace with value.
// 5) org's account to notify when credits run out.
// 6) separate out contract authorization and multisig authorization.
// recognize and addsettings are multisig authorizations.
// notify is contract authorization.

ACTION orgbill::recognize (name trusted_contract) {
  require_auth (get_self());
  authorized_contracts_table _authorized_contracts( _self, _self );
  auto itr = _authorized_contracts.find(trusted_contract.value);
  check(itr == _authorized_contracts.end(), "<trusted_contract> already authorized to issues badges");
  _authorized_contracts.emplace(get_self(), [&](auto& row){
    row.trusted_contract = trusted_contract;
  });
}

ACTION orgbill::addsettings (name key, uint32_t value) {
  require_auth(get_self());
  settings_table _settings(get_self(), get_self().value);
  auto itr = _settings.find(key.value);
  if(itr == _settings.end()) {
    _settings.emplace(get_self(), [&](auto& row) {
      row.key = key;
      row.value = value;
    });
  } else {
    _settings.modify(itr, get_self(), [&](auto& row) {
      row.value = value;
    });
  }
}

void orgbill::buycredits(name from, name to, asset quantity, string memo) {
  if (to != get_self() || from == get_self()){
    eosio::print("Ignoring token transfer of contract to itself and from itself.");
    return;
  }
  credits_table _credits(get_self(), get_self().value);
  auto itr = _credits.find(name(memo).value);
  uint32_t credits_bought = token_amount_to_credits (quantity.amount);

  if(itr == _credits.end()) {
    check(credits_bought <= 10000, "can not buy more than 10000 credits");
    _credits.emplace(get_self(), [&](auto& row) {
      row.org = name(memo);
      row.total_credits = credits_bought;
      row.used_credits = 1;
    });
  } else {
    _credits.modify(itr, get_self(), [&](auto& row) {
      check(row.total_credits + credits_bought <= 10000, "can not hold more than 10000 credits");
      row.total_credits = row.total_credits + credits_bought;
    });
  }
}

ACTION orgbill::syscredits(name org) {
  check_authorization( org, contract);
  uint32_t credits = getvalue(name(PER_USE_FIXED_FEES));
  deduct_credit (org, credits);
}

ACTION orgbill::ramcredits(name org, name contract, uint64_t bytes, string memo) {
  check_authorization( org, contract);
  uint32_t credits = bytes_to_credits(bytes);
  deduct_credit (org, credits);
}

void deduct_credit (name org, uint32_t credits) {
  credits_table _credits(get_self(), get_self().value);
  auto credits_itr = _credits.find(org.value);
  
  check(itr != _credits.end(), "<org> never registered for credits");
  check(itr->total_credits - itr->used_credits - credits >= 0, "Credits exhausted for <org>");
  
  _credits.modify(itr, get_self(), [&](auto& row) {
    row.used_credits = row.used_credits + credits_needed;
  });

  if(itr->total_credits / (itr->used_credits + credits_needed) < .9) {
    action {
      permission_level{get_self(), name("active")},
      get_self(),
      name("notify"),
      notify_args {
        .org = billed_org,
        .total_credits = itr->total_credits,
        .used_credits = itr->used_credits + credits_needed}
    }.send();
  } 
}

ACTION orgbill::notify (name org, uint32_t total_credits, uint32_t used_credits) {
  require_auth(get_self());
  require_recipient(org);
}

