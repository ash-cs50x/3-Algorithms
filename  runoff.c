#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    // name is a string. We need to convert it to the corresponding index of the
    // candidate in the candidates array. Loop over the list of candidates using
    // strcmp. If you find a match with name and candidates[i].name, the counter
    // value i corresponds to the index of the candidate. We set that candidate
    // as the jth preference for voter i.
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)
        {
            preferences[voter][rank] = i;
            return true;
        }
    }
    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    for (int i = 0; i < voter_count; i++)
    {
        // For each voter, go through their array of preferences.
        // Check if their jth choice is eliminated. If so go to the next jth preference.
        // If the jth choice (k) is not eliminated, add a vote to the candidate then
        // terminate the loop. This way we are only counting the vote for the first
        // non-eliminated choice.
        for (int j = 0; j < candidate_count; j++)
        {
            int k = preferences[i][j];
            if (!candidates[k].eliminated)
            {
                candidates[k].votes ++;
                break;
            }
        }
    }
    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        // If a candidate has received over 50% of votes in this round
        // then they have won the plurality and are declared the winner.
        if (candidates[i].votes > (voter_count / 2))
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }
    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{
    // The most votes a candidate can have is the number of voters voting
    int minvotes = voter_count;

    // Loop over the list of candidates. If the candidate has not been eliminated,
    // and they have fewer votes than the current minimum votes (minvotes),
    // update minvotes to that value.
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated == false && candidates[i].votes < minvotes)
        {
            minvotes = candidates[i].votes;
        }
    }


    return minvotes;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    bool tiestatus = false;
    for (int i = 0; i < candidate_count; i++)
    {
        // Check if the candidate has not been eliminated.
        if (candidates[i].eliminated == false)
        {
            // If the candidate has 'min' votes, set tie status to true.
            // If the loop completes without this conditional being false
            // then there must be a tie between all candidates.
            if (candidates[i].votes == min)
            {
                tiestatus = true;

            }

            // If a candidate is found who is not eliminated and has
            // something other than 'min' votes, then set tie status to false
            // and stop executing the loop.
            else if (candidates[i].votes != min)
            {
                tiestatus = false;
                break;
            }
        }
    }
    return tiestatus;
}

// Eliminate the candidate (or candidiates) in last place
void eliminate(int min)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].eliminated == false && candidates[i].votes == min)
        {
            candidates[i].eliminated = true;
        }
    }
    return;
}
