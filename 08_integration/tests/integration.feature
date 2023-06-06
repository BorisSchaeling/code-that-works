Feature: Integration

Scenario: Achieve consensus between users on different hosts

  Given a server with the blockchain
    | data  |
    | hello |
  And a client with the blockchain
    | data  |
  When they sychronize
  Then the server's blockchain contains
    | data  |
    | hello |
  And the client's blockchain contains
    | data  |
    | hello |
