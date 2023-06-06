Feature: Transaction Handling

Scenario: Add a NFT

  Given 1 user with an empty blockchain
  And user 0 makes a transaction with the NFT hello to user 0
  Then the blockchain of user 0 contains
    | data  | signedby | owner |
    | hello | 0        | 0     |

Scenario: Transfer ownership

  Given 2 users with an empty blockchain
  And user 0 makes a transaction with the NFT hello to user 1
  Then the blockchain of user 0 contains
    | data  | signedby | owner |
    | hello | 0        | 1     |
  And the blockchain of user 1 contains
    | data  | signedby | owner |
    | hello | 0        | 1     |

Scenario: Failed ownership transfer

  Given 3 users with an empty blockchain
  And user 0 makes a transaction with the NFT hello to user 1
  And user 1 makes a transaction with the NFT hello to user 2
  And user 0 makes a transaction with the NFT hello to user 2
  Then the blockchain of user 0 contains
    | data  | signedby | owner |
    | hello | 0        | 1     |
    | hello | 0        | 2     |
  And the blockchain of user 1 contains
    | data  | signedby | owner |
    | hello | 0        | 1     |
    | hello | 1        | 2     |
  And the blockchain of user 2 contains
    | data  | signedby | owner |
    | hello | 1        | 2     |
