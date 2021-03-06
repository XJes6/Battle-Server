# Final Project: Battle-Server
____
_Team: Jesse Nhan & Ronald Washington III_

_Due: May 14, 2018_

_CS 375: Computer Networks_

For this project, we are developing our own implementation of a multi-player
network game, _Battle-Server_. Within this game four clients are joining a
simulation where they are communicating their tactics to defeat a monster (Boss)
and must use a limited set of moves during the battle. This network game
utilizes multiple I_O, processes concurrency to relative addresses, and take
in input.
____
## Server.c
### Program Structure and Design

* ***typedef struct Monster{char* Name; int HP; int PATK;}****
    > This definition enables us to be able to identify and later use Monster
    stats.

* ***typedef struct Client{char* Name; int HP; int PATK; int fd; int DEF;}***
    > This definition enables us to be able to identify and later use Client
    stats.

* ***Monster MonsterGen(Monster M, int i, int j, int fdmax, int listener,
  fd_set master)***
    > The purpose of this function is to initialize the creation of a Monster.
    Within here the attributes associated with the Monster stats are created
    and is later prepared to sending data for use in battle.

* ***void acceptance(int i)***
   > This helper function allows the game to recognize when all players have
   been acknowledged in the simulation as well as transfers the data sent in
   the chat room.


* ***Client PlayerGen(Client P, int i, int j, int fdmax, int listener,
   fd_set master)***
     > The purpose of this function is to initialize the creation of a Player.
     Within here the attributes associated with the Monster stats are created
     and is later prepared to sending data for use in battle.

* ***Client Player_stats(Queue *history, Client Player, Monster M)***
   > This helper function aides the clients in battle by recognizing standard
   input for defending monster attack and self healing.

* ***void End_Game(Monster M, Client Player, int j, int fdmax, int listener,
  fd_set master)***
   > The purpose of this function is to recognize whether the player/client has
   'won' or 'lost' the game and send a message as such notice.

* ***void *get_in_addr(struct sockaddr *sa)***
   > The purpose of this function is to gather the addresses of the clients.

* ***int main(void)***
   > Here is where we implement the usage of Select to perform the actions of
   the clients and servers for multiple Inputs and Outputs. Select
   typically utilizes a list of file descriptions to recognize who and what it
   is performing actions for whether it be the number of clients or for the
   server itself. Once the server and the amount of connections has
   been established, the program then beings to accept up to four clients. when
   has clients have been recognized then the battle simulation commences
   where the monster and player generators are initialized. A queue is used to
   track the input of options that the clients/players choose, and Select then
   recognizes these inputs to determine who chose what behavior and enact it
   upon the monster within the server. Once the Select has discovered the action chosen
   by each client then the action is then broadcasted to all clients. 
   The server is influenced by the choices and the remaining clients are
   simply notified. Additionally, a chat system has been developed so that
   between choosing their battle options, clients are able to communicate
   before facing the enemy. Once a choice has been chosen, that client will
   not be able to type or chat anymore until all four are done choosing but 
   the client can still listen to other clients.
____
## Client.c
### Program Structure and Design

* ***void *get_in_addr(struct sockaddr *sa)***
   > The purpose of this function is to gather the address of the server.

* ***int main(void)***
   > Here we are essentially opening our connectoin and gathering
   the clients who are apart of the game. The client simply uses Select by 
   utilizing the standard in and and the information that it gathers from 
   the server. Based on these two sources we are able to determine when there 
   is an acception to the connection and then allow for commands such as 
   Attack, Defend, and Heal to be collected to then be sent to the server program.
____
## Implementation Challenges
* We struggled with ensuring all the bits were being transfered between clients.
* Had issues with implementing the queue to recognize and save the different
  choices made by individual clients.
* Had issues with implementing the healing and defending options for clients
  due to the order of their input countering the other's health for said client.
* Had issues with implementing functions for the bases of the main function due
  to our initial version of the project being done altogether.
* We also struggled with how the server was sending messages to the clients.
  Only a few of the clients would get the message while others would get nothing.
____
## Testing
* Consecutively damage from Monster to reach end game (Lose)
* Consistently attacking Monster to read end game (Win)
* Checking against the properties of heal and defend to work differently
* Ensuring chat works by putting input from each client one by one and
  making sure each person receives all comments from other clients.
* Ensuring protocols worked correctly from the server to the client.
____

## Remaining Bugs
* The are still some possible memory leaks within our program.
  We tried to suppress all errors down to a minimum.
* It is possible that if an alien had inhuman speed to put two inputs
  at the same time, it may crash our game.
* After closing the server or the game, the ending messages would repeat.
____
