#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "math.h"

#define NUM_ROUNDS 13
#define NUM_SUITS 4
#define CARDS_PER_DECK 52

#define MIN_NUM_PLAYERS 2
#define MAX_NUM_PLAYERS 10

#define GAME_DECK_MAX_DECKS 3
//#define MAX_GAME_DECK_SIZE CARDS_PER_DECK * GAME_DECK_MAX_DECKS
#define MAX_GAME_DECK_SIZE 156

#define MIN_CARD_VALUE 2
#define MAX_CARD_VALUE 14

typedef struct Suit {
	int id;
	char name[10];
} Suit; // represents a suit

typedef struct Card {
	int cardId;
	Suit suit;
	int value;
	char cardName[10];
	char fullName[100];
	int isDealtToPlayer;
	int isPlayed;
} Card; // represents a card, which has a suit and a value

// PLAYERS
typedef struct Player {
	//char playerName[100];
	int score;
	Card cards[NUM_ROUNDS];
} Player; // represents a player, with an id, name, score and hand of cards

typedef struct ResultSet {
	// Result set for each value of card
	int value; // result set for this card value

	Card lastMatchedCard;
	int lastMatchedCardHolderIndex;
	int hits;
} ResultSet;

int numPlayers;
int numDecks;
int deckSize;

int numCurrentRound;

Player gamePlayers[MAX_NUM_PLAYERS];
Card gameDeck[MAX_GAME_DECK_SIZE];
Card roundCards[MAX_NUM_PLAYERS];
Suit gameSuits[4];

ResultSet cardValueHits[MAX_CARD_VALUE];
int highestScore;

//// Prototypes
// Game
void setupGame(); // set up cards and players
void startGame(); // begins the first turn
void addPlayersToGame();


void calculateRoundScores();
void displayScore();
void showHighestScore();
void printDeck();

// Cards
void buildSuits();
void buildGameDeck();
Card getRandomCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize);
Card getRandomSuitedCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize, int suitId);
void dealAllPlayersCards();


void main() {

	// create a seed for rng
	srand((unsigned)time(NULL));

	int validSelection = 0;
	// load game or start new game
	printf("Welcome\nPlease make a selection: ");

	// main game loop
	do {

		// ask whether the player would like to start a new game, load a game or exit
		printf("\n1. New Game");
		printf("\n2. Load Game");
		printf("\n3. Exit Game");

		int userSelection = 0;
		printf("\nUser selection: ");
		scanf("%d", &userSelection);

		switch (userSelection)
		{
		case 1:
			printf("\n\nNew game selected!");

			// START A NEW GAME - game.c
			setupGame();
			startGame();

			break;
		case 2:
			printf("\nLoad game selected");
			break;
		case 3:
			printf("\nExit game selected");
			printf("\n\nDone!\n\n");
			return;
			//break;
		default:
			printf("\nNot a valid selection!");
			break;
		}
	} while (validSelection == 0);

	printf("\n\nDone!\n\n");
}

// setup a new game
void setupGame() {

	// (TODO: rewrite this function?)add the correct amount of players to the game
	addPlayersToGame();

	// calculate the number of decks to use - must always be at least 4 suits
	printf("\n\n!!!!!!!!!  %d !!!!!!!!!\n", numDecks);
	numDecks = (int)ceil((double)numPlayers / 4);
	printf("\n%d deck(s) will be used", numDecks);
	deckSize = numDecks * CARDS_PER_DECK;

	// shuffle the deck

	// TODO: check if game is loading
	if (1 == 1) {
		// NEW GAME, NOT LOADING

		// populate the gameDeck array with the cards, 
		// using the suits and number of decks - deck.c
		buildGameDeck();
		printf("\nGame Deck built!\n");

		// deal the cards
		dealAllPlayersCards();
		printf("\nCards dealt to all players!\n");


	}
	else {
		// LOADING GAME

	}
}

void startGame() {

	Player currentPlayer;
	// each round consists of turns and scoring, followed by a final round(?)

	int currentPlayerIndex = 0;
	int playerCardIndex = 0;

	// ROUNDS
	for (numCurrentRound = 1; numCurrentRound <= NUM_ROUNDS; numCurrentRound++) {
		printf("\n\nROUND %d STARTING", numCurrentRound);

		//// ROUND TURNS
		// each player takes a turn in order
		for (currentPlayerIndex = 0; currentPlayerIndex < numPlayers; currentPlayerIndex++) {
			currentPlayer = gamePlayers[currentPlayerIndex];

			//TODO:  verify card has not already been played
			int selectedCardNumber = 0;
			int isValidCard = -1;

			Card c;

			// loop until a valid card has been selected, i.e. index in range and isPlayed == 0
			do {
				printf("\n\nPlayer %d's turn", currentPlayerIndex + 1);
				// for each card the player is holding
				for (playerCardIndex = 0; playerCardIndex < NUM_ROUNDS; playerCardIndex++) {
					// only show cards which have not yet been played
					if (currentPlayer.cards[playerCardIndex].isPlayed == 0) {
						// show the cards
						printf("\n# %d - %s (%d)", playerCardIndex + 1, currentPlayer.cards[playerCardIndex].fullName, currentPlayer.cards[playerCardIndex].value);
					}
				}

				// ask the player to select a card
				printf("\nSelect a card(1 -13): ");
				scanf("%d", &selectedCardNumber);

				// card has already been played
				if (gamePlayers[currentPlayerIndex].cards[selectedCardNumber - 1].isPlayed > 0) {
					printf("\nNot a valid selection"); // LOOP
				}
				// play the card
				else {
					printf("\nPlayer %d played the %s\n", currentPlayerIndex + 1, gamePlayers[currentPlayerIndex].cards[selectedCardNumber - 1].fullName);
					// mark the player's card as played
					gamePlayers[currentPlayerIndex].cards[selectedCardNumber - 1].isPlayed = 1;
					// and add the card to the round cards
					roundCards[currentPlayerIndex] = gamePlayers[currentPlayerIndex].cards[selectedCardNumber - 1];
					isValidCard = 1; // EXIT DO WHILE
				}

			} while (isValidCard == -1);
			printf("\n\nROUND %d ENDING", numCurrentRound);
		}
		//// END ROUND TURNS

		//// CALCULATE END ROUND SCORE

		//int highestUniqueScore = 0; // highest unique score this round
		//int winnerIndex = -1; // if >= 0, then the index of the round winner in the gamePlayers array
		//

		//// set up the cardValueHits array and fill with zeroes,
		//// each element represents a card value, values represent numHits
		//// only array elements 2-14 are used based on MIN_CARD_VALUE and MAX_CARD_VALUE constants
		//int cardValueHitIndex; 
		//int cardValue = MIN_CARD_VALUE;
		//for (cardValueHitIndex = MIN_CARD_VALUE; cardValueHitIndex <= MAX_CARD_VALUE; cardValueHitIndex++) {
		//	cardValueHits[cardValueHitIndex].hits = 0;
		//	cardValueHits[cardValueHitIndex].value = cardValue;
		//	cardValue += 1;

		//}

		//int roundCardIndex = 0;
		//for (roundCardIndex = 0; roundCardIndex < numPlayers; roundCardIndex++) {
		//	// increment the hit counter for each card value found
		//	cardValueHits[roundCards[roundCardIndex].value].hits += 1;
		//	// round card indx is the same as game player index
		//	cardValueHits[roundCards[roundCardIndex].value].lastMatchedCardHolderIndex = roundCardIndex;
		//}

		//int lastMatched = -1;
		//// go back through the result list and keep the highest value with 1 hit, also its lastMatched
		//for (int i = 0; i <= 14; i++) {
		//	if (cardValueHits[i].hits == 1 && i > highestUniqueScore) {
		//		//if (hitArray[i] == 1 && i > highestUniqueScore) {
		//		highestUniqueScore = i;
		//		lastMatched = cardValueHits[i].lastMatchedCardHolderIndex;
		//	}
		//}

		//winnerIndex = lastMatched;

		////// END CALCULATE END ROUND SCORE

		//// show the cards on the board and who played them
		//printf("\n");
		//for (int i = 0; i < numPlayers; i++) {
		//	printf("\n\tPlayer %d - %s (%d)", i + 1, roundCards[i].fullName, roundCards[i].value);
		//}

		//// calculate the score
		//if (winnerIndex > -1) {
		//	printf("\n\n\tHighest Unique score: %d", highestUniqueScore);
		//	printf("\n\tRound WINNER is: Player %d", winnerIndex + 1);
		//	// increment the winning player's score
		//	gamePlayers[winnerIndex].score += highestUniqueScore;
		//}
		//else {
		//	printf("\nNo winner this round!\n");
		//}

		//// show the total score of each player
		//printf("\n\nRound Scores");
		//for (int i = 0; i < numPlayers; i++) {
		//	if (i == winnerIndex) {
		//		// if it's the winner, show the new points
		//		printf("\n\tPlayer %d: %d (+%d)", i + 1, gamePlayers[i].score, highestUniqueScore);
		//	}
		//	else {
		//		printf("\n\tPlayer %d: %d", i + 1, gamePlayers[i].score);
		//	}
		//}

		calculateRoundScores();

		printf("\n\nROUND FINISHED");
		printf("\n\nSAVE, EXIT, ETC");
	}

	printf("\n\nGAME OVER");

	showHighestScore();

}

void calculateRoundScores() {
	int highestUniqueScore = 0; // highest unique score this round
	int winnerIndex = -1; // if >= 0, then the index of the round winner in the gamePlayers array


						  // set up the cardValueHits array and fill with zeroes,
						  // each element represents a card value, values represent numHits
						  // only array elements 2-14 are used based on MIN_CARD_VALUE and MAX_CARD_VALUE constants
	int cardValueHitIndex;
	int cardValue = MIN_CARD_VALUE;
	for (cardValueHitIndex = MIN_CARD_VALUE; cardValueHitIndex <= MAX_CARD_VALUE; cardValueHitIndex++) {
		cardValueHits[cardValueHitIndex].hits = 0;
		cardValueHits[cardValueHitIndex].value = cardValue;
		cardValue += 1;

	}

	int roundCardIndex = 0;
	for (roundCardIndex = 0; roundCardIndex < numPlayers; roundCardIndex++) {
		// increment the hit counter for each card value found
		cardValueHits[roundCards[roundCardIndex].value].hits += 1;
		// round card indx is the same as game player index
		cardValueHits[roundCards[roundCardIndex].value].lastMatchedCardHolderIndex = roundCardIndex;
	}

	int lastMatched = -1;
	// go back through the result list and keep the highest value with 1 hit, also its lastMatched
	for (int i = 0; i <= 14; i++) {
		if (cardValueHits[i].hits == 1 && i > highestUniqueScore) {
			//if (hitArray[i] == 1 && i > highestUniqueScore) {
			highestUniqueScore = i;
			lastMatched = cardValueHits[i].lastMatchedCardHolderIndex;
		}
	}

	winnerIndex = lastMatched;

	//// END CALCULATE END ROUND SCORE

	// show the cards on the board and who played them
	printf("\n");
	for (int i = 0; i < numPlayers; i++) {
		printf("\n\tPlayer %d - %s (%d)", i + 1, roundCards[i].fullName, roundCards[i].value);
	}

	// calculate the score
	if (winnerIndex > -1) {
		printf("\n\n\tHighest Unique score: %d", highestUniqueScore);
		printf("\n\tRound WINNER is: Player %d", winnerIndex + 1);
		// increment the winning player's score
		gamePlayers[winnerIndex].score += highestUniqueScore;
	}
	else {
		printf("\nNo winner this round!\n");
	}

	// show the total score of each player
	printf("\n\nRound Scores");
	for (int i = 0; i < numPlayers; i++) {
		if (i == winnerIndex) {
			// if it's the winner, show the new points
			printf("\n\tPlayer %d: %d (+%d)", i + 1, gamePlayers[i].score, highestUniqueScore);
		}
		else {
			printf("\n\tPlayer %d: %d", i + 1, gamePlayers[i].score);
		}
	}
}

void showHighestScore() {
	// show the highest score
	int highestScore = 0;

	int highestScorePlayerIndexes[MAX_NUM_PLAYERS];

	for (int i = 0; i < numPlayers; i++) {
		highestScorePlayerIndexes[i] = -1;
		if (gamePlayers[i].score > highestScore) {
			highestScore = gamePlayers[i].score;
		}
	}

	int numLeaders = 0;
	printf("\n\nHighest Score:");
	for (int i = 0; i < numPlayers; i++) {

		if (gamePlayers[i].score == highestScore) {
			//printf("\n\tPlayer %d: %d", i + 1, gamePlayers[i].score);
			highestScore = gamePlayers[i].score;
			highestScorePlayerIndexes[i] = i;
			numLeaders += 1;
		}
		else {
			highestScorePlayerIndexes[i] = -1;
		}
	}

	for (int i = 0; i < numLeaders; i++) {

		// if an index matches
		if (i == highestScorePlayerIndexes[i]) {
			printf("\nPlayer %d is the winner with %d points\n\n", i + 1, gamePlayers[i].score);
		}
	}
}

void displayScore() {


}

void addPlayersToGame() {

	// sentinel
	int isValidSelection = 0;

	// get the number of players and set to global variable numPlayers
	printf("\nHow many players will play?");
	printf(" (Minimum 2, Maximum 10)");
	do {
		printf("\nEnter number of players: ");
		scanf("%d", &numPlayers);
		// if valid number of players
		if (numPlayers >= 2 && numPlayers <= MAX_NUM_PLAYERS) {
			// end loop
			isValidSelection = 1;
		}
	} while (isValidSelection == 0);

	// print the selection back to the user
	printf("%d players selected!", numPlayers);
	//char newName[100] = "";


	// now we know how many players, add each player
	int i;
	Player newPlayer;

	for (i = 0; i < numPlayers; i++) {
		// set the values
		newPlayer.score = 0;
		// give some feedback
		printf("\nPlayer %d Added!", i + 1);
		// and add the player to the gamePlayers array
		gamePlayers[i] = newPlayer;
	}

	printf("\nAll players added!\n");
}

//// CARD FUNCTIONS
// build an array of the suits
void buildSuits() {
	Suit suitHearts;
	suitHearts.id = 0;
	strcpy(suitHearts.name, "Hearts");
	gameSuits[0] = suitHearts;

	Suit suitClubs;
	suitClubs.id = 1;
	strcpy(suitClubs.name, "Clubs");
	gameSuits[1] = suitClubs;

	Suit suitDiamonds;
	suitDiamonds.id = 2;
	strcpy(suitDiamonds.name, "Diamonds");
	gameSuits[2] = suitDiamonds;

	Suit suitSpades;
	suitSpades.id = 3;
	strcpy(suitSpades.name, "Spades");
	gameSuits[3] = suitSpades;
}

// create the game deck which can be made up of 1-3 actual decks of cards
void buildGameDeck() {

	//srand(time(NULL));

	int suitId;
	int cardValue;
	int cardId = 0;
	//char cardFullName[100];

	int numCurrentDeck;

	buildSuits();

	// define a new card
	Card c;

	// game deck is built with multiple decks
	for (numCurrentDeck = 1; numCurrentDeck < numDecks + 1; numCurrentDeck++) {
		// loop through each suit in the suit list
		for (suitId = 0; suitId < NUM_SUITS; suitId++) {
			// add a new card, 1 - 13 to the gameDeck array
			for (cardValue = MIN_CARD_VALUE; cardValue <= MAX_CARD_VALUE; cardValue++) {
				// give the card its suit
				c.suit = gameSuits[suitId];
				// and give the card its value
				c.value = cardValue;

				// give the card a name based on its value
				switch (cardValue) {
				case 11:
					strcpy(c.cardName, "Jack");
					break;
				case 12:
					strcpy(c.cardName, "Queen");
					break;
				case 13:
					strcpy(c.cardName, "King");
					break;
				case 14:
					strcpy(c.cardName, "Ace");
					break;
				default:
					sprintf(c.cardName, "%d", c.value);
					//strcpy(c.cardName, c.value);
				}
				// set the cardId - also its location in the gameDeck
				c.cardId = cardId;
				c.isPlayed = 0;

				// create the full name string
				strcpy(c.fullName, c.cardName);
				strcat(c.fullName, " of ");
				strcat(c.fullName, c.suit.name);

				// set a default value of 0
				c.isDealtToPlayer = 0;

				// insert the card into the deck at the correct location
				gameDeck[cardId] = c;
				cardId++;
			}
		}
	}
}

Card getRandomCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize) {

	int numIsValid = 0;
	Card c;
	// random number
	int r;
	do {
		r = rand() % deckSize;
		if (gameDeck[r].isDealtToPlayer == 0) {
			numIsValid = 1;
			gameDeck[r].isDealtToPlayer = 1;
			c = gameDeck[r];
		}

	} while (numIsValid == 0);

	return c;
}

Card getRandomSuitedCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize, int suitId) {

	int suitIsEqual = 0;
	Card c;
	// random number
	int r;
	do {
		r = rand() % deckSize;
		if (gameDeck[r].isDealtToPlayer == 0 && gameDeck[r].suit.id == suitId) {
			suitIsEqual = 1;
			gameDeck[r].isDealtToPlayer = 1;
			c = gameDeck[r];
		}

	} while (suitIsEqual == 0);

	return c;
}

// performs the intial deal
void dealAllPlayersCards() {

	// player will be dealt four cards of the first suit they are dealt
	int suitId = 0; // may randomise this in future
	int numOfFirstSuit = 4;  // number of cards of the first dealt suit to be dealt

	for (int i = 0; i < numPlayers; i++) {
		for (int j = 0; j < NUM_ROUNDS; j++) {
			// first 4 cards will match suit
			if (j < numOfFirstSuit) {
				gamePlayers[i].cards[j] = getRandomSuitedCardFromDeck(gameDeck, CARDS_PER_DECK * numDecks, suitId);
			}
			// get completely random last 7 cards
			else {
				gamePlayers[i].cards[j] = getRandomCardFromDeck(gameDeck, CARDS_PER_DECK * numDecks);
			}
		}

		// increment the suitId - don't go past 
		if (suitId < 3) {
			suitId++;
		}
		else {
			suitId = 0;
		}
	}
	printf("\n\n\nCard dealt OK!!!!!!");
}

void printDeck() {
	// and test that the deck has built correctly
	for (int i = 0; i < numDecks * 52; i++) {
		printf("\n%s of %s: %d\n (isDealt = %d)", gameDeck[i].cardName, gameDeck[i].suit.name, gameDeck[i].value, gameDeck[i].isDealtToPlayer);
	}
}