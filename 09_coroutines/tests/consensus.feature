Feature: Blockchain Consensus Mechanism

Scenario: Achieve consensus between users

  Given 2 users with an empty blockchain
  When blocks are added to the blockchain of user 0
    | data  |
    | hello |
  And user 0 and user 1 synchronize their blockchains
  Then the blockchain of user 0 contains
    | data  |
    | hello |
  And the blockchain of user 1 contains
    | data  |
    | hello |
