# eosiobadges

##### design considerations - 
##### minimum code write for orgs using badges.
##### minimum need of redeploy of smart contracts.
##### ram to be payed by org whose members issue the badges.
##### easily pluggable new badge contracts.
##### ??



### FAQs

#### I am an organization, what can i do with this?

#### How can my organization use this?

#### What are these 6 smart contracts for?

#### How to integrate new badge contract?





## CONTRACT - profile 

### TABLES
#### authorized
| # | Column Name      | Type | Index   | Description                                                  |
|---|------------------|------|---------|--------------------------------------------------------------|
| 1 | trusted_contract | name | primary | Holds name of trusted contracts that can execute actions A,B |
#### badgedata
| # | Column Name | Type     | Index   | Description                            |
|---|-------------|----------|---------|----------------------------------------|
| 1 | badge_id    | uint64_t | primary | id for row in this table               |
| 2 | contract    | name     |         | contract name which created this badge |
#### achievements
### ACTIONS
#### authcontract (name org, name trusted_contract)
##### requires authority of org.
#### initbadge(name org, name badge, string ipfs, string details)
##### requires authority of one of the trusted_contracts
#### achievement (name org, name badge, name account, uint8_t count)
##### requires authority of one of the trusted_contracts

## CONTRACT - org

### tables
### actions

## CONTRACT - orgbill

### tables
### actions
