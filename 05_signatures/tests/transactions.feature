Feature: Transaction Handling

Scenario: Add a NFT

  Given an empty blockchain
  And 1 user
  And user 0 adds the NFT hello
  Then the blocks exist
    | data  | signedby | owner |
    | hello | 0        | 0     |

Scenario: Transfer ownership

  Given an empty blockchain
  And 2 users
  And user 0 adds the NFT hello
  And user 0 transfers the NFT hello to user 1
  Then the blocks exist
    | data  | signedby | owner |
    | hello | 0        | 0     |
    | hello | 0        | 1     |
