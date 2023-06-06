Feature: Blockchain Validation

Scenario: Validate a blockchain

  Given an empty blockchain
  When a block with a valid hash is added
  Then validation should pass

Scenario: Detect an invalid block in the blockchain

  Given an empty blockchain
  When a block with an invalid hash is added
  Then validation should fail

Scenario: Add a block with data

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
  Then validation should pass

Scenario: Tamper with data in first block

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
  And data in block 0 is changed to abc
  Then validation should fail

Scenario: Tamper with data in second block

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
    | world |
  And data in block 1 is changed to abc
  Then validation should fail
