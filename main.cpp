/*
 */

/* 
 * File:   main.cpp
 * Author: jorda
 *
 * Created on May 28, 2019, 8:49 PM
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ctime>

using namespace std;

class Card
{
public:
    enum rank
    {
        ACE = 1,
        TWO,
        THREE,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,
        TEN,
        JACK,
        QUEEN,
        KING
    };

    enum suit
    {
        CLUBS,
        DIAMONDS,
        HEARTS,
        SPADES
    };

    // overloading << operator so can send Card object to standard output
    friend ostream &operator<<(ostream &os, const Card &aCard);

    Card(rank r = ACE, suit s = SPADES, bool isFaceUp = true);

    // Return the value of the card, 1 - 11
    int GetCardValue() const;

    // Flips a card; if face up becomes face down and vice versa.
    void Flip();

private:
    rank cardRank;
    suit cardSuit;
    bool isFaceUp;
};

Card::Card(rank r, suit s, bool ifu) : cardRank(r), cardSuit(s), isFaceUp(ifu)
{
}

int Card::GetCardValue() const
{
    // if a cards is face down its value is 0
    int value = 0;
    if (isFaceUp == true)
    {
        // value is number showing on card
        value = cardRank;
        // Value is 10 for all face cards
        if (value > 10)
        {
            value = 10;
        }
    }
    return value;
}

void Card::Flip()
{
    isFaceUp = !(isFaceUp);
}

class Hand
{
public:
    Hand();
    virtual ~Hand();

    // Adds a card to the hand
    void AddCard(Card *cardPtr);

    // clears hand of all cards
    void Clear();

    // Gets total hand value, intelligently treats aces as 1 or 11
    int GetTotal() const;

protected:
    vector<Card *> listOfCards;
};

Hand::Hand()
{
    listOfCards.reserve(7);
}

Hand::~Hand()
{
    Clear();
}

void Hand::AddCard(Card *ptrCard)
{
    listOfCards.push_back(ptrCard);
}

void Hand::Clear()
{
    // Iterate through vector, freeing all memory on the heap
    vector<Card *>::iterator iter = listOfCards.begin();
    for (iter = listOfCards.begin(); iter != listOfCards.end(); iter++)
    {
        delete *iter;
        *iter = 0;
    }

    // Clear vectors of pointers
    listOfCards.clear();
}

int Hand::GetTotal() const
{
    // if there are not cards in the hand, return 0
    if (listOfCards.empty())
    {
        return 0;
    }

    // If a first has a value of 0, the it's face down, return 0
    if (listOfCards[0]->GetCardValue() == 0)
    {
        return 0;
    }

    // Add up card values, treat each ace as 1
    int total = 0;
    vector<Card *>::const_iterator iter;

    for (iter = listOfCards.begin(); iter != listOfCards.end(); iter++)
    {
        total += (*iter)->GetCardValue();
    }

    // determine if hand contains, if so treat it correctly
    bool hasAce = false;
    for (iter = listOfCards.begin(); iter != listOfCards.end(); iter++)
    {
        if ((*iter)->GetCardValue() == Card::ACE)
        {
            hasAce = true;
        }
    }

    // if hand contains ace and hand total is less than 11, treat the ace as an 11
    if ((hasAce == true) && (total <= 11))
    {
        // Add only 10 since we've already added the one for the ace
        total += 10;
    }

    return total;
}

class GenericPlayer : public Hand
{

    friend ostream &operator<<(ostream &os, const GenericPlayer &aGenericPlayer);

public:
    GenericPlayer(const string &name = "");

    virtual ~GenericPlayer();

    // indicates if the GenericPlayer wants another card hit
    virtual bool IsHitting() const = 0;

    // indicates if the GenericPlayer has busted (has a value greater than 21)
    bool IsBusted() const;

    // Announces that the GenericPlayer has busted.
    void Bust() const;

protected:
    string name;
};

GenericPlayer::GenericPlayer(const string &name) : name(name)
{
}

GenericPlayer::~GenericPlayer()
{
}

bool GenericPlayer::IsBusted() const
{
    return (GetTotal() > 21);
}

void GenericPlayer::Bust() const
{
    cout << name << " has bust.\n"
         << endl;
}

class Player : public GenericPlayer
{
public:
    Player(const string &name = "");

    virtual ~Player();

    // returns whether or not the player wants another hit
    virtual bool IsHitting() const;

    // Announces that the player wins
    void Win() const;

    // announces that the player lose
    void Lose() const;

    // Announces that player pushes
    void Push() const;
};

Player::Player(const string &name) : GenericPlayer(name)
{
}

Player::~Player()
{
}

bool Player::IsHitting() const
{
    cout << name << ", do you want a hit? (Y/N): ";
    char reponse;
    cin >> reponse;
    return (reponse == 'y' || reponse == 'Y');
}

void Player::Win() const
{
    cout << name << " wins. \n"
         << endl;
}

void Player::Lose() const
{
    cout << name << " loses.\n"
         << endl;
}

void Player::Push() const
{
    cout << name << " pushes.\n"
         << endl;
}

class House : public GenericPlayer
{
public:
    House(const string &name = "House");
    virtual ~House();

    // Indicates whether house is hitting - will always hit on 16 or less
    virtual bool IsHitting() const;

    // Flips over first card
    void FlipFirstCard();
};

House::House(const string &name) : GenericPlayer(name)
{
}

House::~House()
{
}

bool House::IsHitting() const
{
    return (GetTotal() <= 16);
}

void House::FlipFirstCard()
{
    if (!(listOfCards.empty()))
    {
        listOfCards[0]->Flip();
    }
    else
    {
        cout << "There is no card to flip\n"
             << endl;
    }
}

class Deck : public Hand
{
public:
    Deck();
    virtual ~Deck();

    // Create a standard decks 52 cards
    void Populate();

    // Shuffle Cards
    void Shuffle();

    // Deal one card to hand
    void Deal(Hand &someHand);

    // Give additional cards to generic player
    void AdditionalCards(GenericPlayer &someGenericPlayer);
};

Deck::Deck()
{
    listOfCards.reserve(52);
    Populate();
}

Deck::~Deck()
{
}

void Deck::Populate()
{
    Clear();

    // create standard deck
    for (int suit = Card::CLUBS; suit <= Card::SPADES; suit++)
    {
        for (int rank = Card::ACE; rank <= Card::KING; rank++)
        {
            AddCard(new Card(static_cast<Card::rank>(rank), static_cast<Card::suit>(suit)));
        }
    }
}

void Deck::Shuffle()
{
    random_shuffle(listOfCards.begin(), listOfCards.end());
}

void Deck::Deal(Hand &someHand)
{
    if (!listOfCards.empty())
    {
        someHand.AddCard(listOfCards.back());
        listOfCards.pop_back();
    }
    else
    {
        cout << "Out of cards. Unable to deal.";
    }
}

void Deck::AdditionalCards(GenericPlayer &someGenericPlayer)
{
    cout << endl;

    // Continue to deal a card as long as generic player isn't busted and wants another hit
    while (!(someGenericPlayer.IsBusted()) && someGenericPlayer.IsHitting())
    {
        Deal(someGenericPlayer);
        cout << someGenericPlayer << endl;

        if (someGenericPlayer.IsBusted())
        {
            someGenericPlayer.Bust();
        }
    }
}

class Game
{
public:
    Game(const vector<string> &listOfName);

    ~Game();

    // Plays the game Of Blackjack
    void Play();

private:
    Deck gameDeck;
    House gameHouse;
    vector<Player> listOfPlayers;
};

Game::Game(const vector<string> &listOfNames)
{
    // Create a Vector of players from a vector of names
    vector<string>::const_iterator namePtr;

    for (namePtr = listOfNames.begin(); namePtr != listOfNames.end(); namePtr++)
    {
        listOfPlayers.push_back(Player(*namePtr));
    }

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));
    gameDeck.Populate();
    gameDeck.Shuffle();
}

Game::~Game()
{
}

void Game::Play()
{
    // deal initial 2 cards to everyone
    vector<Player>::iterator playerPtr;
    for (int i = 0; i < 2; i++)
    {
        for (playerPtr = listOfPlayers.begin(); playerPtr != listOfPlayers.end(); playerPtr++)
        {
            gameDeck.Deal(*playerPtr);
        }
        gameDeck.Deal(gameHouse);
    }

    // Hide House First Card
    gameHouse.FlipFirstCard();

    // display everyone's hand
    for (playerPtr = listOfPlayers.begin(); playerPtr != listOfPlayers.end(); playerPtr++)
    {
        cout << *playerPtr << endl;
    }

    // deal additional cards to players
    for (playerPtr = listOfPlayers.begin(); playerPtr != listOfPlayers.end(); playerPtr++)
    {
        gameDeck.AdditionalCards(*playerPtr);
    }

    // reveal house's first card
    gameHouse.FlipFirstCard();
    cout << gameHouse << endl;

    // Deal additional cards to house
    gameDeck.AdditionalCards(gameHouse);

    if (gameHouse.IsBusted())
    {
        // Everyone still playing wins the game
        for (playerPtr = listOfPlayers.begin(); playerPtr != listOfPlayers.end(); playerPtr++)
        {
            if (!(playerPtr->IsBusted()))
            {
                playerPtr->Win();
            }
        }
    }
    else
    {
        // Compare each player still playing to the house
        for (playerPtr = listOfPlayers.begin(); playerPtr != listOfPlayers.end(); playerPtr++)
            if (!(playerPtr->IsBusted()))
            {
                if (playerPtr->GetTotal() > gameHouse.GetTotal())
                {
                    playerPtr->Win();
                }
                else if (playerPtr->GetTotal() < gameHouse.GetTotal())
                {
                    playerPtr->Lose();
                }
                else
                {
                    playerPtr->Push();
                }
            }
    }

    for (playerPtr = listOfPlayers.begin(); playerPtr != listOfPlayers.end(); playerPtr++)
    {
        playerPtr->Clear();
    }
    gameHouse.Clear();
}

// function prototypes
ostream &operator<<(ostream &os, const Card &aCard);
ostream &operator<<(ostream &os, const GenericPlayer &aGenericPlayer);

int main(int argc, char **argv)
{
    cout << "\t\tWelcome to Hotel Hermes, let's play some Blackjack\n\n";

    int numPlayers = 0;

    while (numPlayers < 1 || numPlayers > 7)
    {
        cout << "How many player? (1-7): ";
        cin >> numPlayers;
    }

    vector<string> listOfNames;
    string name;

    for (int i = 0; i < numPlayers; i++)
    {
        cout << "Enter player name: ";
        cin >> name;
        listOfNames.push_back(name);
    }

    cout << endl;

    // the game loop
    Game theGame(listOfNames);

    char again = 'y';
    while (again != 'n' && again != 'N')
    {
        theGame.Play();
        cout << "\nDo you want to play again? (Y/N): ";
        cin >> again;
    }

    return 0;
}

// overlord << operator so Card object can be sent to cout
ostream &operator<<(ostream &os, const Card &aCard)
{
    const string RANKS[] = {"0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    const string SUITS[] = {"Clubs", "Diamonds", "Hearts", "Spades"};

    if (aCard.isFaceUp)
    {
        os << RANKS[aCard.cardRank] << " " << SUITS[aCard.cardSuit];
    }
    else
    {
        os << "XX";
    }

    return os;
}

// overlord << operator so a GenericPlayer object can be sent to output
ostream &operator<<(ostream &os, const GenericPlayer &aGenericPlayer)
{
    os << aGenericPlayer.name << ":\t";

    vector<Card *>::const_iterator cardPtr;
    if (!aGenericPlayer.listOfCards.empty())
    {
        for (cardPtr = aGenericPlayer.listOfCards.begin(); cardPtr != aGenericPlayer.listOfCards.end(); cardPtr++)
        {
            os << *(*cardPtr) << "\t";
        }

        if (aGenericPlayer.GetTotal() != 0)
        {
            cout << "Total Card Value: [-- " << aGenericPlayer.GetTotal() << " --]";
        }
    }
    else
    {
        os << "<empty>";
    }
    return os;
}