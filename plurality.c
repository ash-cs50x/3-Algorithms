#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
}
candidate;

// Array of candidates
candidate candidates[MAX];

// Number of candidates
int candidate_count;

// Function prototypes
bool vote(string name);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
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
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
    }

    int voter_count = get_int("Number of voters: ");

    // Loop over all voters
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: ");

        // Check for invalid vote
        if (!vote(name))
        {
            printf("Invalid vote.\n");
        }
    }

    // Display winner of election
    print_winner();
}

// Update vote totals given a new vote
bool vote(string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)
        {
            candidates[i].votes += 1;
            return true;
        }
    }
    return false;
}

// Print the winner (or winners) of the election
void print_winner(void)
{
    //Let's do a selection sort.
    //For each element in the votes array, start by saying that's the smallest number.
    //Loop through the rest of the array searching for one that's smaller,
    //setting that number as the smallest number each time.
    //If you find the smallest number in the rest of the array, swap the element with that number.
    //Otherwise leave that element alone and move to the next one.

    for (int i = 0; i < (candidate_count - 1); i++)
    {

        int fewest = candidates[i].votes;
        int s = i;

        for (int j = i + 1; j < candidate_count; j++)
        {

            if (candidates[j].votes < fewest)
            {
                fewest = candidates[j].votes;
                s = j;
            }
        }
        //If the candidate with the fewest votes is not candidate i
        if (s != i)
        {
            candidate c = candidates[s];
            candidates[s] = candidates[i];
            candidates[i] = c;
        }

    }
    //The candidates should now be sorted from fewest votes to most votes.
    //Now we need to print all candidates with the max votes.

    //What is the number of votes received by the candidate with the most votes?
    int most = candidates[candidate_count - 1].votes;

    //Iterate over the list of candidates and print the name(s) of the candidate(s)
    //if they have "most" votes.
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == most)
        {
            printf("%s\n", candidates[i].name);
        }
    }

    return;
}

