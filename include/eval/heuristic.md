Two types of utility function: 

Also, they mention flipping for max/min player???

### Type 1: Table weight

Utility value = max player utility value - min player utility value

Table of weights: 

| 4  | -3 | 2 | 2 | 2 | 2 | -3 | 4 |
| -3 | -4 | -1 | -1 | -1 | -1 | -4 | -3 |
| 2  | -1 | 1 | 0 | 0 | 1 | -1 | 2 |
| 2  | -1 | 0 | 1 | 1 | 0 | -1 | 2 |
| 2  | -1 | 0 | 1 | 1 | 0 | -1 | 2 |
| 2  | -1 | 1 | 0 | 0 | 1 | -1 | 2 |
| -3 | -4 | -1 | -1 | -1 | -1 | -4 | -3 |
| 4  | -3 | 2 | 2 | 2 | 2 | -3 | 4 |


### Type 2: Component wise

Weight: 
corner = 30
mobility = 5
stability = 25
coin = 25

#### Coin parity

Coin par heuristic val = 100 * (max player - min player) / (max player + min player)

#### Mobility
- Two flavors: 
    + Actual mobility: number of legal moves
    + Potential mobility: number of empty spaces next to at least 1 opponent's coin

- For both mobility values, use the same loop: 

if (max player mobility val + min player mobility val != 0) 
    mobility heuristic val = 100 * (max player mob val - min player mob val ) / (max player mob val + min player mob val )
else
    mobility heuristic val = 0

#### Corners
- Types: 
    Corners captured
    Potential corner
    Unlikely corners

Same flavor
if (max player corner val + min player corner val != 0) 
    corner heuristic val = 100 * (max player corner val - min player corner val ) / (max player corner val + min player corner val )
else
    corner heuristic val = 0

#### Stability
- Types: stable, semi-stable, unstable
- Weights: stable 1, semi 0, unstable -1