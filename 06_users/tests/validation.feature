Feature: Blockchain Validation

Scenario: Validate a blockchain

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
  Then validation should pass

Scenario: Tamper with data in block 0

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
  And data in block 0 is changed to abc
  Then validation should fail

Scenario: Tamper with data in block 1

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
    | world |
  And data in block 1 is changed to abc
  Then validation should fail

Scenario: Tamper with hash in block 0

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
  And hash in block 0 is changed to 123
  Then validation should fail

Scenario: Tamper with hash in block 1

  Given an empty blockchain
  When blocks are added
    | data  |
    | hello |
    | world |
  And hash in block 1 is changed to 123
  Then validation should fail
