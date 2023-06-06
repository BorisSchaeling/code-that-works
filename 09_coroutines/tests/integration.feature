Feature: Integration

Scenario: Achieve consensus between users on different hosts

  Given a server with the blockchain
    | data  |
    | hello |
  And 3 clients with the blockchain
    | data  |
  When they sychronize
  Then the server blockchain contains
    | data  |
    | hello |
  And the client blockchains contain
    | data  |
    | hello |
