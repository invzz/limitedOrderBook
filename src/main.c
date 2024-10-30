#include <time.h>
#include <book.h>

// Initialize market with an array of order books for multiple assets
OrderBook market[NUM_ASSETS];

// Function to simulate random orders
void simulate_market(int num_orders)
{
    srand(time(NULL));
    int timestamp = 1;

    for (int i = 0; i < num_orders; i++)
    {
        int asset_id = rand() % NUM_ASSETS;                       // Randomly pick an asset
        double price = 90 + rand() % 20 + (rand() % 100) / 100.0; // Random price between 90 and 110
        int quantity = (rand() % 10) + 1;                         // Random quantity between 1 and 10
        int is_bid = rand() % 2;                                  // Randomly pick bid (1) or ask (0)

        printf("\n--- Order %d ---\n", i + 1);
        add_order_to_book(&market[asset_id], i + 1, price, quantity, timestamp++, is_bid);
    }
}

// Main function
int main()
{
    // Initialize empty market for each asset
    for (int i = 0; i < NUM_ASSETS; i++)
    {
        market[i].bids = NULL;
        market[i].asks = NULL;
    }

    printf("Simulating market with %d assets and 20 random orders...\n", NUM_ASSETS);
    simulate_market(20);

    return 0;
}