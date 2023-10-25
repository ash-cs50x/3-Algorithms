#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

//My own prototypes.
void print_preferences(void);
void print_pairs();
bool cycle_check(int starting_point, int end_point);
void print_locked(void);
bool isLock(int i);
bool hasLock(int j);


int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    //print_preferences();
    add_pairs();
    //print_pairs();
    sort_pairs();
    //printf("Sorted pairs\n");
    //print_pairs();
    lock_pairs();
    //printf("Pairs locked\n");
    //printf("Printing locked...\n");
    //print_locked();
    print_winner();
    return 0;
}

// Update ranks given a new vote
// The ranks array is an array of ints representing the voter's ranks. The candidates[] array is an array of strings.
// This function finds the corresponding index from candidates[] and records it in ranks[].
bool vote(int rank, string name, int ranks[])
{
    int i = 0;
    while (i < candidate_count)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;

            return true;
        }
        i++;
    }
    return false;
}

// Update preferences given one voter's ranks
// If a candidate is in the ranks[i] position, then that candidate is preferred over all the ranks[j] candidates (starting with j = i + 1).
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        int x = ranks[i];
        for (int j = i + 1; j < candidate_count; j++)
        {
            int y = ranks[j];
            preferences[x][y] += 1;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
// preferences[] is a square matrix. We only need to compare the pairs in the upper triangle (without the diagonal). The lower triangle is just the same pairs in reverse.
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            int prefer_i = preferences[i][j];
            int prefer_j = preferences[j][i];

            //printf("The number that prefer %i is %i\n", i, prefer_i);
            //printf("The number that prefer %i is %i\n", j, prefer_j);

            if (prefer_i > prefer_j)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;

                pair_count++;

                //printf("Created pair. %i --> %i\n", pairs[pair_count - 1].winner, pairs[pair_count - 1].loser);
            }

            else if (prefer_j > prefer_i)
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;

                pair_count++;

                //printf("Created pair. %i --> %i\n", pairs[pair_count - 1].winner, pairs[pair_count - 1].loser);
            }

        }
    }

    //printf("pair count is %i\n", pair_count);
    return;
}

void print_preferences(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%i | ", preferences[i][j]);
        }
        printf("\n");
        printf("------------------------------------------------\n");
    }
}

void print_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        printf("Pair %i (W --> L) : ", i + 1);
        printf("%i --> %i ", pairs[i].winner, pairs[i].loser);
        printf("STR: %i\n", preferences[pairs[i].winner][pairs[i].loser]);
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Sort pairs using bubble sort (for the fun of it).

    bool isSorted = false;

    int sizeUnsorted = pair_count;

    pair dummyPair = pairs[0];


    while (!isSorted)
    {
        isSorted = true;
        for (int i = 0; i < sizeUnsorted - 1; i++)
        {
            pair PairA = pairs[i];
            pair PairB = pairs[i + 1];

            int strPairA = preferences[PairA.winner][PairA.loser];
            int strPairB = preferences[PairB.winner][PairB.loser];

            if (strPairA < strPairB)
            {
                dummyPair = PairA;
                pairs[i] = PairB;
                pairs[i + 1] = dummyPair;

                isSorted = false;
            }
        }
        sizeUnsorted -= 1;
    }


    return;
}


// Checks if locking the pair will create a cycle. Returns true if a cycle would be created.
bool cycle_check(int starting_point, int end_point)
{
    // Will locking A --> B create a cycle?


    for (int j = 0; j < candidate_count; j++)
    {
        if (locked[end_point][j] == true)       // Is B locked over any candidate j?
        {
            //printf("A lock exists: %i --> %i\n", end_point, j);

            if (j == starting_point)            // If so, is it locked over A?
            {
                return true;                    // Then a cycle will be created
            }
            else                                // No, it's locked over another node.
            {
                if (cycle_check(starting_point, j) == true) // Recursively check if that node leads back to A
                {
                    return true;
                }

            }

        }
        else                                    // B is not locked over j, check a different branch
        {
            //printf("%i is not locked over %i\n", end_point, j);
        }
    }

    return false;                               // If B is not locked over any other candidate (no branches), then no cycle will be created
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        //printf("Checking if %i -- > %i will create a cycle....\n", pairs[i].winner, pairs[i].loser);
        bool isCycle = cycle_check(pairs[i].winner, pairs[i].loser);

        if (isCycle == false)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
            //printf("Locked %i --> %i\n", pairs[i].winner, pairs[i].loser);
        }
        else if (isCycle == true)
        {
            //printf("A cycle would be created. Going to next pair...\n");
        }
    }
    return;
}

void print_locked(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%i ", locked[i][j]);
            printf("| ");
        }
        printf("\n");
        printf("-----------------\n");
    }

    return;
}

bool isLocked(int i)
{
    for (int j = 0; j < candidate_count; j++)
    {
        if (j == i)
        {
            continue;
        }

        else if (locked[i][j] == true)
        {
            //printf("%i is locked over %i\n", i, j);
            return true;
        }
    }

    return false;
}

bool hasLock(int i)
{
    for (int k = 0; k < candidate_count; k++)
    {
        if (k == i)
        {
            continue;
        }

        else if (locked[k][i] == true)
        {
            //printf("%i is locked over %i. %i cannot be the source\n", k, i, i);
            return true;
        }
    }
    return false;
}


// Print the winner of the election
void print_winner(void)
{
    // The winner is the source - the candidate which has at least one lock over another candidate, but no locks over itself.

    for (int i = 0; i < candidate_count; i++)
    {
        //printf("i is %i\n", i);
        if (isLocked(i))
        {
            if (hasLock(i))
            {
                continue;
            }
            else
            {
                printf("%s\n", candidates[i]); // Winner winner, Chicken dinner!
                return;
            }
        }
        else
        {
            //printf("%i is not locked over anything, going to next i...\n", i);
        }
    }
    return;
}

