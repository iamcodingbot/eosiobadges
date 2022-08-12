# eosiobadges

## Intro
This suite of smart contracts is a framework for organizations to distribute Badges to its members. A Badge is a non-transferable token whose supply can vary from 1 to very high. 

Badges are defined in badge contracts. Badge contract can be seen as a factory to create and issue Badges.

Following 3 badge contracts are available out-of-the-box -

### Simple Badge 
Simple Badge smart contract allows an organization to initialize and issue a simple badge. When a simple badge is issued to an account, all its parent badges are also issued to that account.

### Gotcha Badge 
Gotcha badges are defined with a max issuance supply and duration cycle. At the end of the duration cycle, issuance capacity for an account is restored to max.

### Rollup Badge 
Rollup badge is defined by setting up one or more [badge, count] combinations. An account can claim a rollup badge, if it holds badges for all defined combinations.



Organization would need to buy Credits to operate with the framework. Credits are used when badges are created and issued. Cumulative account balance of badges is maintained in Profile smart contract and granular per badge information can be maintained in Atomic Assets. 



## Organization onboarding steps

Organization onboarding is broadly 3 steps - 
 
Setup members list and roles :
- Deploy Org contract.
- Create a checks contract to validate members and inputs.
- Call Init action on orgs contract to link checks contract from above.

Pay smart contract for service:
- Buy Credits via sending tokens to orgbill contract.

Selecting types of badges:
- Choose set of trusted badge contracts in profile contract.

## Receiver onboarding step

For an account to receive a badge, it must whitelist issuing organization in userprefs contract


## Key design considerations 

- Minimum code write for an organization to onboard. 

- Optimal storage. 
Organizations can choose to store badges for its members either cumulatively or both cumulatively and granularly. Storing cumulative balance, where applicable, reduces storage costs.

- Permissionless integration of new badge contracts and organizations.
