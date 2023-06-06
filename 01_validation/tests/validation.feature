Feature: Blockchain Validation

Scenario: Validate a blockchain

  Given an empty blockchain
  When a block with a valid hash is added
  Then validation should pass

Scenario: Detect an invalid block in the blockchain

  Given an empty blockchain
  When a block with an invalid hash is added
  Then validation should fail
