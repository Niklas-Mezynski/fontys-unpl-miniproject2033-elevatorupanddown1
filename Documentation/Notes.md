# Notes for the elevator simulation

## Pseudo code for manager
- Initialize elevators
- Store important elevator information in a global variable
- Setup connection to the floors socket and a mailbox to store incoming messages.
-  while day is not over
    - Handle an event from the mailbox
        - Calculate which elevator is optimal in order to reach that floor.
        - Put that "goal" into the elevator target floor queue
        - Log necessary information about waitng/idle times
